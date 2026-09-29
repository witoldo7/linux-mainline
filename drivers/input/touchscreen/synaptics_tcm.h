/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Synaptics TouchComm (TCM) touchscreen driver, bus interface.
 */

#ifndef __SYNAPTICS_TCM_H__
#define __SYNAPTICS_TCM_H__

#include <linux/pm.h>
#include <linux/types.h>

struct device;

/**
 * struct syna_tcm_chip_data - Controller specific timings
 * @powerup_to_reset_ms: Delay between enabling the supplies and releasing reset
 * @reset_to_ready_ms: Delay between releasing reset and the first command
 */
struct syna_tcm_chip_data {
	unsigned int powerup_to_reset_ms;
	unsigned int reset_to_ready_ms;
};

/**
 * struct syna_tcm_bus - Transport of the TouchComm packets
 * @read: Read exactly @len bytes of the message stream
 * @write: Write one command packet of @len bytes
 * @bustype: Input bus type reported to userspace
 * @rd_chunk_size: Largest read the controller accepts on this bus
 * @first_read_size: Bytes read for the start of a message. The controller
 *	streams the rest of the message with continued-read packets.
 */
struct syna_tcm_bus {
	int (*read)(struct device *dev, void *buf, size_t len);
	int (*write)(struct device *dev, const void *buf, size_t len);
	u16 bustype;
	unsigned int rd_chunk_size;
	unsigned int first_read_size;
};

int syna_tcm_probe(struct device *dev, int irq, const struct syna_tcm_bus *bus,
		   const struct syna_tcm_chip_data *chip);

extern const struct dev_pm_ops syna_tcm_pm_ops;

#endif /* __SYNAPTICS_TCM_H__ */
