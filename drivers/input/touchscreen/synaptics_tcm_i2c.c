// SPDX-License-Identifier: GPL-2.0-only
/*
 * Synaptics TouchComm (TCM) touchscreen driver, I2C transport.
 */

#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/module.h>

#include "synaptics_tcm.h"

/*
 * Read length limit of the controller (RD_CHUNK_SIZE in the vendor driver).
 * One read may not exceed this, including the header.
 */
#define SYNA_I2C_RD_CHUNK_SIZE		64

static int syna_tcm_i2c_read(struct device *dev, void *buf, size_t len)
{
	int ret;

	ret = i2c_master_recv(to_i2c_client(dev), buf, len);
	if (ret < 0)
		return ret;

	return ret == len ? 0 : -EIO;
}

static int syna_tcm_i2c_write(struct device *dev, const void *buf, size_t len)
{
	int ret;

	ret = i2c_master_send(to_i2c_client(dev), buf, len);
	if (ret < 0)
		return ret;

	return ret == len ? 0 : -EIO;
}

static const struct syna_tcm_bus syna_tcm_i2c_bus = {
	.read = syna_tcm_i2c_read,
	.write = syna_tcm_i2c_write,
	.bustype = BUS_I2C,
	.rd_chunk_size = SYNA_I2C_RD_CHUNK_SIZE,
	/* Messages up to a chunk long come in one read. */
	.first_read_size = SYNA_I2C_RD_CHUNK_SIZE,
};

/* Vendor driver: POWERUP_TO_RESET_TIME and RESET_TO_NORMAL_TIME. */
static const struct syna_tcm_chip_data syna_s3908_data = {
	.powerup_to_reset_ms = 10,
	.reset_to_ready_ms = 1000,
};

static int syna_tcm_i2c_probe(struct i2c_client *client)
{
	return syna_tcm_probe(&client->dev, client->irq, &syna_tcm_i2c_bus,
			      i2c_get_match_data(client));
}

static const struct i2c_device_id syna_tcm_i2c_id[] = {
	{ "synaptics-tcm", (kernel_ulong_t)&syna_s3908_data },
	{ }
};
MODULE_DEVICE_TABLE(i2c, syna_tcm_i2c_id);

static const struct of_device_id syna_tcm_i2c_of_match[] = {
	{ .compatible = "syna,s3908", .data = &syna_s3908_data },
	{ }
};
MODULE_DEVICE_TABLE(of, syna_tcm_i2c_of_match);

static struct i2c_driver syna_tcm_i2c_driver = {
	.driver = {
		.name = "synaptics-tcm-i2c",
		.of_match_table = syna_tcm_i2c_of_match,
		.pm = pm_sleep_ptr(&syna_tcm_pm_ops),
	},
	.probe = syna_tcm_i2c_probe,
	.id_table = syna_tcm_i2c_id,
};
module_i2c_driver(syna_tcm_i2c_driver);

MODULE_DESCRIPTION("Synaptics TouchComm I2C touchscreen driver");
MODULE_AUTHOR("Vsevolod Nevorotov <sevanevorotov29@gmail.com>");
MODULE_LICENSE("GPL");
