// SPDX-License-Identifier: GPL-2.0-only
/*
 * Google Tensor G6 (malibu) display controller
 *
 * The display processing unit is a VeriSilicon DC9400. This driver supports
 * a minimal pipeline: one linear RGB layer on display 0, sent to output 0
 * and the DSI0 host, optionally through the DSC encoder, for command mode
 * panels. Each commit pushes one frame with a software start-of-frame
 * trigger and completes on the frame done interrupt.
 *
 * Programming model: most registers are shadowed. Updates are made with the
 * layer, panel and output shadow switches off and take effect when they are
 * switched back on and a frame is started.
 *
 * Based on the downstream Pixel driver.
 *
 * Copyright (C) 2023 VeriSilicon Holdings Co., Ltd.
 * Copyright 2025 Google LLC
 */

#include <linux/bitops.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/math.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/of_graph.h>
#include <linux/platform_device.h>
#include <linux/pm_domain.h>
#include <linux/spinlock.h>
#include <linux/units.h>

#include <drm/clients/drm_client_setup.h>
#include <drm/display/drm_dsc.h>
#include <drm/drm_atomic.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_bridge.h>
#include <drm/drm_crtc.h>
#include <drm/drm_drv.h>
#include <drm/drm_encoder.h>
#include <drm/drm_fb_dma_helper.h>
#include <drm/drm_fbdev_dma.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_framebuffer.h>
#include <drm/drm_gem_dma_helper.h>
#include <drm/drm_gem_framebuffer_helper.h>
#include <drm/drm_managed.h>
#include <drm/drm_of.h>
#include <drm/drm_plane.h>
#include <drm/drm_probe_helper.h>
#include <drm/drm_vblank.h>

#include "mbu_dpu_regs.h"
#include "mbu_dsi.h"

#define MBU_DPU_MAX_WIDTH		4096
#define MBU_DPU_MAX_HEIGHT		4096

/* Outstanding read transactions of layer 0 */
#define MBU_DPU_LAYER_OT_NUMBER		127

/* Reference clock of the underrun counters, and margin of the line time */
#define MBU_DPU_REF_CLK_MHZ		100
#define MBU_DPU_UNDERRUN_MARGIN_PCT	99

/* DSC encoder of display 0: APB registers, one per dword */
#define DSC_HS0				0x40
#define DSC_ENC_MAIN_CONF		0x000
#define DSC_HS_DF_CTRL			0x000
#define DSC_HS_MAIN_CONF		0x00c
#define DSC_HS_PICT_SIZE		0x00d
#define DSC_HS_SLICE_SIZE		0x00e
#define DSC_HS_MISC_SIZE		0x00f
#define DSC_HS_HRD_DELAY		0x010
#define DSC_HS_RC_SCALE			0x011
#define DSC_HS_RC_SCALE_INC_DEC		0x012
#define DSC_HS_RC_OFFSETS_1		0x013
#define DSC_HS_RC_OFFSETS_2		0x014
#define DSC_HS_RC_OFFSETS_3		0x015
#define DSC_HS_RC_OFFSETS_4		0x016
#define DSC_HS_FLATNESS_QP		0x017
#define DSC_HS_RC_MODEL_SIZE		0x018
#define DSC_HS_RC_CONFIG		0x019
#define DSC_HS_RC_BUF_THRESH(n)		(0x01a + (n))
#define DSC_HS_RC_MIN_QP(n)		(0x01e + (n))
#define DSC_HS_RC_MAX_QP(n)		(0x021 + (n))
#define DSC_HS_RC_BPG_OFFSETS(n)	(0x024 + (n))

/* Encoder capabilities */
#define DSC_NB_HS_ENC			2
#define DSC_MAX_PIXELS_HS_LINE		4096

struct mbu_dpu {
	struct drm_device drm;
	struct device *dev;
	void __iomem *regs;

	struct drm_plane plane;
	struct drm_crtc crtc;
	struct drm_encoder encoder;
	struct device_node *dsi_np;

	/* Protects @event and the interrupt enable register. */
	spinlock_t lock;
	struct drm_pending_vblank_event *event;
};

static struct mbu_dpu *to_mbu_dpu(struct drm_device *drm)
{
	return container_of(drm, struct mbu_dpu, drm);
}

static void dpu_write(struct mbu_dpu *dpu, u32 reg, u32 val)
{
	writel(val, dpu->regs + reg);
}

static u32 dpu_read(struct mbu_dpu *dpu, u32 reg)
{
	return readl(dpu->regs + reg);
}

static u32 dpu_field(u32 mask, u32 val)
{
	return (val << __ffs(mask)) & mask;
}

static void dpu_update(struct mbu_dpu *dpu, u32 reg, u32 mask, u32 val)
{
	u32 tmp = dpu_read(dpu, reg);

	dpu_write(dpu, reg, (tmp & ~mask) | dpu_field(mask, val));
}

