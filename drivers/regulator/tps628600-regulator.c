// SPDX-License-Identifier: GPL-2.0-only OR MIT
/*
 * Texas Instruments TPS628600 step-down converter
 *
 * The converter is switched on and off through its enable pin and loses its
 * register contents while off, so the output voltage is kept in the driver
 * and programmed again after every enable.
 *
 * Based on the downstream Pixel driver.
 *
 * Copyright 2023-2025 Google LLC
 */

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/of_regulator.h>

#define TPS628600_REG_VOUT2		0x02
#define TPS628600_REG_CONTROL		0x03

/* Output discharge disabled, other settings at their defaults */
#define TPS628600_CONTROL_DEFAULT	0x63

#define TPS628600_MIN_UV		400000
#define TPS628600_STEP_UV		12500
#define TPS628600_N_VOLTAGES		128
#define TPS628600_VOUT2_DEFAULT		0x38

/* Time for the converter to accept I2C accesses after enable */
#define TPS628600_ENABLE_DELAY_US	1000

struct tps628600 {
	struct i2c_client *client;
	struct gpio_desc *enable_gpio;
	u8 vsel;
};

static int tps628600_write(struct tps628600 *tps, u8 reg, u8 val)
{
	return i2c_smbus_write_byte_data(tps->client, reg, val);
}

static int tps628600_is_enabled(struct regulator_dev *rdev)
{
	struct tps628600 *tps = rdev_get_drvdata(rdev);

	return gpiod_get_value_cansleep(tps->enable_gpio);
}

static int tps628600_enable(struct regulator_dev *rdev)
{
	struct tps628600 *tps = rdev_get_drvdata(rdev);
	int ret;

	gpiod_set_value_cansleep(tps->enable_gpio, 1);
	usleep_range(TPS628600_ENABLE_DELAY_US, TPS628600_ENABLE_DELAY_US + 100);

	ret = tps628600_write(tps, TPS628600_REG_VOUT2, tps->vsel);
	if (ret)
		return ret;

	return tps628600_write(tps, TPS628600_REG_CONTROL, TPS628600_CONTROL_DEFAULT);
}

static int tps628600_disable(struct regulator_dev *rdev)
{
	struct tps628600 *tps = rdev_get_drvdata(rdev);

	gpiod_set_value_cansleep(tps->enable_gpio, 0);

	return 0;
}

static int tps628600_get_voltage_sel(struct regulator_dev *rdev)
{
	struct tps628600 *tps = rdev_get_drvdata(rdev);

	return tps->vsel;
}

static int tps628600_set_voltage_sel(struct regulator_dev *rdev, unsigned int sel)
{
	struct tps628600 *tps = rdev_get_drvdata(rdev);

	tps->vsel = sel;

	/* Applied at the next enable if the converter is off. */
	if (!gpiod_get_value_cansleep(tps->enable_gpio))
		return 0;

	return tps628600_write(tps, TPS628600_REG_VOUT2, sel);
}

static const struct regulator_ops tps628600_ops = {
	.list_voltage		= regulator_list_voltage_linear,
	.map_voltage		= regulator_map_voltage_linear,
	.is_enabled		= tps628600_is_enabled,
	.enable			= tps628600_enable,
	.disable		= tps628600_disable,
	.get_voltage_sel	= tps628600_get_voltage_sel,
	.set_voltage_sel	= tps628600_set_voltage_sel,
};

static const struct regulator_desc tps628600_desc = {
	.name		= "tps628600",
	.type		= REGULATOR_VOLTAGE,
	.owner		= THIS_MODULE,
	.ops		= &tps628600_ops,
	.min_uV		= TPS628600_MIN_UV,
	.uV_step	= TPS628600_STEP_UV,
	.n_voltages	= TPS628600_N_VOLTAGES,
};

static int tps628600_probe(struct i2c_client *client)
{
	struct device *dev = &client->dev;
	struct regulator_config config = { };
	struct regulator_dev *rdev;
	struct tps628600 *tps;
	int ret;

	tps = devm_kzalloc(dev, sizeof(*tps), GFP_KERNEL);
	if (!tps)
		return -ENOMEM;

	tps->client = client;
	tps->vsel = TPS628600_VOUT2_DEFAULT;

	/* The bootloader may already have the converter running for the panel. */
	tps->enable_gpio = devm_gpiod_get(dev, "enable", GPIOD_ASIS);
	if (IS_ERR(tps->enable_gpio))
		return dev_err_probe(dev, PTR_ERR(tps->enable_gpio), "no enable GPIO\n");

	if (gpiod_get_value_cansleep(tps->enable_gpio) > 0) {
		ret = i2c_smbus_read_byte_data(client, TPS628600_REG_VOUT2);
		if (ret < 0)
			return dev_err_probe(dev, ret, "failed to read output voltage\n");
		tps->vsel = ret;
	}

	config.dev = dev;
	config.of_node = dev->of_node;
	config.driver_data = tps;
	config.init_data = of_get_regulator_init_data(dev, dev->of_node, &tps628600_desc);

	rdev = devm_regulator_register(dev, &tps628600_desc, &config);

	return PTR_ERR_OR_ZERO(rdev);
}

static const struct of_device_id tps628600_of_match[] = {
	{ .compatible = "ti,tps628600" },
	{ }
};
MODULE_DEVICE_TABLE(of, tps628600_of_match);

static struct i2c_driver tps628600_driver = {
	.probe = tps628600_probe,
	.driver = {
		.name = "tps628600",
		.of_match_table = tps628600_of_match,
	},
};
module_i2c_driver(tps628600_driver);

MODULE_DESCRIPTION("TI TPS628600 step-down converter driver");
MODULE_LICENSE("Dual MIT/GPL");
