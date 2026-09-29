// SPDX-License-Identifier: GPL-2.0-only
/*
 * GPIOs of the Renesas (Dialog) DA9188/DA9189 AP PMIC pair on Google Tensor
 * G6 (malibu) boards.
 *
 * The PMICs sit on a bus owned by the Central Power Manager, so every
 * access is a request to the CPM's PMIC service.
 *
 * Based on the downstream Pixel driver.
 *
 * Copyright 2024-2025 Google LLC
 */

#include <linux/gpio/driver.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/soc/google/google-cpm.h>

/* GPIO commands of the CPM PMIC service */
#define GPIO_CMD_GET_DIRECTION		1
#define GPIO_CMD_SET_DIRECTION		2
#define GPIO_CMD_GET_VALUE		3
#define GPIO_CMD_SET_VALUE		4

#define GPIO_DIR_INPUT			0
#define GPIO_DIR_OUTPUT			1

struct da9188_gpio {
	struct gpio_chip gc;
	struct google_cpm *cpm;
};

static int da9188_gpio_request(struct gpio_chip *gc, u8 cmd, unsigned int offset,
			       u32 arg, u32 *result)
{
	struct da9188_gpio *gpio = gpiochip_get_data(gc);

	return google_cpm_pmic_request(gpio->cpm, GOOGLE_CPM_PMIC_GPIO, cmd, offset,
				       arg, result);
}

static int da9188_gpio_get_direction(struct gpio_chip *gc, unsigned int offset)
{
	u32 dir;
	int ret;

	ret = da9188_gpio_request(gc, GPIO_CMD_GET_DIRECTION, offset, 0, &dir);
	if (ret)
		return ret;

	return dir == GPIO_DIR_OUTPUT ? GPIO_LINE_DIRECTION_OUT : GPIO_LINE_DIRECTION_IN;
}

static int da9188_gpio_direction_input(struct gpio_chip *gc, unsigned int offset)
{
	return da9188_gpio_request(gc, GPIO_CMD_SET_DIRECTION, offset, GPIO_DIR_INPUT, NULL);
}

static int da9188_gpio_set(struct gpio_chip *gc, unsigned int offset, int value)
{
	return da9188_gpio_request(gc, GPIO_CMD_SET_VALUE, offset, !!value, NULL);
}

static int da9188_gpio_direction_output(struct gpio_chip *gc, unsigned int offset,
					int value)
{
	int ret;

	ret = da9188_gpio_request(gc, GPIO_CMD_SET_DIRECTION, offset, GPIO_DIR_OUTPUT, NULL);
	if (ret)
		return ret;

	return da9188_gpio_set(gc, offset, value);
}

static int da9188_gpio_get(struct gpio_chip *gc, unsigned int offset)
{
	u32 value;
	int ret;

	ret = da9188_gpio_request(gc, GPIO_CMD_GET_VALUE, offset, 0, &value);

	return ret ? ret : !!value;
}

static int da9188_gpio_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct da9188_gpio *gpio;
	u32 ngpios;
	int ret;

	gpio = devm_kzalloc(dev, sizeof(*gpio), GFP_KERNEL);
	if (!gpio)
		return -ENOMEM;

	gpio->cpm = google_cpm_get(dev);

	ret = device_property_read_u32(dev, "ngpios", &ngpios);
	if (ret)
		return dev_err_probe(dev, ret, "missing ngpios\n");

	gpio->gc.label = dev_name(dev);
	gpio->gc.parent = dev;
	gpio->gc.owner = THIS_MODULE;
	gpio->gc.base = -1;
	gpio->gc.ngpio = ngpios;
	gpio->gc.can_sleep = true;
	gpio->gc.get_direction = da9188_gpio_get_direction;
	gpio->gc.direction_input = da9188_gpio_direction_input;
	gpio->gc.direction_output = da9188_gpio_direction_output;
	gpio->gc.get = da9188_gpio_get;
	gpio->gc.set = da9188_gpio_set;

	return devm_gpiochip_add_data(dev, &gpio->gc, gpio);
}

static const struct of_device_id da9188_gpio_of_match[] = {
	{ .compatible = "google,mbu-pmic-gpio" },
	{ }
};
MODULE_DEVICE_TABLE(of, da9188_gpio_of_match);

static struct platform_driver da9188_gpio_driver = {
	.probe = da9188_gpio_probe,
	.driver = {
		.name = "da9188-cpm-gpio",
		.of_match_table = da9188_gpio_of_match,
	},
};
module_platform_driver(da9188_gpio_driver);

MODULE_DESCRIPTION("DA9188/DA9189 GPIOs behind the Google Tensor CPM");
MODULE_LICENSE("GPL");