static void dpu_shadow_enable(struct mbu_dpu *dpu, bool enable)
{
	dpu_update(dpu, DCREG_LAYER0_CONFIG, DCREG_LAYER0_CONFIG_REG_SWITCH, enable);
	dpu_update(dpu, DCREG_PANEL0_CONFIG, DCREG_PANEL0_CONFIG_REG_SWITCH, enable);
	dpu_update(dpu, DCREG_OUTPUT0_CONFIG, DCREG_OUTPUT0_CONFIG_REG_SWITCH, enable);
}

/* DSC */

struct dsc_usage {
	u32 slices_per_line;
	u32 ss_num;
	u32 initial_lines;
	u32 ob_max_addr;
	u32 drb_max_addr;
};

static void dsc_write(struct mbu_dpu *dpu, u32 dword, u32 val)
{
	dpu_write(dpu, DCREG_PANEL0_DSC + dword * 4, val);
}

static u32 dsc_initial_lines(const struct drm_dsc_config *dsc, const struct dsc_usage *u)
{
	/* Latencies of the encoder pipeline, from the IP documentation */
	const u32 pipeline_latency = 28, ssm_delay = 91, ob_data_width = 128;
	u32 bpp = dsc->bits_per_pixel >> 4;
	u32 hs_num = u->slices_per_line / u->ss_num;
	u32 chunk_bits = dsc->slice_chunk_size * 8;
	u32 input_latency = pipeline_latency + 3 * (ssm_delay + 2) * u->ss_num;
	u32 obuf_latency = DIV_ROUND_UP(9 * ob_data_width + dsc->mux_word_size, bpp) + 1;
	u32 base_latency = dsc->initial_xmit_delay + input_latency + obuf_latency;
	/* Split panel, multiplexed and de-rasterised on a single link */
	u32 extra_bits = DIV_ROUND_UP((hs_num - 1) * chunk_bits, hs_num);
	u32 extra_latency = DIV_ROUND_UP(extra_bits, bpp) + 5;

	return DIV_ROUND_UP(base_latency + extra_latency, dsc->slice_width);
}

static u32 dsc_deraster_buffer(const struct drm_dsc_config *dsc, const struct dsc_usage *u)
{
	u32 hs_num = u->slices_per_line / u->ss_num;
	u32 width = min_t(u32, dsc->slice_width * u->ss_num, DSC_MAX_PIXELS_HS_LINE);

	return round_down(width * (hs_num - 1) / (hs_num * DSC_NB_HS_ENC), 2) + 3;
}

