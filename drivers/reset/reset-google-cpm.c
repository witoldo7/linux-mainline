// SPDX-License-Identifier: GPL-2.0-only
/*
 * Google Tensor CPM reset driver
 *
 * Resets are owned by the Central Power Manager and grouped per subsystem
 * clock manager (LPCM). Consumers refer to a reset by its LPCM and reset id,
 * which are part of the CPM firmware interface.
 *
 * Based on the downstream Pixel driver.
 *
 * Copyright 2023-2025 Google LLC
 */

#include <linux/auxiliary_bus.h>
#include <linux/bitfield.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/reset-controller.h>
#include <linux/soc/google/google-cpm.h>

#define LPCM_CMD_SET_RST	0x4

#define RST_REQ_CMD		GENMASK(7, 0)
#define RST_REQ_LPCM		GENMASK(15, 8)
#define RST_REQ_ID		GENMASK(23, 16)
#define RST_REQ_OP		GENMASK(31, 24)

#define RST_OP_ASSERT		0
#define RST_OP_DEASSERT		1

/* Reset ids handed out by of_xlate: LPCM in bits 15:8, reset in bits 7:0. */
#define RST_ID_LPCM		GENMASK(15, 8)
#define RST_ID_RESET		GENMASK(7, 0)

struct google_cpm_reset {
	struct reset_controller_dev rcdev;
	struct google_cpm *cpm;
};

#define to_google_cpm_reset(r) container_of(r, struct google_cpm_reset, rcdev)

static int google_cpm_reset_op(struct reset_controller_dev *rcdev,
			       unsigned long id, u8 op)
{
	struct google_cpm_reset *rst = to_google_cpm_reset(rcdev);
	u32 req[GOOGLE_CPM_PAYLOAD_WORDS] = {
		FIELD_PREP(RST_REQ_CMD, LPCM_CMD_SET_RST) |
		FIELD_PREP(RST_REQ_LPCM, FIELD_GET(RST_ID_LPCM, id)) |
		FIELD_PREP(RST_REQ_ID, FIELD_GET(RST_ID_RESET, id)) |
		FIELD_PREP(RST_REQ_OP, op),
	};
	u32 resp[GOOGLE_CPM_PAYLOAD_WORDS];
	int ret;

	ret = google_cpm_request(rst->cpm, GOOGLE_CPM_SVC_LPCM, req, resp);
	if (ret)
		return ret;

	if (resp[0]) {
		dev_err(rcdev->dev, "reset %#lx op %u failed: %d\n", id, op,
			(s32)resp[0]);
		return -EIO;
	}

	return 0;
}

static int google_cpm_reset_assert(struct reset_controller_dev *rcdev,
				   unsigned long id)
{
	return google_cpm_reset_op(rcdev, id, RST_OP_ASSERT);
}

static int google_cpm_reset_deassert(struct reset_controller_dev *rcdev,
				     unsigned long id)
{
	return google_cpm_reset_op(rcdev, id, RST_OP_DEASSERT);
}

static const struct reset_control_ops google_cpm_reset_ops = {
	.assert		= google_cpm_reset_assert,
	.deassert	= google_cpm_reset_deassert,
};

static int google_cpm_reset_xlate(struct reset_controller_dev *rcdev,
				  const struct of_phandle_args *args)
{
	if (args->args[0] > FIELD_MAX(RST_ID_LPCM) ||
	    args->args[1] > FIELD_MAX(RST_ID_RESET))
		return -EINVAL;

	return FIELD_PREP(RST_ID_LPCM, args->args[0]) |
	       FIELD_PREP(RST_ID_RESET, args->args[1]);
}

static int google_cpm_reset_probe(struct auxiliary_device *adev,
				  const struct auxiliary_device_id *id)
{
	struct device *dev = &adev->dev;
	struct google_cpm_reset *rst;

	rst = devm_kzalloc(dev, sizeof(*rst), GFP_KERNEL);
	if (!rst)
		return -ENOMEM;

	rst->cpm = google_cpm_get(dev);

	/* The provider node is the CPM node itself. */
	rst->rcdev.of_node = dev->parent->of_node;
	rst->rcdev.dev = dev;
	rst->rcdev.owner = THIS_MODULE;
	rst->rcdev.ops = &google_cpm_reset_ops;
	rst->rcdev.of_reset_n_cells = 2;
	rst->rcdev.of_xlate = google_cpm_reset_xlate;
	rst->rcdev.nr_resets = FIELD_MAX(RST_ID_LPCM | RST_ID_RESET) + 1;

	return devm_reset_controller_register(dev, &rst->rcdev);
}

static const struct auxiliary_device_id google_cpm_reset_id_table[] = {
	{ .name = "google_cpm.reset" },
	{ }
};
MODULE_DEVICE_TABLE(auxiliary, google_cpm_reset_id_table);

static struct auxiliary_driver google_cpm_reset_driver = {
	.probe = google_cpm_reset_probe,
	.id_table = google_cpm_reset_id_table,
};
module_auxiliary_driver(google_cpm_reset_driver);

MODULE_DESCRIPTION("Google Tensor CPM reset driver");
MODULE_LICENSE("GPL");
