// SPDX-License-Identifier: GPL-2.0-only
/*
 * Google Tensor Generic Interrupt Aggregator (GIA)
 *
 * A GIA collects up to 32 interrupt lines from a subsystem and forwards
 * them to its parent interrupt controller as a single summary interrupt.
 * GIAs can be chained, e.g. a pulse GIA feeding a level GIA feeding the GIC.
 *
 * Based on the downstream Pixel driver.
 *
 * Copyright 2024-2025 Google LLC
 */

#include <linux/bitops.h>
#include <linux/cleanup.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/irqdomain.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/spinlock.h>

#define GIA_NR_IRQS	32

/**
 * struct gia_variant - register layout of a GIA flavour
 * @status: interrupt status register offset
 * @overflow: overflow status register offset, only valid if @pulse is set
 * @enable: interrupt enable register offset
 * @mask: interrupt mask register offset, a set bit means unmasked
 * @pulse: pulse (edge) aggregator. The status and overflow registers latch
 *	   incoming pulses and must be cleared by writing 1. Level aggregators
 *	   mirror the state of their input lines instead.
 */
struct gia_variant {
	u32 status;
	u32 overflow;
	u32 enable;
	u32 mask;
	bool pulse;
};

struct gia {
	void __iomem *base;
	const struct gia_variant *variant;
	struct irq_domain *domain;
	raw_spinlock_t lock;
	u32 unmasked;
};

static void gia_handle_status(struct gia *gia, u32 offset)
{
	unsigned long pending;
	unsigned int hwirq;

	scoped_guard(raw_spinlock, &gia->lock) {
		pending = readl_relaxed(gia->base + offset) & gia->unmasked;
		if (pending && gia->variant->pulse)
			writel_relaxed(pending, gia->base + offset);
	}

	for_each_set_bit(hwirq, &pending, GIA_NR_IRQS)
		generic_handle_domain_irq(gia->domain, hwirq);
}

static void gia_irq_handler(struct irq_desc *desc)
{
	struct gia *gia = irq_desc_get_handler_data(desc);
	struct irq_chip *chip = irq_desc_get_chip(desc);

	chained_irq_enter(chip, desc);

	gia_handle_status(gia, gia->variant->status);
	/* A pulse that arrives while its status bit is still set lands here. */
	if (gia->variant->pulse)
		gia_handle_status(gia, gia->variant->overflow);

	chained_irq_exit(chip, desc);
}

static void gia_update_mask(struct irq_data *d, bool unmask)
{
	struct gia *gia = irq_data_get_irq_chip_data(d);

	guard(raw_spinlock_irqsave)(&gia->lock);

	if (unmask)
		gia->unmasked |= BIT(d->hwirq);
	else
		gia->unmasked &= ~BIT(d->hwirq);

	writel_relaxed(gia->unmasked, gia->base + gia->variant->mask);
}

static void gia_irq_mask(struct irq_data *d)
{
	gia_update_mask(d, false);
}

static void gia_irq_unmask(struct irq_data *d)
{
	gia_update_mask(d, true);
}

static void gia_irq_ack(struct irq_data *d)
{
	/* Pulse status is cleared by the demultiplexing handler. */
}

static struct irq_chip gia_irq_chip = {
	.name		= "GIA",
	.irq_ack	= gia_irq_ack,
	.irq_mask	= gia_irq_mask,
	.irq_unmask	= gia_irq_unmask,
	/* Wakeup sources are configured by the power management firmware. */
	.flags		= IRQCHIP_SKIP_SET_WAKE,
};

static int gia_domain_map(struct irq_domain *domain, unsigned int virq,
			  irq_hw_number_t hwirq)
{
	struct gia *gia = domain->host_data;

	irq_set_chip_data(virq, gia);
	irq_set_chip_and_handler(virq, &gia_irq_chip,
				 gia->variant->pulse ? handle_edge_irq : handle_level_irq);

	return 0;
}

static const struct irq_domain_ops gia_domain_ops = {
	.map	= gia_domain_map,
	.xlate	= irq_domain_xlate_onecell,
};

static int gia_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct gia *gia;
	int parent_irq;

	gia = devm_kzalloc(dev, sizeof(*gia), GFP_KERNEL);
	if (!gia)
		return -ENOMEM;

	gia->variant = device_get_match_data(dev);
	raw_spin_lock_init(&gia->lock);

	gia->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(gia->base))
		return PTR_ERR(gia->base);

	parent_irq = platform_get_irq(pdev, 0);
	if (parent_irq < 0)
		return parent_irq;

	/* Mask everything, then let all lines through the input stage. */
	writel_relaxed(0, gia->base + gia->variant->mask);
	writel_relaxed(~0U, gia->base + gia->variant->enable);
	if (gia->variant->pulse) {
		writel_relaxed(~0U, gia->base + gia->variant->status);
		writel_relaxed(~0U, gia->base + gia->variant->overflow);
	}

	gia->domain = irq_domain_create_linear(dev_fwnode(dev), GIA_NR_IRQS,
					       &gia_domain_ops, gia);
	if (!gia->domain)
		return -ENOMEM;

	irq_set_chained_handler_and_data(parent_irq, gia_irq_handler, gia);

	return 0;
}

static const struct gia_variant gia_level = {
	.status	= 0x0,
	.enable	= 0x4,
	.mask	= 0x8,
};

/* Level aggregators on Tensor G6 have an extra register before enable. */
static const struct gia_variant gia_level_mbu = {
	.status	= 0x0,
	.enable	= 0x8,
	.mask	= 0xc,
};

static const struct gia_variant gia_pulse = {
	.status		= 0x0,
	.overflow	= 0x4,
	.enable		= 0x8,
	.mask		= 0xc,
	.pulse		= true,
};

static const struct of_device_id gia_of_match[] = {
	{ .compatible = "google,lga-level-gia", .data = &gia_level },
	{ .compatible = "google,lga-pulse-gia", .data = &gia_pulse },
	{ .compatible = "google,mbu-level-gia", .data = &gia_level_mbu },
	{ .compatible = "google,mbu-pulse-gia", .data = &gia_pulse },
	{ }
};

static struct platform_driver gia_driver = {
	.probe = gia_probe,
	.driver = {
		.name = "google-gia",
		.of_match_table = gia_of_match,
		.suppress_bind_attrs = true,
	},
};
builtin_platform_driver(gia_driver);