static void dpu_dsc_setup(struct mbu_dpu *dpu, const struct drm_dsc_config *dsc)
{
	const struct drm_dsc_rc_range_parameters *rp = dsc->rc_range_params;
	const u16 *thr = dsc->rc_buf_thresh;
	struct dsc_usage u = { };
	u32 hs = DSC_HS0, val;
	int i;

	u.slices_per_line = dsc->slice_count ?: dsc->pic_width / dsc->slice_width;
	u.ss_num = max_t(u32, u.slices_per_line / DSC_NB_HS_ENC, 1);
	u.initial_lines = dsc_initial_lines(dsc, &u);
	u.ob_max_addr = u.ss_num == 1 ? 919 : 459;
	u.drb_max_addr = dsc_deraster_buffer(dsc, &u);

	dpu_update(dpu, DCREG_SH_PANEL0_SPLIT_CONFIG, DCREG_SH_PANEL0_SPLIT_CONFIG_SOURCE,
		   DCREG_SH_PANEL0_SPLIT_CONFIG_SOURCE_DSC);

	/* Split panel, multiplexed output, de-rasterisation buffer enabled */
	dsc_write(dpu, DSC_ENC_MAIN_CONF,
		  BIT(0) | BIT(1) | BIT(6) | u.ss_num << 7 | u.drb_max_addr << 18);
	/* Command mode, ICH enabled */
	dsc_write(dpu, hs | DSC_HS_DF_CTRL, u.initial_lines | u.ob_max_addr << 18);

	dsc_write(dpu, hs | DSC_HS_MAIN_CONF,
		  dsc->bits_per_component | dsc->convert_rgb << 4 |
		  dsc->simple_422 << 5 | dsc->line_buf_depth << 6 |
		  dsc->bits_per_pixel << 10 | dsc->block_pred_enable << 20 |
		  dsc->native_420 << 21 | dsc->native_422 << 22 |
		  dsc->dsc_version_minor << 28);
	dsc_write(dpu, hs | DSC_HS_PICT_SIZE, dsc->pic_width | dsc->pic_height << 16);
	dsc_write(dpu, hs | DSC_HS_SLICE_SIZE, dsc->slice_width | dsc->slice_height << 16);
	dsc_write(dpu, hs | DSC_HS_MISC_SIZE, dsc->slice_chunk_size);
	dsc_write(dpu, hs | DSC_HS_HRD_DELAY,
		  dsc->initial_xmit_delay | dsc->initial_dec_delay << 16);
	dsc_write(dpu, hs | DSC_HS_RC_SCALE, dsc->initial_scale_value);
	dsc_write(dpu, hs | DSC_HS_RC_SCALE_INC_DEC,
		  dsc->scale_increment_interval | dsc->scale_decrement_interval << 16);
	dsc_write(dpu, hs | DSC_HS_RC_OFFSETS_1,
		  dsc->first_line_bpg_offset | dsc->second_line_bpg_offset << 5);
	dsc_write(dpu, hs | DSC_HS_RC_OFFSETS_2,
		  dsc->nfl_bpg_offset | dsc->slice_bpg_offset << 16);
	dsc_write(dpu, hs | DSC_HS_RC_OFFSETS_3,
		  dsc->initial_offset | dsc->final_offset << 16);
	dsc_write(dpu, hs | DSC_HS_RC_OFFSETS_4,
		  dsc->nsl_bpg_offset | dsc->second_line_offset_adj << 16);
	dsc_write(dpu, hs | DSC_HS_FLATNESS_QP,
		  dsc->flatness_min_qp | dsc->flatness_max_qp << 5 |
		  (2 << (dsc->bits_per_component - 8)) << 10);
	dsc_write(dpu, hs | DSC_HS_RC_MODEL_SIZE, dsc->rc_model_size);
	dsc_write(dpu, hs | DSC_HS_RC_CONFIG,
		  dsc->rc_edge_factor | dsc->rc_quant_incr_limit0 << 8 |
		  dsc->rc_quant_incr_limit1 << 13 | dsc->rc_tgt_offset_high << 20 |
		  dsc->rc_tgt_offset_low << 24);

	for (i = 0; i < 4; i++) {
		val = thr[i * 4] | thr[i * 4 + 1] << 8;
		if (i < 3)
			val |= thr[i * 4 + 2] << 16 | thr[i * 4 + 3] << 24;
		dsc_write(dpu, hs | DSC_HS_RC_BUF_THRESH(i), val);
	}

	for (i = 0; i < 3; i++) {
		const struct drm_dsc_rc_range_parameters *p = &rp[i * 5];

		dsc_write(dpu, hs | DSC_HS_RC_MIN_QP(i),
			  p[0].range_min_qp | p[1].range_min_qp << 5 |
			  p[2].range_min_qp << 10 | p[3].range_min_qp << 15 |
			  p[4].range_min_qp << 20);
		dsc_write(dpu, hs | DSC_HS_RC_MAX_QP(i),
			  p[0].range_max_qp | p[1].range_max_qp << 5 |
			  p[2].range_max_qp << 10 | p[3].range_max_qp << 15 |
			  p[4].range_max_qp << 20);
		dsc_write(dpu, hs | DSC_HS_RC_BPG_OFFSETS(i),
			  (p[0].range_bpg_offset & 0x3f) | (p[1].range_bpg_offset & 0x3f) << 6 |
			  (p[2].range_bpg_offset & 0x3f) << 12 |
			  (p[3].range_bpg_offset & 0x3f) << 18 |
			  (p[4].range_bpg_offset & 0x3f) << 24);
	}
}

/* Display 0 and output 0 */

