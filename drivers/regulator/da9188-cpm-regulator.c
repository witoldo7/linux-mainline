// SPDX-License-Identifier: GPL-2.0-only
/*
 * Regulators of the Renesas (Dialog) DA9188/DA9189 AP PMIC pair on Google
 * Tensor G6 (malibu) boards.
 *
 * The PMICs sit on a bus owned by the Central Power Manager, so every
 * access is a request to the CPM's PMIC service.
 *
 * Based on the downstream Pixel driver.
 *
 * Copyright 2024-2025 Google LLC
 */

#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/soc/google/google-cpm.h>

/* Regulator commands of the CPM PMIC service */
#define REG_CMD_GET_IS_ENABLED		1
#define REG_CMD_SET_ENABLE		2
#define REG_CMD_GET_VOLTAGE_SEL		3
#define REG_CMD_SET_VOLTAGE_SEL		4

static int da9188_request(struct regulator_dev *rdev, u8 cmd, u32 arg, u32 *result)
{
	struct google_cpm *cpm = rdev_get_drvdata(rdev);

	return google_cpm_pmic_request(cpm, GOOGLE_CPM_PMIC_REGULATOR, cmd,
				       rdev_get_id(rdev), arg, result);
}

static int da9188_is_enabled(struct regulator_dev *rdev)
{
	u32 enabled;
	int ret;

	ret = da9188_request(rdev, REG_CMD_GET_IS_ENABLED, 0, &enabled);

	return ret ? ret : !!enabled;
}

static int da9188_enable(struct regulator_dev *rdev)
{
	return da9188_request(rdev, REG_CMD_SET_ENABLE, 1, NULL);
}

static int da9188_disable(struct regulator_dev *rdev)
{
	return da9188_request(rdev, REG_CMD_SET_ENABLE, 0, NULL);
}

static int da9188_get_voltage_sel(struct regulator_dev *rdev)
{
	u32 sel;
	int ret;

	ret = da9188_request(rdev, REG_CMD_GET_VOLTAGE_SEL, 0, &sel);

	return ret ? ret : sel;
}

static int da9188_set_voltage_sel(struct regulator_dev *rdev, unsigned int sel)
{
	return da9188_request(rdev, REG_CMD_SET_VOLTAGE_SEL, sel, NULL);
}

static const struct regulator_ops da9188_ops = {
	.list_voltage		= regulator_list_voltage_linear,
	.map_voltage		= regulator_map_voltage_linear,
	.get_voltage_sel	= da9188_get_voltage_sel,
	.set_voltage_sel	= da9188_set_voltage_sel,
	.is_enabled		= da9188_is_enabled,
	.enable			= da9188_enable,
	.disable		= da9188_disable,
};

#define DA9188_RAIL(_name, _id, _min, _step)		\
	{						\
		.name = #_name,				\
		.of_match = #_name,			\
		.regulators_node = "regulators",	\
		.ops = &da9188_ops,			\
		.type = REGULATOR_VOLTAGE,		\
		.id = _id,				\
		.owner = THIS_MODULE,			\
		.n_voltages = 256,			\
		.min_uV = _min,				\
		.uV_step = _step,			\
	}

#define DA9188_BUCK(_name, _id, _min, _step)	DA9188_RAIL(_name, _id, _min, _step)
#define DA9188_LDO(_name, _id, _min, _step)	DA9188_RAIL(_name, _id, _min, _step)

