// SPDX-License-Identifier: GPL-2.0-only
/*
 * Google Tensor G6 (malibu) power domains
 *
 * Power domains are owned by the CPM's system power manager (SysPM). The AP
 * votes for a domain by writing its vote file in the CPM's MBFS. If the CPM
 * cannot complete the transition immediately it answers "try again" and
 * later sends a notification carrying the domain's resource id.
 *
 * Based on the downstream Pixel power controller.
 *
 * Copyright 2023-2025 Google LLC
 */

#include <linux/auxiliary_bus.h>
#include <linux/completion.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/pm_domain.h>
#include <linux/soc/google/google-cpm.h>

#include <dt-bindings/power/google,mbu-power.h>

/* MBFS folder holding this VM's votes. */
#define MBU_PD_VOTE_ROOT	"syspm/clients/vm1"

#define MBU_PD_VOTE_OFF		0
#define MBU_PD_VOTE_ON		1
#define MBU_PD_VOTE_PENDING	2

#define MBU_PD_TIMEOUT_MS	3000

/**
 * struct mbu_pd_desc - static description of a power domain
 * @name: domain name
 * @vote: name of the vote file in the MBFS vote folder
 * @sid: SysPM resource id, used in completion notifications
 * @parent: index of the parent domain, or -1
 * @always_on: the domain must never be turned off
 */
struct mbu_pd_desc {
	const char *name;
	const char *vote;
	u16 sid;
	s8 parent;
	bool always_on;
};