static void dpu_display_setup(struct mbu_dpu *dpu, const struct drm_display_mode *mode,
			      const struct drm_dsc_config *dsc)
{
	u32 fps = drm_mode_vrefresh(mode);
	u32 line_cycles;

	if (dsc)
		dpu_dsc_setup(dpu, dsc);
	else
		dpu_update(dpu, DCREG_SH_PANEL0_SPLIT_CONFIG,
			   DCREG_SH_PANEL0_SPLIT_CONFIG_SOURCE,
			   DCREG_SH_PANEL0_SPLIT_CONFIG_SOURCE_PIPE);

	dpu_write(dpu, DCREG_SH_OUTPUT0_CLK_EN, 1);
	dpu_write(dpu, DCREG_SH_PANEL0_FORMAT,
		  dpu_field(DCREG_SH_PANEL0_FORMAT_OUTPUT_FORMAT,
			    DCREG_SH_PANEL0_FORMAT_OUTPUT_FORMAT_RGB888));
	dpu_write(dpu, DCREG_SH_OUTPUT0_IPI_FORMAT,
		  dsc ? DCREG_SH_OUTPUT0_IPI_FORMAT_VALUE_COMPRESS_DATA :
			DCREG_SH_OUTPUT0_IPI_FORMAT_VALUE_RGB);
	dpu_write(dpu, DCREG_SH_OUTPUT0_IPI_COLOR_DEPTH,
		  DCREG_SH_OUTPUT0_IPI_COLOR_DEPTH_VALUE_BITS8);

	/* Timing, sync pulses active high unless the mode says otherwise */
	dpu_write(dpu, DCREG_OUTPUT0_TIMING_HSYNC, !!(mode->flags & DRM_MODE_FLAG_NHSYNC));
	dpu_write(dpu, DCREG_SH_OUTPUT0_TIMING_HS_WIDTH, mode->hsync_end - mode->hsync_start);
	dpu_write(dpu, DCREG_SH_OUTPUT0_TIMING_HBP_WIDTH, mode->htotal - mode->hsync_end);
	dpu_write(dpu, DCREG_SH_OUTPUT0_TIMING_HA_WIDTH, mode->hdisplay);
	dpu_write(dpu, DCREG_SH_OUTPUT0_TIMING_HFP_WIDTH, mode->hsync_start - mode->hdisplay);
	dpu_write(dpu, DCREG_OUTPUT0_TIMING_VSYNC, !!(mode->flags & DRM_MODE_FLAG_NVSYNC));
	dpu_write(dpu, DCREG_SH_OUTPUT0_TIMING_VS_HEIGHT, mode->vsync_end - mode->vsync_start);
	dpu_write(dpu, DCREG_SH_OUTPUT0_TIMING_VBP_HEIGHT, mode->vtotal - mode->vsync_end);
	dpu_write(dpu, DCREG_SH_OUTPUT0_TIMING_VA_HEIGHT, mode->vdisplay);
	dpu_write(dpu, DCREG_SH_OUTPUT0_TIMING_VFP_HEIGHT, mode->vsync_start - mode->vdisplay);

	/* Command mode, frames started by software */
	dpu_update(dpu, DCREG_OUTPUT0, DCREG_OUTPUT0_WORK_MODE, DCREG_OUTPUT0_WORK_MODE_COMMAND);
	dpu_update(dpu, DCREG_OUTPUT0_CONFIG_COMMAND_OPT, DCREG_OUTPUT0_CONFIG_COMMAND_OPT_SYNC,
		   DCREG_OUTPUT0_CONFIG_COMMAND_OPT_SYNC_DISABLED);
	dpu_update(dpu, DCREG_OUTPUT0_CONFIG_COMMAND_OPT, DCREG_OUTPUT0_CONFIG_COMMAND_OPT_OPTION,
		   DCREG_OUTPUT0_CONFIG_COMMAND_OPT_OPTION_TRIGGER_MODE);
	dpu_write(dpu, DCREG_OUTPUT0_DE_SYNC_MODE, 0);

	/* Underrun detection: one line must be scanned out in time */
	line_cycles = mult_frac(USEC_PER_SEC / (fps ?: 60),
				MBU_DPU_REF_CLK_MHZ * MBU_DPU_UNDERRUN_MARGIN_PCT,
				100 * mode->vdisplay);
	dpu_write(dpu, DCREG_SH_OUTPUT0_SCANOUT_COUNTER,
		  dpu_field(DCREG_SH_OUTPUT0_SCANOUT_COUNTER_WIDTH, line_cycles) |
		  dpu_field(DCREG_SH_OUTPUT0_SCANOUT_COUNTER_HEIGHT, mode->vdisplay));
	dpu_write(dpu, DCREG_OUTPUT0_SCANOUT_DELAY_COUNTER, 0);

	/* No urgent requests to the memory system */
	dpu_update(dpu, DCREG_SH_OUTPUT0_URGENT_VALUE, DCREG_SH_OUTPUT0_URGENT_VALUE_VALUE, 0);

	dpu_write(dpu, DCREG_SH_PANEL0_WIDTH, mode->hdisplay);
	dpu_write(dpu, DCREG_SH_PANEL0_HEIGHT, mode->vdisplay);
	dpu_write(dpu, DCREG_SH_PANEL0_IMAGE_WIDTH, mode->hdisplay);
	dpu_write(dpu, DCREG_SH_PANEL0_IMAGE_HEIGHT, mode->vdisplay);
	dpu_write(dpu, DCREG_SH_PANEL0_DITHER_WIDTH,
		  dpu_field(DCREG_SH_PANEL0_DITHER_WIDTH_VALUE, mode->hdisplay));
	dpu_write(dpu, DCREG_SH_PANEL0_DITHER_HEIGHT,
		  dpu_field(DCREG_SH_PANEL0_DITHER_HEIGHT_VALUE, mode->vdisplay));
}

/* Layer 0 */

