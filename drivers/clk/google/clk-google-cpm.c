// SPDX-License-Identifier: GPL-2.0-only
/*
 * Google Tensor CPM clock driver
 *
 * Clocks are owned by the Central Power Manager and grouped per subsystem
 * clock manager (LPCM). Consumers refer to a clock by its LPCM and clock id,
 * which are part of the CPM firmware interface. Clocks are registered the
 * first time they are looked up.
 *
 * Based on the downstream Pixel driver.
 *
 * Copyright 2023-2025 Google LLC
 */

#include <linux/auxiliary_bus.h>
#include <linux/bitfield.h>
#include <linux/cleanup.h>
#include <linux/clk-provider.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/slab.h>
#include <linux/soc/google/google-cpm.h>

#define LPCM_CMD_SET_CLK	0x2

#define CLK_REQ_CMD		GENMASK(7, 0)
#define CLK_REQ_LPCM		GENMASK(15, 8)
#define CLK_REQ_CLOCK		GENMASK(23, 16)
#define CLK_REQ_OP		GENMASK(31, 24)

#define CLK_OP_PREPARE		0
#define CLK_OP_UNPREPARE	1
#define CLK_OP_RECALC_RATE	3
#define CLK_OP_SET_RATE		5

/* Argument of the (un)prepare operations: plain clock gating. */
#define CLK_GATE_NORMAL		1

/* Error codes returned by the LPCM service. */
#define LPCM_ERR_GENERIC	-1
#define LPCM_ERR_UNAVAILABLE	-3
#define LPCM_ERR_INVALID_ARGS	-8
#define LPCM_ERR_TIMED_OUT	-13
#define LPCM_ERR_NOT_SUPPORTED	-24

struct google_cpm_clk_provider {
	struct device *dev;
	struct google_cpm *cpm;
	/* Protects @clks. */
	struct mutex lock;
	struct list_head clks;
};

struct google_cpm_clk {
	struct clk_hw hw;
	struct google_cpm_clk_provider *cp;
	struct list_head node;
	u8 lpcm;
	u8 id;
};

#define to_google_cpm_clk(_hw) container_of(_hw, struct google_cpm_clk, hw)

static int google_cpm_clk_errno(s32 err)
{
	switch (err) {
	case LPCM_ERR_INVALID_ARGS:
		return -EINVAL;
	case LPCM_ERR_TIMED_OUT:
		return -ETIMEDOUT;
	case LPCM_ERR_UNAVAILABLE:
		return -ENODEV;
	case LPCM_ERR_NOT_SUPPORTED:
		return -EOPNOTSUPP;
	default:
		return -EIO;
	}
}

/* Returns a non-negative result from the CPM, or a negative errno. */
static long google_cpm_clk_op(struct google_cpm_clk *clk, u8 op, u32 arg)
{
	u32 req[GOOGLE_CPM_PAYLOAD_WORDS] = {
		FIELD_PREP(CLK_REQ_CMD, LPCM_CMD_SET_CLK) |
		FIELD_PREP(CLK_REQ_LPCM, clk->lpcm) |
		FIELD_PREP(CLK_REQ_CLOCK, clk->id) |
		FIELD_PREP(CLK_REQ_OP, op),
		arg,
	};
	u32 resp[GOOGLE_CPM_PAYLOAD_WORDS];
	s32 result;
	int ret;

	ret = google_cpm_request(clk->cp->cpm, GOOGLE_CPM_SVC_LPCM, req, resp);
	if (ret)
		return ret;

	result = resp[0];
	if (result < 0) {
		dev_dbg(clk->cp->dev, "clock %u:%u op %u failed: %d\n",
			clk->lpcm, clk->id, op, result);
		return google_cpm_clk_errno(result);
	}

	return result;
}

static int google_cpm_clk_prepare(struct clk_hw *hw)
{
	long ret = google_cpm_clk_op(to_google_cpm_clk(hw), CLK_OP_PREPARE,
				     CLK_GATE_NORMAL);

	return ret < 0 ? ret : 0;
}

static void google_cpm_clk_unprepare(struct clk_hw *hw)
{
	google_cpm_clk_op(to_google_cpm_clk(hw), CLK_OP_UNPREPARE,
			  CLK_GATE_NORMAL);
}

