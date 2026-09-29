// SPDX-License-Identifier: GPL-2.0-only OR MIT
/*
 * Google "dcsda" 1080x2424 command mode AMOLED panel, used in the Pixel 11
 * family (Cubs).
 *
 * The panel takes DSC 1.2a compressed pixels over four DSI lanes and
 * refreshes from its own frame memory, synchronised to the host through
 * the TE signal.
 *
 * Based on the downstream Pixel panel driver.
 *
 * Copyright 2025 Google LLC
 */

#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/regulator/consumer.h>
#include <linux/units.h>

#include <video/mipi_display.h>

#include <drm/display/drm_dsc.h>
#include <drm/display/drm_dsc_helper.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <drm/drm_probe_helper.h>

/* Lane rate the panel's frequency compensation is tuned for */
#define DCSDA_LANE_RATE_MBPS		756

#define DCSDA_MAX_BRIGHTNESS		4095
#define DCSDA_DEFAULT_BRIGHTNESS	1228

/* WRCTRLD: brightness control enabled */
#define DCSDA_WRCTRLD_BCTRL		0x20

static const u8 test_key_enable[] = { 0xf0, 0x5a, 0x5a };
static const u8 test_key_disable[] = { 0xf0, 0xa5, 0xa5 };
static const u8 test_key_fc_enable[] = { 0xfc, 0x5a, 0x5a };
static const u8 test_key_fc_disable[] = { 0xfc, 0xa5, 0xa5 };

struct dcsda {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct drm_dsc_config dsc;
	struct regulator *vddi;
	struct regulator *vci;
	struct regulator *vddd;
	struct gpio_desc *reset_gpio;
};

static inline struct dcsda *to_dcsda(struct drm_panel *panel)
{
	return container_of(panel, struct dcsda, panel);
}

/* 1080x2424, 12 pixel HSA, 2 line VSA, 60 Hz */
static const struct drm_display_mode dcsda_mode = {
	.clock = (1080 + 32 + 12 + 16) * (2424 + 8 + 2 + 16) * 60 / 1000,
	.hdisplay = 1080,
	.hsync_start = 1080 + 32,
	.hsync_end = 1080 + 32 + 12,
	.htotal = 1080 + 32 + 12 + 16,
	.vdisplay = 2424,
	.vsync_start = 2424 + 8,
	.vsync_end = 2424 + 8 + 2,
	.vtotal = 2424 + 8 + 2 + 16,
	.width_mm = 65,
	.height_mm = 146,
	.type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED,
};

static void dcsda_init_dsc(struct drm_dsc_config *dsc)
{
	static const u16 rc_buf_thresh[] = {
		14, 28, 42, 56, 70, 84, 98, 105, 112, 119, 121, 123, 125, 126,
	};
	static const struct drm_dsc_rc_range_parameters rc_range_params[] = {
		{ 0, 4, 2 }, { 0, 4, 0 }, { 1, 5, 0 }, { 1, 6, 62 },
		{ 3, 7, 60 }, { 3, 7, 58 }, { 3, 7, 56 }, { 3, 8, 56 },
		{ 3, 9, 56 }, { 3, 10, 54 }, { 5, 11, 54 }, { 5, 12, 52 },
		{ 5, 13, 52 }, { 7, 13, 52 }, { 13, 15, 52 },
	};

	dsc->dsc_version_major = 1;
	dsc->dsc_version_minor = 2;
	dsc->line_buf_depth = 9;
	dsc->bits_per_component = 8;
	dsc->convert_rgb = true;
	dsc->slice_width = 540;
	dsc->slice_height = 101;
	dsc->slice_count = 2;
	dsc->pic_width = 1080;
	dsc->pic_height = 2424;
	dsc->bits_per_pixel = 8 << 4;
	dsc->block_pred_enable = true;
	dsc->rc_tgt_offset_high = 3;
	dsc->rc_tgt_offset_low = 3;
	dsc->rc_edge_factor = 6;
	dsc->rc_quant_incr_limit0 = 11;
	dsc->rc_quant_incr_limit1 = 11;
	dsc->initial_xmit_delay = 512;
	dsc->initial_dec_delay = 526;
	dsc->first_line_bpg_offset = 12;
	dsc->initial_offset = 6144;
	dsc->rc_model_size = 8192;
	dsc->flatness_min_qp = 3;
	dsc->flatness_max_qp = 12;
	dsc->initial_scale_value = 32;
	dsc->scale_decrement_interval = 7;
	dsc->scale_increment_interval = 2517;
	dsc->nfl_bpg_offset = 246;
	dsc->slice_bpg_offset = 258;
	dsc->final_offset = 4336;
	dsc->slice_chunk_size = 540;

	memcpy(dsc->rc_buf_thresh, rc_buf_thresh, sizeof(rc_buf_thresh));
	memcpy(dsc->rc_range_params, rc_range_params, sizeof(rc_range_params));
}