static void dpu_layer_setup(struct mbu_dpu *dpu, struct drm_plane_state *state)
{
	struct drm_framebuffer *fb = state->fb;
	u32 format, config;
	dma_addr_t addr;

	config = dpu_read(dpu, DCREG_SH_LAYER0_CONFIG);

	if (!fb || !state->visible) {
		dpu_write(dpu, DCREG_SH_LAYER0_CONFIG,
			  config & ~DCREG_SH_LAYER0_CONFIG_ENABLE);
		return;
	}

	format = fb->format->format == DRM_FORMAT_ARGB8888 ?
		 DCREG_SH_LAYER0_CONFIG_FORMAT_A8R8G8B8 :
		 DCREG_SH_LAYER0_CONFIG_FORMAT_X8R8G8B8;
	addr = drm_fb_dma_get_gem_addr(fb, state, 0);

	dpu_write(dpu, DCREG_SH_LAYER0_ADDRESS, lower_32_bits(addr));
	dpu_write(dpu, DCREG_SH_LAYER0_HIGH_ADDRESS, upper_32_bits(addr));
	dpu_write(dpu, DCREG_SH_LAYER0_STRIDE, fb->pitches[0]);
	dpu_write(dpu, DCREG_SH_LAYER0_SIZE,
		  dpu_field(DCREG_SH_LAYER0_SIZE_WIDTH, drm_rect_width(&state->src) >> 16) |
		  dpu_field(DCREG_SH_LAYER0_SIZE_HEIGHT, drm_rect_height(&state->src) >> 16));
	dpu_write(dpu, DCREG_SH_LAYER0_OT_NUMBER, MBU_DPU_LAYER_OT_NUMBER);

	dpu_write(dpu, DCREG_SH_LAYER0_OUT_ROI_ORIGIN,
		  dpu_field(DCREG_SH_LAYER0_OUT_ROI_ORIGIN_X, state->dst.x1) |
		  dpu_field(DCREG_SH_LAYER0_OUT_ROI_ORIGIN_Y, state->dst.y1));
	dpu_write(dpu, DCREG_SH_LAYER0_OUT_ROI_SIZE,
		  dpu_field(DCREG_SH_LAYER0_OUT_ROI_SIZE_WIDTH, drm_rect_width(&state->dst)) |
		  dpu_field(DCREG_SH_LAYER0_OUT_ROI_SIZE_HEIGHT, drm_rect_height(&state->dst)));

	/* Bottom of the stack, nothing below to blend with */
	dpu_write(dpu, DCREG_SH_LAYER0_BLEND_STACK_ID, 0);
	dpu_update(dpu, DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG,
		   DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND,
		   DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND_DISABLED);

	/* A linear 32-bit line of up to 4096 pixels fits in 64 KB of SRAM. */
	dpu_write(dpu, DCREG_SH_LAYER0_DMA_SRAM_SIZE, DCREG_SH_LAYER0_DMA_SRAM_SIZE_VALUE_KBYTE64);
	dpu_write(dpu, DCREG_SH_LAYER0_SCALER_SRAM_SIZE,
		  DCREG_SH_LAYER0_SCALER_SRAM_SIZE_VALUE_KBYTE36);

	config &= ~(DCREG_SH_LAYER0_CONFIG_FORMAT | DCREG_SH_LAYER0_CONFIG_TILE_MODE |
		    DCREG_SH_LAYER0_CONFIG_ROT_ANGLE | DCREG_SH_LAYER0_CONFIG_SWIZZLE);
	config |= DCREG_SH_LAYER0_CONFIG_ENABLE |
		  dpu_field(DCREG_SH_LAYER0_CONFIG_FORMAT, format) |
		  dpu_field(DCREG_SH_LAYER0_CONFIG_TILE_MODE,
			    DCREG_SH_LAYER0_CONFIG_TILE_MODE_LINEAR) |
		  dpu_field(DCREG_SH_LAYER0_CONFIG_SWIZZLE, DCREG_SH_LAYER0_CONFIG_SWIZZLE_ARGB);
	dpu_write(dpu, DCREG_SH_LAYER0_CONFIG, config);
}

static void dpu_start_frame(struct mbu_dpu *dpu)
{
	u32 mux, sel;
	int i;

	/* Drop a stale frame start so that the next one belongs to this frame. */
	dpu_write(dpu, DCREG_BE_INTR_STATUS, DCREG_BE_INTR_STATUS_OUTPATH0_FRM_START);

	/*
	 * Route display 0 to output 0. Outputs must not share a source, so an
	 * output that used display 0 takes over output 0's old source.
	 */
	mux = dpu_read(dpu, DCREG_POST_PROCESS_OUT);
	sel = mux & DCREG_POST_PROCESS_OUT_MUX_OUT0_SEL;
	if (sel) {
		u32 width = hweight32(DCREG_POST_PROCESS_OUT_MUX_OUT0_SEL);

		for (i = 1; i < 4; i++) {
			u32 mask = DCREG_POST_PROCESS_OUT_MUX_OUT0_SEL << (i * width);

			if (!(mux & mask))
				mux = (mux & ~mask) | (sel << (i * width));
		}
		mux &= ~DCREG_POST_PROCESS_OUT_MUX_OUT0_SEL;
		dpu_write(dpu, DCREG_POST_PROCESS_OUT, mux);
	}

	dpu_write(dpu, DCREG_OUTPUT0_START, 1);
	/* Command mode: start one frame. */
	dpu_write(dpu, DCREG_OUTPUT0_SW_CONFIG, 1);
}