static unsigned long google_cpm_clk_recalc_rate(struct clk_hw *hw,
						unsigned long parent_rate)
{
	long rate = google_cpm_clk_op(to_google_cpm_clk(hw),
				      CLK_OP_RECALC_RATE, 0);

	/* Pure gates have no rate of their own. */
	return rate < 0 ? 0 : rate;
}

static int google_cpm_clk_determine_rate(struct clk_hw *hw,
					 struct clk_rate_request *req)
{
	/* The CPM picks the closest supported rate on set_rate. */
	return 0;
}

static int google_cpm_clk_set_rate(struct clk_hw *hw, unsigned long rate,
				   unsigned long parent_rate)
{
	long ret;

	if (rate > U32_MAX)
		return -EINVAL;

	ret = google_cpm_clk_op(to_google_cpm_clk(hw), CLK_OP_SET_RATE, rate);

	return ret < 0 ? ret : 0;
}

static const struct clk_ops google_cpm_clk_ops = {
	.prepare	= google_cpm_clk_prepare,
	.unprepare	= google_cpm_clk_unprepare,
	.recalc_rate	= google_cpm_clk_recalc_rate,
	.determine_rate	= google_cpm_clk_determine_rate,
	.set_rate	= google_cpm_clk_set_rate,
};

static struct clk_hw *google_cpm_clk_xlate(struct of_phandle_args *args,
					   void *data)
{
	struct google_cpm_clk_provider *cp = data;
	struct clk_init_data init = { };
	struct google_cpm_clk *clk;
	int ret;

	if (args->args_count != 2 || args->args[0] > U8_MAX ||
	    args->args[1] > U8_MAX)
		return ERR_PTR(-EINVAL);

	guard(mutex)(&cp->lock);

	list_for_each_entry(clk, &cp->clks, node)
		if (clk->lpcm == args->args[0] && clk->id == args->args[1])
			return &clk->hw;

	clk = devm_kzalloc(cp->dev, sizeof(*clk), GFP_KERNEL);
	if (!clk)
		return ERR_PTR(-ENOMEM);

	clk->cp = cp;
	clk->lpcm = args->args[0];
	clk->id = args->args[1];

	init.name = devm_kasprintf(cp->dev, GFP_KERNEL, "cpm-%u-%u",
				   clk->lpcm, clk->id);
	if (!init.name)
		return ERR_PTR(-ENOMEM);
	init.ops = &google_cpm_clk_ops;
	init.flags = CLK_GET_RATE_NOCACHE;
	clk->hw.init = &init;

	ret = devm_clk_hw_register(cp->dev, &clk->hw);
	if (ret)
		return ERR_PTR(ret);

	list_add(&clk->node, &cp->clks);

	return &clk->hw;
}

static void google_cpm_clk_del_provider(void *np)
{
	of_clk_del_provider(np);
}

static int google_cpm_clk_probe(struct auxiliary_device *adev,
				const struct auxiliary_device_id *id)
{
	struct device *dev = &adev->dev;
	struct google_cpm_clk_provider *cp;
	int ret;

	cp = devm_kzalloc(dev, sizeof(*cp), GFP_KERNEL);
	if (!cp)
		return -ENOMEM;

	cp->dev = dev;
	cp->cpm = google_cpm_get(dev);
	mutex_init(&cp->lock);
	INIT_LIST_HEAD(&cp->clks);

	/* The provider node is the CPM node itself. */
	ret = of_clk_add_hw_provider(dev->parent->of_node, google_cpm_clk_xlate, cp);
	if (ret)
		return ret;

	return devm_add_action_or_reset(dev, google_cpm_clk_del_provider,
					dev->parent->of_node);
}

static const struct auxiliary_device_id google_cpm_clk_id_table[] = {
	{ .name = "google_cpm.clk" },
	{ }
};
MODULE_DEVICE_TABLE(auxiliary, google_cpm_clk_id_table);

static struct auxiliary_driver google_cpm_clk_driver = {
	.probe = google_cpm_clk_probe,
	.id_table = google_cpm_clk_id_table,
};
module_auxiliary_driver(google_cpm_clk_driver);

MODULE_DESCRIPTION("Google Tensor CPM clock driver");
MODULE_LICENSE("GPL");
