// SPDX-License-Identifier: GPL-2.0-only
/*
 * Google Tensor G6 (malibu) MIPI DSI glue
 *
 * The DSI outputs are Synopsys DesignWare MIPI DSI-2 host controllers,
 * driven by the common dw-mipi-dsi2 bridge, each with its own Synopsys
 * C/D-PHY in D-PHY mode.
 *
 * Copyright 2025 Google LLC
 */

#include <linux/math64.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/phy/phy.h>
#include <linux/phy/phy-mipi-dphy.h>
#include <linux/platform_device.h>
#include <linux/units.h>

#include <drm/bridge/dw_mipi_dsi2.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>

#include "mbu_dsi.h"

/* Limit of the C/D-PHY in D-PHY mode */
#define MBU_DSI_MAX_LANE_KBPS	4500000

struct mbu_dsi {
	struct device *dev;
	struct dw_mipi_dsi2 *dmd;
	struct dw_mipi_dsi2_plat_data pdata;
	struct phy *phy;
	union phy_configure_opts phy_opts;
	unsigned int lane_mbps;
	unsigned long hs_rate;
	const struct drm_dsc_config *dsc;
};

static int mbu_dsi_phy_init(void *priv_data)
{
	/* The PHY is initialised together with power on, to keep it balanced. */
	return 0;
}

static void mbu_dsi_phy_power_on(void *priv_data)
{
	struct mbu_dsi *dsi = priv_data;
	int ret;

	ret = phy_init(dsi->phy);
	if (ret)
		goto err;

	ret = phy_set_mode(dsi->phy, PHY_MODE_MIPI_DPHY);
	if (!ret)
		ret = phy_configure(dsi->phy, &dsi->phy_opts);
	if (!ret)
		ret = phy_power_on(dsi->phy);
	if (!ret)
		return;

	phy_exit(dsi->phy);
err:
	dev_err(dsi->dev, "failed to start D-PHY: %d\n", ret);
}

static void mbu_dsi_phy_power_off(void *priv_data)
{
	struct mbu_dsi *dsi = priv_data;

	phy_power_off(dsi->phy);
	phy_exit(dsi->phy);
}

static void mbu_dsi_phy_get_interface(void *priv_data,
				      struct dw_mipi_dsi2_phy_iface *iface)
{
	/* The PHY is programmed for a 16-bit PPI. */
	iface->ppi_width = 16;
	iface->phy_type = DW_MIPI_DSI2_DPHY;
}

static int mbu_dsi_get_lane_mbps(void *priv_data,
				 const struct drm_display_mode *mode,
				 unsigned long mode_flags, u32 lanes, u32 format,
				 unsigned int *lane_mbps)
{
	struct mbu_dsi *dsi = priv_data;
	unsigned long lane_kbps;
	int bpp;

	bpp = mipi_dsi_pixel_format_to_bpp(format);
	if (bpp < 0)
		return bpp;

	if (dsi->hs_rate) {
		/* Command mode panels usually need a specific lane rate. */
		lane_kbps = dsi->hs_rate / HZ_PER_KHZ;
	} else {
		lane_kbps = mode->clock * bpp / lanes;
		/* Leave room for protocol overhead and LP transitions. */
		lane_kbps = lane_kbps * 10 / 9;
	}

	if (lane_kbps > MBU_DSI_MAX_LANE_KBPS)
		return -ERANGE;

	dsi->lane_mbps = DIV_ROUND_UP(lane_kbps, 1000);
	*lane_mbps = dsi->lane_mbps;

	phy_mipi_dphy_get_default_config_for_hsclk((u64)dsi->lane_mbps * HZ_PER_MHZ,
						   lanes, &dsi->phy_opts.mipi_dphy);

	return 0;
}

static int mbu_dsi_get_timing(void *priv_data, unsigned int lane_mbps,
			      struct dw_mipi_dsi2_phy_timing *timing)
{
	struct mbu_dsi *dsi = priv_data;
	struct phy_configure_opts_mipi_dphy *cfg = &dsi->phy_opts.mipi_dphy;
	u64 hstx_clk, period_ps, tmp;

	/* The PPI clock runs at lane rate / PPI width. */
	hstx_clk = DIV_ROUND_CLOSEST_ULL((u64)lane_mbps * HZ_PER_MHZ, 16);
	period_ps = DIV_ROUND_UP_ULL(PSEC_PER_SEC, hstx_clk);