/* CRTC */

static void mbu_dpu_crtc_atomic_enable(struct drm_crtc *crtc, struct drm_atomic_commit *state)
{
	struct mbu_dpu *dpu = to_mbu_dpu(crtc->dev);
	const struct drm_dsc_config *dsc = mbu_dsi_get_dsc(dpu->dsi_np);

	dpu_shadow_enable(dpu, false);
	dpu_display_setup(dpu, &crtc->state->adjusted_mode, dsc);
	dpu_write(dpu, DCREG_LAYER0_OUTPUT_PATH_ID, 0);
	dpu_shadow_enable(dpu, true);

	drm_crtc_vblank_on(crtc);
}

static void mbu_dpu_crtc_atomic_disable(struct drm_crtc *crtc, struct drm_atomic_commit *state)
{
	struct mbu_dpu *dpu = to_mbu_dpu(crtc->dev);
	unsigned long flags;

	dpu_write(dpu, DCREG_OUTPUT0_START, 0);
	dpu_update(dpu, DCREG_SH_LAYER0_CONFIG, DCREG_SH_LAYER0_CONFIG_ENABLE, 0);
	dpu_write(dpu, DCREG_SH_OUTPUT0_CLK_EN, 0);

	drm_crtc_vblank_off(crtc);

	spin_lock_irqsave(&dpu->lock, flags);
	if (crtc->state->event && !crtc->state->active) {
		drm_crtc_send_vblank_event(crtc, crtc->state->event);
		crtc->state->event = NULL;
	}
	spin_unlock_irqrestore(&dpu->lock, flags);
}

static void mbu_dpu_crtc_atomic_flush(struct drm_crtc *crtc, struct drm_atomic_commit *state)
{
	struct mbu_dpu *dpu = to_mbu_dpu(crtc->dev);
	struct drm_pending_vblank_event *event = crtc->state->event;
	unsigned long flags;

	if (!crtc->state->active)
		return;

	dpu_shadow_enable(dpu, false);
	dpu_layer_setup(dpu, dpu->plane.state);
	dpu_shadow_enable(dpu, true);

	if (event) {
		crtc->state->event = NULL;
		WARN_ON(drm_crtc_vblank_get(crtc));

		spin_lock_irqsave(&dpu->lock, flags);
		dpu->event = event;
		spin_unlock_irqrestore(&dpu->lock, flags);
	}

	dpu_start_frame(dpu);
}

static int mbu_dpu_crtc_enable_vblank(struct drm_crtc *crtc)
{
	struct mbu_dpu *dpu = to_mbu_dpu(crtc->dev);
	unsigned long flags;

	spin_lock_irqsave(&dpu->lock, flags);
	dpu_update(dpu, DCREG_BE_INTR_ENABLE, DCREG_BE_INTR_ENABLE_OUTPATH0_FRM_DONE, 1);
	spin_unlock_irqrestore(&dpu->lock, flags);

	return 0;
}

static void mbu_dpu_crtc_disable_vblank(struct drm_crtc *crtc)
{
	struct mbu_dpu *dpu = to_mbu_dpu(crtc->dev);
	unsigned long flags;

	spin_lock_irqsave(&dpu->lock, flags);
	dpu_update(dpu, DCREG_BE_INTR_ENABLE, DCREG_BE_INTR_ENABLE_OUTPATH0_FRM_DONE, 0);
	spin_unlock_irqrestore(&dpu->lock, flags);
}

static const struct drm_crtc_helper_funcs mbu_dpu_crtc_helper_funcs = {
	.atomic_enable	= mbu_dpu_crtc_atomic_enable,
	.atomic_disable	= mbu_dpu_crtc_atomic_disable,
	.atomic_flush	= mbu_dpu_crtc_atomic_flush,
};

static const struct drm_crtc_funcs mbu_dpu_crtc_funcs = {
	.reset			= drm_atomic_helper_crtc_reset,
	.destroy		= drm_crtc_cleanup,
	.set_config		= drm_atomic_helper_set_config,
	.page_flip		= drm_atomic_helper_page_flip,
	.atomic_duplicate_state	= drm_atomic_helper_crtc_duplicate_state,
	.atomic_destroy_state	= drm_atomic_helper_crtc_destroy_state,
	.enable_vblank		= mbu_dpu_crtc_enable_vblank,
	.disable_vblank		= mbu_dpu_crtc_disable_vblank,
};