static const struct mbu_pd_desc mbu_pd_descs[] = {
	[MBU_PD_AURDSP] = { "sswrp_aurdsp", "aurdsp.v", 0x3, -1 },
	[MBU_PD_AOSS_PG] = { "sswrp_aoss_pg", "aoss_pg.v", 0x2, -1, .always_on = true },
	[MBU_PD_CODEC_3P] = { "sswrp_codec_3p", "codec_3p.v", 0x5, -1 },
	[MBU_PD_CPUACC_GPDMA] = { "cpuacc_gpdma", "CpuaccGpdma.v", 0x290, -1 },
	[MBU_PD_DPU] = { "sswrp_dpu", "dpu.v", 0x8, -1 },
	[MBU_PD_DPU_BE] = { "dpu_be", "DpuBe.v", 0x200, MBU_PD_DPU },
	[MBU_PD_DPU_FE0] = { "dpu_fe0", "DpuFe0.v", 0x201, MBU_PD_DPU_BE },
	[MBU_PD_DPU_FE1] = { "dpu_fe1", "DpuFe1.v", 0x202, MBU_PD_DPU_FE0 },
	[MBU_PD_DPU_DSI0] = { "dpu_dsi0", "DpuDsi0.v", 0x203, MBU_PD_DPU_BE },
	[MBU_PD_DPU_DSI1] = { "dpu_dsi1", "DpuDsi1.v", 0x204, MBU_PD_DPU_BE },
	[MBU_PD_DPU_DP0] = { "dpu_dp0", "DpuDp0.v", 0x205, MBU_PD_DPU_BE },
	[MBU_PD_G2D] = { "sswrp_g2d", "g2d.v", 0xe, -1 },
	[MBU_PD_G2D_CORE] = { "g2d_core", "G2dCore.v", 0x260, MBU_PD_G2D },
	[MBU_PD_GCV] = { "sswrp_gcv", "gcv.v", 0xf, -1 },
	[MBU_PD_GPU] = { "sswrp_gpu", "gpu.v", 0x17, -1 },
	[MBU_PD_HSIO_N] = { "sswrp_hsio_n", "hsio_n.v", 0x19, -1 },
	[MBU_PD_HSIO_N_USB] = { "hsio_n_usb", "HsioNUsb.v", 0x210, MBU_PD_HSIO_N },
	[MBU_PD_HSIO_N_EBU] = { "hsio_n_ebu", "HsioNEbu.v", 0x212, MBU_PD_HSIO_N_USB },
	[MBU_PD_HSIO_N_DP] = { "hsio_n_dp", "HsioNDp.v", 0x213, MBU_PD_HSIO_N_USB },
	[MBU_PD_HSIO_N_USB2AUX] = { "hsio_n_usb2aux", "HsioNU2Aux.v", 0x214, MBU_PD_HSIO_N_USB },
	[MBU_PD_HSIO_N_USB2AUX_PSW] = { "hsio_n_usb2aux_psw", "HsioNU2AuxPsw.v", 0x215,
					MBU_PD_HSIO_N_USB2AUX },
	[MBU_PD_HSIO_S] = { "sswrp_hsio_s", "hsio_s.v", 0x1a, -1 },
	[MBU_PD_HSIO_S_UFS_PREP] = { "hsio_s_ufs_prep", "HsioSUfsPrep.v", 0x222, MBU_PD_HSIO_S },
	[MBU_PD_HSIO_S_UFS] = { "hsio_s_ufs", "HsioSUfs.v", 0x220, MBU_PD_HSIO_S_UFS_PREP },
	[MBU_PD_HSIO_S_SD] = { "hsio_s_sd", "HsioSSd.v", 0x221, MBU_PD_HSIO_S_UFS },
	[MBU_PD_ISPFE] = { "sswrp_ispfe", "ispfe.v", 0x1c, -1 },
	[MBU_PD_ISPFE_CORE0] = { "ispfe_core0", "IspfeCore0.v", 0x2a0, MBU_PD_ISPFE },
	[MBU_PD_ISPFE_CORE1] = { "ispfe_core1", "IspfeCore1.v", 0x2a1, MBU_PD_ISPFE },
	[MBU_PD_ISPFE_CORE2] = { "ispfe_core2", "IspfeCore2.v", 0x2a2, MBU_PD_ISPFE },
	[MBU_PD_ISPFE_CSIS] = { "ispfe_csis", "IspfeCsis.v", 0x2a3, MBU_PD_ISPFE },
	[MBU_PD_ISPBE] = { "sswrp_ispbe", "ispbe.v", 0x1b, -1 },
	[MBU_PD_LSIO_E] = { "sswrp_lsio_e", "lsio_e.v", 0x1d, -1 },
	[MBU_PD_LSIO_E_CLI_GPIO] = { "lsio_e_cli_gpio", "LsioECli.v", 0x230, MBU_PD_LSIO_E },
	[MBU_PD_LSIO_S] = { "sswrp_lsio_s", "lsio_s.v", 0x1e, -1 },
	[MBU_PD_LSIO_S_CLI_GPIO] = { "lsio_s_cli_gpio", "LsioSCli.v", 0x240, MBU_PD_LSIO_S },
	[MBU_PD_PCIE] = { "sswrp_pcie", "pcie.v", 0x20, -1 },
	[MBU_PD_PCIE_TOP] = { "pcie_top", "PcieTop.v", 0x252, MBU_PD_PCIE },
	[MBU_PD_PCIE_CTRL0] = { "pcie_ctrl0", "PcieCtrl0.v", 0x250, MBU_PD_PCIE_TOP },
	[MBU_PD_PCIE_CTRL1] = { "pcie_ctrl1", "PcieCtrl1.v", 0x251, MBU_PD_PCIE_TOP },
	[MBU_PD_TPU] = { "sswrp_tpu", "tpu.v", 0x21, -1 },
	[MBU_PD_MEMSS_DTA] = { "memss_dta", "MemssDta.v", 0x270, -1 },
};

struct mbu_pd_provider;

struct mbu_pd {
	struct generic_pm_domain genpd;
	struct mbu_pd_provider *pp;
	const struct mbu_pd_desc *desc;
	u32 handle;
	struct completion done;
};

struct mbu_pd_provider {
	struct device *dev;
	struct google_cpm *cpm;
	struct mbu_pd pds[ARRAY_SIZE(mbu_pd_descs)];
	struct generic_pm_domain *genpds[ARRAY_SIZE(mbu_pd_descs)];
	struct genpd_onecell_data data;
};

#define to_mbu_pd(gpd) container_of(gpd, struct mbu_pd, genpd)

static int mbu_pd_vote(struct mbu_pd *pd, u64 vote)
{
	int ret;

	reinit_completion(&pd->done);

	ret = google_cpm_mbfs_write(pd->pp->cpm, pd->handle, vote);
	if (ret == -EAGAIN) {
		/* Accepted, the CPM notifies us once the transition is done. */
		if (wait_for_completion_timeout(&pd->done,
						msecs_to_jiffies(MBU_PD_TIMEOUT_MS)))
			ret = 0;
		else
			ret = -ETIMEDOUT;
	}

	if (ret)
		dev_err(pd->pp->dev, "%s: failed to vote %llu: %d\n",
			pd->desc->name, vote, ret);

	return ret;
}