static int dcsda_power_on(struct dcsda *ctx)
{
	int ret;

	ret = regulator_enable(ctx->vddi);
	if (ret)
		return ret;

	usleep_range(1000, 1100);

	ret = regulator_enable(ctx->vci);
	if (ret)
		goto err_vddi;

	usleep_range(10000, 11000);

	ret = regulator_enable(ctx->vddd);
	if (ret)
		goto err_vci;

	/* Release reset and let the panel boot. */
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(10000, 11000);

	return 0;

err_vci:
	regulator_disable(ctx->vci);
err_vddi:
	regulator_disable(ctx->vddi);
	return ret;
}

static void dcsda_power_off(struct dcsda *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_disable(ctx->vddd);
	regulator_disable(ctx->vci);
	regulator_disable(ctx->vddi);
}

static int dcsda_prepare(struct drm_panel *panel)
{
	return dcsda_power_on(to_dcsda(panel));
}

static int dcsda_unprepare(struct drm_panel *panel)
{
	dcsda_power_off(to_dcsda(panel));

	return 0;
}

static int dcsda_enable(struct drm_panel *panel)
{
	struct dcsda *ctx = to_dcsda(panel);
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };
	struct drm_dsc_picture_parameter_set pps;

	ctx->dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_set_tear_on_multi(&dsi_ctx, MIPI_DSI_DCS_TEAR_MODE_VBLANK);
	mipi_dsi_dcs_exit_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);

	mipi_dsi_dcs_set_column_address_multi(&dsi_ctx, 0, 1080 - 1);
	mipi_dsi_dcs_set_page_address_multi(&dsi_ctx, 0, 2424 - 1);

	/* Frequency compensation for the 756 Mbps lane rate */
	mipi_dsi_dcs_write_buffer_multi(&dsi_ctx, test_key_enable, sizeof(test_key_enable));
	mipi_dsi_dcs_write_buffer_multi(&dsi_ctx, test_key_fc_enable,
					sizeof(test_key_fc_enable));
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb0, 0x00, 0x3e, 0xc5);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc5, 0x56, 0x59);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb0, 0x00, 0x36, 0xc5);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc5, 0x11, 0x10, 0x50, 0x05);
	mipi_dsi_dcs_write_buffer_multi(&dsi_ctx, test_key_fc_disable,
					sizeof(test_key_fc_disable));
	mipi_dsi_dcs_write_buffer_multi(&dsi_ctx, test_key_disable, sizeof(test_key_disable));

	/* 60 Hz refresh */
	mipi_dsi_dcs_write_buffer_multi(&dsi_ctx, test_key_enable, sizeof(test_key_enable));
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x83, 0x08);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xf7, 0x2f);
	mipi_dsi_dcs_write_buffer_multi(&dsi_ctx, test_key_disable, sizeof(test_key_disable));

	/* Compression */
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DSI_COMPRESSION_MODE, 0x01);
	drm_dsc_pps_payload_pack(&pps, &ctx->dsc);
	mipi_dsi_picture_parameter_set_multi(&dsi_ctx, &pps);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x9d, 0x01);

	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_WRITE_CONTROL_DISPLAY,
				     DCSDA_WRCTRLD_BCTRL);
	mipi_dsi_dcs_set_display_on_multi(&dsi_ctx);

	ctx->dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	return dsi_ctx.accum_err;
}