/* Plane */

static int mbu_dpu_plane_atomic_check(struct drm_plane *plane, struct drm_atomic_commit *state)
{
	struct drm_plane_state *new = drm_atomic_get_new_plane_state(state, plane);
	struct drm_crtc_state *crtc_state;

	if (!new->crtc)
		return 0;

	crtc_state = drm_atomic_get_new_crtc_state(state, new->crtc);

	/* No scaling and no rotation. */
	return drm_atomic_helper_check_plane_state(new, crtc_state, DRM_PLANE_NO_SCALING,
						   DRM_PLANE_NO_SCALING, false, true);
}

static const struct drm_plane_helper_funcs mbu_dpu_plane_helper_funcs = {
	.atomic_check = mbu_dpu_plane_atomic_check,
};

static const struct drm_plane_funcs mbu_dpu_plane_funcs = {
	.update_plane		= drm_atomic_helper_update_plane,
	.disable_plane		= drm_atomic_helper_disable_plane,
	.destroy		= drm_plane_cleanup,
	.reset			= drm_atomic_helper_plane_reset,
	.atomic_duplicate_state	= drm_atomic_helper_plane_duplicate_state,
	.atomic_destroy_state	= drm_atomic_helper_plane_destroy_state,
};

static const u32 mbu_dpu_formats[] = {
	DRM_FORMAT_XRGB8888,
	DRM_FORMAT_ARGB8888,
};

/* Interrupts */

static irqreturn_t mbu_dpu_irq(int irq, void *data)
{
	struct mbu_dpu *dpu = data;
	struct drm_crtc *crtc = &dpu->crtc;
	unsigned long flags;
	u32 status;

	status = dpu_read(dpu, DCREG_BE_INTR_STATUS);
	if (!status)
		return IRQ_NONE;

	dpu_write(dpu, DCREG_BE_INTR_STATUS, status);

	if (status & DCREG_BE_INTR_STATUS_OUTPATH0_UNDERRUN)
		dev_err_ratelimited(dpu->dev, "output 0 underrun\n");

	if (status & DCREG_BE_INTR_STATUS_OUTPATH0_FRM_DONE) {
		drm_crtc_handle_vblank(crtc);

		spin_lock_irqsave(&dpu->lock, flags);
		if (dpu->event) {
			drm_crtc_send_vblank_event(crtc, dpu->event);
			drm_crtc_vblank_put(crtc);
			dpu->event = NULL;
		}
		spin_unlock_irqrestore(&dpu->lock, flags);
	}

	return IRQ_HANDLED;
}

/* Device */

static const struct drm_mode_config_funcs mbu_dpu_mode_config_funcs = {
	.fb_create	= drm_gem_fb_create_with_dirty,
	.atomic_check	= drm_atomic_helper_check,
	.atomic_commit	= drm_atomic_helper_commit,
};

/* Program the whole pipeline before the first frame is started. */
static const struct drm_mode_config_helper_funcs mbu_dpu_mode_config_helpers = {
	.atomic_commit_tail = drm_atomic_helper_commit_tail_rpm,
};

DEFINE_DRM_GEM_DMA_FOPS(mbu_dpu_fops);

static const struct drm_driver mbu_dpu_drm_driver = {
	.driver_features	= DRIVER_GEM | DRIVER_MODESET | DRIVER_ATOMIC,
	.fops			= &mbu_dpu_fops,
	.name			= "mbu-dpu",
	.desc			= "Google Tensor G6 display controller",
	.major			= 1,
	.minor			= 0,
	DRM_GEM_DMA_DRIVER_OPS,
	DRM_FBDEV_DMA_DRIVER_OPS,
};

static int mbu_dpu_kms_init(struct mbu_dpu *dpu)
{
	struct drm_device *drm = &dpu->drm;
	struct drm_bridge *bridge;
	int ret;

	ret = drmm_mode_config_init(drm);
	if (ret)
		return ret;

	drm->mode_config.min_width = 16;
	drm->mode_config.min_height = 16;
	drm->mode_config.max_width = MBU_DPU_MAX_WIDTH;
	drm->mode_config.max_height = MBU_DPU_MAX_HEIGHT;
	drm->mode_config.funcs = &mbu_dpu_mode_config_funcs;
	drm->mode_config.helper_private = &mbu_dpu_mode_config_helpers;

	ret = drm_universal_plane_init(drm, &dpu->plane, 0, &mbu_dpu_plane_funcs,
				       mbu_dpu_formats, ARRAY_SIZE(mbu_dpu_formats), NULL,
				       DRM_PLANE_TYPE_PRIMARY, NULL);
	if (ret)
		return ret;
	drm_plane_helper_add(&dpu->plane, &mbu_dpu_plane_helper_funcs);
	drm_plane_enable_fb_damage_clips(&dpu->plane);

	ret = drmm_crtc_init_with_planes(drm, &dpu->crtc, &dpu->plane, NULL,
					 &mbu_dpu_crtc_funcs, NULL);
	if (ret)
		return ret;
	drm_crtc_helper_add(&dpu->crtc, &mbu_dpu_crtc_helper_funcs);

	ret = drmm_encoder_init(drm, &dpu->encoder, NULL, DRM_MODE_ENCODER_DSI, NULL);
	if (ret)
		return ret;
	dpu->encoder.possible_crtcs = drm_crtc_mask(&dpu->crtc);

	/* Port 0 leads to the DSI0 host. */
	bridge = devm_drm_of_get_bridge(dpu->dev, dpu->dev->of_node, 0, 0);
	if (IS_ERR(bridge))
		return dev_err_probe(dpu->dev, PTR_ERR(bridge), "no DSI bridge\n");

	ret = drm_bridge_attach(&dpu->encoder, bridge, NULL, 0);
	if (ret)
		return ret;

	ret = drm_vblank_init(drm, 1);
	if (ret)
		return ret;

	drm_mode_config_reset(drm);

	return 0;
}