	/* LP to HS: TLPX + THS-PREPARE + THS-ZERO, in 16.16 PPI clock cycles */
	tmp = (u64)cfg->lpx + cfg->hs_prepare + cfg->hs_zero;
	timing->data_lp2hs = DIV_ROUND_CLOSEST_ULL(tmp << 16, period_ps);

	/* HS to LP: THS-TRAIL + THS-EXIT */
	tmp = (u64)cfg->hs_trail + cfg->hs_exit;
	timing->data_hs2lp = DIV_ROUND_CLOSEST_ULL(tmp << 16, period_ps);

	return 0;
}

static const struct dw_mipi_dsi2_phy_ops mbu_dsi_phy_ops = {
	.init		= mbu_dsi_phy_init,
	.power_on	= mbu_dsi_phy_power_on,
	.power_off	= mbu_dsi_phy_power_off,
	.get_interface	= mbu_dsi_phy_get_interface,
	.get_lane_mbps	= mbu_dsi_get_lane_mbps,
	.get_timing	= mbu_dsi_get_timing,
};

static int mbu_dsi_host_attach(void *priv_data, struct mipi_dsi_device *device)
{
	struct mbu_dsi *dsi = priv_data;

	dsi->hs_rate = device->hs_rate;
	dsi->dsc = device->dsc;

	return 0;
}

static int mbu_dsi_host_detach(void *priv_data, struct mipi_dsi_device *device)
{
	struct mbu_dsi *dsi = priv_data;

	dsi->hs_rate = 0;
	dsi->dsc = NULL;

	return 0;
}

static const struct dw_mipi_dsi2_host_ops mbu_dsi_host_ops = {
	.attach	= mbu_dsi_host_attach,
	.detach	= mbu_dsi_host_detach,
};

/**
 * mbu_dsi_get_dsc() - Get the DSC configuration of the panel on a DSI host
 * @np: device tree node of the DSI host
 *
 * The display controller has to compress the stream itself, with the
 * parameters the panel expects.
 *
 * Return: the DSC configuration, or NULL if the panel uses no compression
 * or has not attached yet.
 */
const struct drm_dsc_config *mbu_dsi_get_dsc(struct device_node *np)
{
	struct platform_device *pdev = of_find_device_by_node(np);
	const struct drm_dsc_config *dsc = NULL;
	struct mbu_dsi *dsi;

	if (!pdev)
		return NULL;

	dsi = platform_get_drvdata(pdev);
	if (dsi)
		dsc = dsi->dsc;

	put_device(&pdev->dev);

	return dsc;
}
EXPORT_SYMBOL_GPL(mbu_dsi_get_dsc);

static int mbu_dsi_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct mbu_dsi *dsi;

	dsi = devm_kzalloc(dev, sizeof(*dsi), GFP_KERNEL);
	if (!dsi)
		return -ENOMEM;

	dsi->dev = dev;

	dsi->phy = devm_phy_get(dev, "dphy");
	if (IS_ERR(dsi->phy))
		return dev_err_probe(dev, PTR_ERR(dsi->phy), "no D-PHY\n");

	dsi->pdata.max_data_lanes = 4;
	dsi->pdata.phy_ops = &mbu_dsi_phy_ops;
	dsi->pdata.host_ops = &mbu_dsi_host_ops;
	dsi->pdata.priv_data = dsi;
	platform_set_drvdata(pdev, dsi);

	dsi->dmd = dw_mipi_dsi2_probe(pdev, &dsi->pdata);
	if (IS_ERR(dsi->dmd))
		return dev_err_probe(dev, PTR_ERR(dsi->dmd), "failed to probe DSI-2 host\n");

	return 0;
}

static void mbu_dsi_remove(struct platform_device *pdev)
{
	struct mbu_dsi *dsi = platform_get_drvdata(pdev);

	dw_mipi_dsi2_remove(dsi->dmd);
}

static const struct of_device_id mbu_dsi_of_match[] = {
	{ .compatible = "google,mbu-mipi-dsi" },
	{ }
};
MODULE_DEVICE_TABLE(of, mbu_dsi_of_match);

static struct platform_driver mbu_dsi_driver = {
	.probe = mbu_dsi_probe,
	.remove = mbu_dsi_remove,
	.driver = {
		.name = "google-mbu-dsi",
		.of_match_table = mbu_dsi_of_match,
	},
};
module_platform_driver(mbu_dsi_driver);

MODULE_DESCRIPTION("Google Tensor G6 MIPI DSI glue");
MODULE_LICENSE("GPL");