static int dcsda_disable(struct drm_panel *panel)
{
	struct dcsda *ctx = to_dcsda(panel);
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	ctx->dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_set_display_off_multi(&dsi_ctx);
	mipi_dsi_dcs_enter_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);

	return dsi_ctx.accum_err;
}

static int dcsda_get_modes(struct drm_panel *panel, struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &dcsda_mode);
}

static const struct drm_panel_funcs dcsda_panel_funcs = {
	.prepare = dcsda_prepare,
	.unprepare = dcsda_unprepare,
	.enable = dcsda_enable,
	.disable = dcsda_disable,
	.get_modes = dcsda_get_modes,
};

static int dcsda_bl_update_status(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);

	return mipi_dsi_dcs_set_display_brightness_large(dsi, backlight_get_brightness(bl));
}

static const struct backlight_ops dcsda_bl_ops = {
	.update_status = dcsda_bl_update_status,
};

static int dcsda_probe(struct mipi_dsi_device *dsi)
{
	const struct backlight_properties props = {
		.type = BACKLIGHT_RAW,
		.brightness = DCSDA_DEFAULT_BRIGHTNESS,
		.max_brightness = DCSDA_MAX_BRIGHTNESS,
	};
	struct device *dev = &dsi->dev;
	struct dcsda *ctx;
	int ret;

	ctx = devm_drm_panel_alloc(dev, struct dcsda, panel, &dcsda_panel_funcs,
				   DRM_MODE_CONNECTOR_DSI);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	ctx->vddi = devm_regulator_get(dev, "vddi");
	if (IS_ERR(ctx->vddi))
		return dev_err_probe(dev, PTR_ERR(ctx->vddi), "failed to get vddi\n");

	ctx->vci = devm_regulator_get(dev, "vci");
	if (IS_ERR(ctx->vci))
		return dev_err_probe(dev, PTR_ERR(ctx->vci), "failed to get vci\n");

	ctx->vddd = devm_regulator_get(dev, "vddd");
	if (IS_ERR(ctx->vddd))
		return dev_err_probe(dev, PTR_ERR(ctx->vddd), "failed to get vddd\n");

	/*
	 * Keep the panel as the bootloader left it, it is powered and
	 * showing the splash screen.
	 */
	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_ASIS);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio), "failed to get reset\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_CLOCK_NON_CONTINUOUS;
	dsi->hs_rate = DCSDA_LANE_RATE_MBPS * HZ_PER_MHZ;

	dcsda_init_dsc(&ctx->dsc);
	dsi->dsc = &ctx->dsc;

	ctx->panel.prepare_prev_first = true;
	ctx->panel.backlight = devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
							      &dcsda_bl_ops, &props);
	if (IS_ERR(ctx->panel.backlight))
		return dev_err_probe(dev, PTR_ERR(ctx->panel.backlight),
				     "failed to register backlight\n");

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret) {
		drm_panel_remove(&ctx->panel);
		return dev_err_probe(dev, ret, "failed to attach to DSI host\n");
	}

	return 0;
}

static void dcsda_remove(struct mipi_dsi_device *dsi)
{
	struct dcsda *ctx = mipi_dsi_get_drvdata(dsi);

	mipi_dsi_detach(dsi);
	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id dcsda_of_match[] = {
	{ .compatible = "google,dcsda" },
	{ }
};
MODULE_DEVICE_TABLE(of, dcsda_of_match);

static struct mipi_dsi_driver dcsda_driver = {
	.probe = dcsda_probe,
	.remove = dcsda_remove,
	.driver = {
		.name = "panel-google-dcsda",
		.of_match_table = dcsda_of_match,
	},
};
module_mipi_dsi_driver(dcsda_driver);

MODULE_DESCRIPTION("Google dcsda DSI panel driver");
MODULE_LICENSE("Dual MIT/GPL");