static void mbu_dpu_put_node(void *np)
{
	of_node_put(np);
}

static int mbu_dpu_probe(struct platform_device *pdev)
{
	static const char * const irq_names[] = { "frame-done", "underrun" };
	struct dev_pm_domain_attach_data pd_data = {
		.pd_flags = PD_FLAG_DEV_LINK_ON,
	};
	struct device *dev = &pdev->dev;
	struct mbu_dpu *dpu;
	unsigned int i;
	int irq, ret;

	dpu = devm_drm_dev_alloc(dev, &mbu_dpu_drm_driver, struct mbu_dpu, drm);
	if (IS_ERR(dpu))
		return PTR_ERR(dpu);

	dpu->dev = dev;
	spin_lock_init(&dpu->lock);
	platform_set_drvdata(pdev, dpu);

	dpu->regs = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(dpu->regs))
		return PTR_ERR(dpu->regs);

	/* The layers can fetch from a 40-bit address space. */
	ret = dma_set_mask_and_coherent(dev, DMA_BIT_MASK(40));
	if (ret)
		return ret;

	/* The controller and its DSI output sit in several power domains. */
	ret = devm_pm_domain_attach_list(dev, &pd_data, NULL);
	if (ret < 0)
		return dev_err_probe(dev, ret, "failed to attach power domains\n");

	dpu->dsi_np = of_graph_get_remote_node(dev->of_node, 0, 0);
	if (!dpu->dsi_np)
		return dev_err_probe(dev, -ENODEV, "no DSI host in the graph\n");
	ret = devm_add_action_or_reset(dev, mbu_dpu_put_node, dpu->dsi_np);
	if (ret)
		return ret;

	for (i = 0; i < ARRAY_SIZE(irq_names); i++) {
		irq = platform_get_irq_byname(pdev, irq_names[i]);
		if (irq < 0)
			return irq;

		ret = devm_request_irq(dev, irq, mbu_dpu_irq, 0, dev_name(dev), dpu);
		if (ret)
			return ret;
	}

	/* Report underruns, frame done is enabled together with vblank. */
	dpu_update(dpu, DCREG_BE_INTR_ENABLE, DCREG_BE_INTR_ENABLE_OUTPATH0_UNDERRUN, 1);

	ret = mbu_dpu_kms_init(dpu);
	if (ret)
		return ret;

	ret = drm_dev_register(&dpu->drm, 0);
	if (ret)
		return ret;

	drm_client_setup(&dpu->drm, NULL);

	return 0;
}

static void mbu_dpu_remove(struct platform_device *pdev)
{
	struct mbu_dpu *dpu = platform_get_drvdata(pdev);

	drm_dev_unregister(&dpu->drm);
	drm_atomic_helper_shutdown(&dpu->drm);
}

static void mbu_dpu_shutdown(struct platform_device *pdev)
{
	struct mbu_dpu *dpu = platform_get_drvdata(pdev);

	drm_atomic_helper_shutdown(&dpu->drm);
}

static const struct of_device_id mbu_dpu_of_match[] = {
	{ .compatible = "google,mbu-dpu" },
	{ }
};
MODULE_DEVICE_TABLE(of, mbu_dpu_of_match);

static struct platform_driver mbu_dpu_driver = {
	.probe = mbu_dpu_probe,
	.remove = mbu_dpu_remove,
	.shutdown = mbu_dpu_shutdown,
	.driver = {
		.name = "google-mbu-dpu",
		.of_match_table = mbu_dpu_of_match,
	},
};
module_platform_driver(mbu_dpu_driver);

MODULE_DESCRIPTION("Google Tensor G6 display controller driver");
MODULE_LICENSE("GPL");