/* The ids are part of the CPM interface: DA9188 ("m") rails, then DA9189 ("s"). */
static const struct regulator_desc da9188_regulators[] = {
	DA9188_BUCK(buck_1m, 0, 240000, 5000),
	DA9188_BUCK(buck_2m, 1, 240000, 5000),
	DA9188_BUCK(buck_3m, 2, 240000, 5000),
	DA9188_BUCK(buck_4m, 3, 240000, 5000),
	DA9188_BUCK(buck_5m, 4, 240000, 5000),
	DA9188_BUCK(buck_6m, 5, 240000, 5000),
	DA9188_BUCK(buck_7m, 6, 240000, 5000),
	DA9188_BUCK(buck_8m, 7, 240000, 5000),
	DA9188_BUCK(buck_9m, 8, 240000, 5000),
	DA9188_BUCK(buck_10m, 9, 240000, 5000),
	DA9188_BUCK(buck_11m, 10, 240000, 5000),
	DA9188_BUCK(buck_12m, 11, 240000, 5000),
	DA9188_BUCK(buck_13m, 12, 240000, 5000),
	DA9188_LDO(ldo_1m, 13, 400000, 5000),
	DA9188_LDO(ldo_2m, 14, 1200000, 10000),
	DA9188_LDO(ldo_3m, 15, 400000, 5000),
	DA9188_LDO(ldo_4m, 16, 1200000, 10000),
	DA9188_LDO(ldo_5m, 17, 1200000, 10000),
	DA9188_LDO(ldo_6m, 18, 400000, 5000),
	DA9188_LDO(ldo_7m, 19, 960000, 15000),
	DA9188_LDO(ldo_8m, 20, 400000, 5000),
	DA9188_LDO(ldo_9m, 21, 400000, 5000),
	DA9188_LDO(ldo_10m, 22, 400000, 5000),
	DA9188_LDO(ldo_11m, 23, 400000, 5000),
	DA9188_LDO(ldo_12m, 24, 400000, 5000),
	DA9188_LDO(ldo_13m, 25, 400000, 5000),
	DA9188_LDO(ldo_14m, 26, 1200000, 10000),
	DA9188_LDO(ldo_15m, 27, 400000, 5000),
	DA9188_LDO(ldo_16m, 28, 400000, 5000),
	DA9188_LDO(ldo_17m, 29, 400000, 5000),
	DA9188_LDO(ldo_18m, 30, 400000, 5000),
	DA9188_LDO(ldo_19m, 31, 1200000, 10000),
	DA9188_LDO(ldo_20m, 32, 1200000, 10000),
	DA9188_LDO(ldo_21m, 33, 1200000, 10000),
	DA9188_LDO(ldo_22m, 34, 400000, 5000),
	DA9188_LDO(ldo_23m, 35, 1200000, 10000),
	DA9188_LDO(ldo_24m, 36, 1200000, 10000),
	DA9188_LDO(ldo_25m, 37, 1200000, 10000),
	DA9188_LDO(ldo_26m, 38, 1200000, 10000),
	DA9188_LDO(ldo_27m, 39, 1200000, 10000),
	DA9188_LDO(ldo_28m, 40, 400000, 5000),
	DA9188_BUCK(buck_1s, 41, 240000, 5000),
	DA9188_BUCK(buck_2s, 42, 240000, 5000),
	DA9188_BUCK(buck_3s, 43, 240000, 5000),
	DA9188_BUCK(buck_4s, 44, 240000, 5000),
	DA9188_BUCK(buck_5s, 45, 240000, 5000),
	DA9188_BUCK(buck_6s, 46, 240000, 5000),
	DA9188_BUCK(buck_7s, 47, 800000, 10000),
	DA9188_BUCK(buck_8s, 48, 240000, 5000),
	DA9188_BUCK(buck_9s, 49, 240000, 5000),
	DA9188_BUCK(buck_10s, 50, 240000, 5000),
	DA9188_BUCK(buck_11s, 51, 800000, 10000),
	DA9188_BUCK(buck_12s, 52, 800000, 10000),
	DA9188_BUCK(buck_13s, 53, 240000, 5000),
	DA9188_BUCK(buck_boost, 54, 1300000, 10000),
	DA9188_LDO(ldo_1s, 55, 400000, 5000),
	DA9188_LDO(ldo_2s, 56, 400000, 5000),
	DA9188_LDO(ldo_3s, 57, 1200000, 10000),
	DA9188_LDO(ldo_4s, 58, 1200000, 10000),
	DA9188_LDO(ldo_5s, 59, 1200000, 10000),
	DA9188_LDO(ldo_6s, 60, 400000, 5000),
	DA9188_LDO(ldo_7s, 61, 1200000, 10000),
	DA9188_LDO(ldo_8s, 62, 1200000, 10000),
	DA9188_LDO(ldo_9s, 63, 400000, 5000),
	DA9188_LDO(ldo_10s, 64, 400000, 5000),
	DA9188_LDO(ldo_11s, 65, 400000, 5000),
	DA9188_LDO(ldo_12s, 66, 400000, 5000),
	DA9188_LDO(ldo_13s, 67, 400000, 5000),
	DA9188_LDO(ldo_14s, 68, 400000, 5000),
	DA9188_LDO(ldo_15s, 69, 1200000, 10000),
	DA9188_LDO(ldo_16s, 70, 1200000, 10000),
	DA9188_LDO(ldo_17s, 71, 1200000, 10000),
	DA9188_LDO(ldo_18s, 72, 1200000, 10000),
	DA9188_LDO(ldo_19s, 73, 1200000, 10000),
	DA9188_LDO(ldo_20s, 74, 1200000, 10000),
	DA9188_LDO(ldo_21s, 75, 400000, 5000),
	DA9188_LDO(ldo_22s, 76, 1200000, 10000),
	DA9188_LDO(ldo_23s, 77, 400000, 5000),
	DA9188_LDO(ldo_24s, 78, 400000, 5000),
	DA9188_LDO(ldo_25s, 79, 1200000, 10000),
	DA9188_LDO(ldo_26s, 80, 1200000, 10000),
	DA9188_LDO(ldo_27s, 81, 1200000, 10000),
	DA9188_LDO(ldo_28s, 82, 1200000, 10000),};

static int da9188_regulator_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct regulator_config config = {
		.dev = dev,
		.driver_data = google_cpm_get(dev),
	};
	struct regulator_dev *rdev;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(da9188_regulators); i++) {
		rdev = devm_regulator_register(dev, &da9188_regulators[i], &config);
		if (IS_ERR(rdev))
			return dev_err_probe(dev, PTR_ERR(rdev), "failed to register %s\n",
					     da9188_regulators[i].name);
	}

	return 0;
}

static const struct of_device_id da9188_regulator_of_match[] = {
	{ .compatible = "google,mbu-pmic-regulators" },
	{ }
};
MODULE_DEVICE_TABLE(of, da9188_regulator_of_match);

static struct platform_driver da9188_regulator_driver = {
	.probe = da9188_regulator_probe,
	.driver = {
		.name = "da9188-cpm-regulator",
		.of_match_table = da9188_regulator_of_match,
		.probe_type = PROBE_PREFER_ASYNCHRONOUS,
	},
};
module_platform_driver(da9188_regulator_driver);

MODULE_DESCRIPTION("DA9188/DA9189 regulators behind the Google Tensor CPM");
MODULE_LICENSE("GPL");