static int mbu_pd_power_on(struct generic_pm_domain *genpd)
{
	return mbu_pd_vote(to_mbu_pd(genpd), MBU_PD_VOTE_ON);
}

static int mbu_pd_power_off(struct generic_pm_domain *genpd)
{
	return mbu_pd_vote(to_mbu_pd(genpd), MBU_PD_VOTE_OFF);
}

static void mbu_pd_notify(void *data, const u32 msg[GOOGLE_CPM_PAYLOAD_WORDS])
{
	struct mbu_pd_provider *pp = data;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(pp->pds); i++) {
		if (pp->pds[i].desc->sid == msg[0]) {
			complete(&pp->pds[i].done);
			return;
		}
	}

	dev_dbg(pp->dev, "notification for unknown resource %#x\n", msg[0]);
}

static void mbu_pd_remove_provider(void *np)
{
	of_genpd_del_provider(np);
}

static int mbu_pd_probe(struct auxiliary_device *adev,
			const struct auxiliary_device_id *id)
{
	struct device *dev = &adev->dev;
	struct device_node *np = dev->parent->of_node;
	struct mbu_pd_provider *pp;
	unsigned int i;
	u32 root;
	int ret;

	pp = devm_kzalloc(dev, sizeof(*pp), GFP_KERNEL);
	if (!pp)
		return -ENOMEM;

	pp->dev = dev;
	pp->cpm = google_cpm_get(dev);

	ret = google_cpm_mbfs_get_handle(pp->cpm, MBU_PD_VOTE_ROOT, &root);
	if (ret)
		return dev_err_probe(dev, ret, "no SysPM vote folder\n");

	for (i = 0; i < ARRAY_SIZE(mbu_pd_descs); i++) {
		const struct mbu_pd_desc *desc = &mbu_pd_descs[i];
		struct mbu_pd *pd = &pp->pds[i];
		u64 vote;

		pd->pp = pp;
		pd->desc = desc;
		init_completion(&pd->done);

		ret = google_cpm_mbfs_lookup(pp->cpm, root, desc->vote, &pd->handle);
		if (!ret)
			ret = google_cpm_mbfs_read(pp->cpm, pd->handle, &vote);
		if (ret)
			return dev_err_probe(dev, ret, "%s: no vote file\n", desc->name);

		pd->genpd.name = desc->name;
		pd->genpd.power_on = mbu_pd_power_on;
		pd->genpd.power_off = mbu_pd_power_off;
		if (desc->always_on)
			pd->genpd.flags |= GENPD_FLAG_ALWAYS_ON;

		ret = pm_genpd_init(&pd->genpd, NULL, vote == MBU_PD_VOTE_OFF);
		if (ret)
			return ret;

		if (desc->parent >= 0) {
			ret = pm_genpd_add_subdomain(&pp->pds[desc->parent].genpd,
						     &pd->genpd);
			if (ret)
				return ret;
		}

		pp->genpds[i] = &pd->genpd;
	}

	ret = google_cpm_register_notifier(pp->cpm, GOOGLE_CPM_AP_SVC_POWER,
					   mbu_pd_notify, pp);
	if (ret)
		return ret;

	pp->data.domains = pp->genpds;
	pp->data.num_domains = ARRAY_SIZE(pp->genpds);

	ret = of_genpd_add_provider_onecell(np, &pp->data);
	if (ret)
		return ret;

	return devm_add_action_or_reset(dev, mbu_pd_remove_provider, np);
}

static const struct auxiliary_device_id mbu_pd_id_table[] = {
	{ .name = "google_cpm.pd" },
	{ }
};
MODULE_DEVICE_TABLE(auxiliary, mbu_pd_id_table);

static struct auxiliary_driver mbu_pd_driver = {
	.probe = mbu_pd_probe,
	.id_table = mbu_pd_id_table,
	.driver = {
		.suppress_bind_attrs = true,
	},
};
module_auxiliary_driver(mbu_pd_driver);

MODULE_DESCRIPTION("Google Tensor G6 power domain driver");
MODULE_LICENSE("GPL");
