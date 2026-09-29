// SPDX-License-Identifier: GPL-2.0-only
/*
 * Synaptics TouchComm (TCM) touchscreen driver, SPI transport.
 *
 * Commands are plain SPI writes and messages plain SPI reads, the bytes
 * clocked out by the host during a read are ignored by the controller.
 *
 * Based on the downstream Pixel driver.
 */

#include <linux/input.h>
#include <linux/module.h>
#include <linux/spi/spi.h>

#include "synaptics_tcm.h"

/* Read length limit of the controller on SPI (RD_CHUNK_SIZE in the vendor driver) */
#define SYNA_SPI_RD_CHUNK_SIZE		2048

/* Size of the message header */
#define SYNA_SPI_HEADER_SIZE		4

static int syna_tcm_spi_read(struct device *dev, void *buf, size_t len)
{
	return spi_read(to_spi_device(dev), buf, len);
}

static int syna_tcm_spi_write(struct device *dev, const void *buf, size_t len)
{
	return spi_write(to_spi_device(dev), buf, len);
}

static const struct syna_tcm_bus syna_tcm_spi_bus = {
	.read = syna_tcm_spi_read,
	.write = syna_tcm_spi_write,
	.bustype = BUS_SPI,
	.rd_chunk_size = SYNA_SPI_RD_CHUNK_SIZE,
	/* As the vendor driver: the header, then the rest in continued reads. */
	.first_read_size = SYNA_SPI_HEADER_SIZE,
};

/* Timings of the Pixel 11 (cubs) controller */
static const struct syna_tcm_chip_data syna_tcm_data = {
	.powerup_to_reset_ms = 200,
	.reset_to_ready_ms = 70,
};

static int syna_tcm_spi_probe(struct spi_device *spi)
{
	return syna_tcm_probe(&spi->dev, spi->irq, &syna_tcm_spi_bus,
			      spi_get_device_match_data(spi));
}

static const struct spi_device_id syna_tcm_spi_id[] = {
	{ "tcm", (kernel_ulong_t)&syna_tcm_data },
	{ }
};
MODULE_DEVICE_TABLE(spi, syna_tcm_spi_id);

static const struct of_device_id syna_tcm_spi_of_match[] = {
	{ .compatible = "syna,tcm", .data = &syna_tcm_data },
	{ }
};
MODULE_DEVICE_TABLE(of, syna_tcm_spi_of_match);

static struct spi_driver syna_tcm_spi_driver = {
	.driver = {
		.name = "synaptics-tcm-spi",
		.of_match_table = syna_tcm_spi_of_match,
		.pm = pm_sleep_ptr(&syna_tcm_pm_ops),
	},
	.probe = syna_tcm_spi_probe,
	.id_table = syna_tcm_spi_id,
};
module_spi_driver(syna_tcm_spi_driver);

MODULE_DESCRIPTION("Synaptics TouchComm SPI touchscreen driver");
MODULE_LICENSE("GPL");
