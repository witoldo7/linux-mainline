// SPDX-License-Identifier: GPL-2.0-only
/*
 * Google Tensor pin controller and GPIO driver
 *
 * Every pin has its own register window with mux, pad configuration, data
 * and interrupt registers. Pins that can interrupt the CPU each have their
 * own line to a parent interrupt controller, usually a GIA.
 *
 * Based on the downstream Pixel driver.
 *
 * Copyright 2022-2025 Google LLC
 */

#include <linux/bitfield.h>
#include <linux/bitops.h>
#include <linux/cleanup.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio/driver.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pinctrl/pinconf-generic.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/property.h>
#include <linux/seq_file.h>
#include <linux/spinlock.h>

#include "../pinctrl-utils.h"
#include "pinctrl-google.h"

/* PARAM register */
#define PARAM_DRV		GENMASK(3, 0)
#define PARAM_IE		BIT(8)
#define PARAM_RXMODE		BIT(12)
#define PARAM_SLEW		BIT(16)

/* TXDATA register */
#define TXDATA_PUPD		GENMASK(1, 0)
#define TXDATA_PUPD_OPEN	0x0
#define TXDATA_PUPD_PD		0x1
#define TXDATA_PUPD_PU		0x2
#define TXDATA_DOUT		BIT(2)
#define TXDATA_OE		BIT(3)

/* RXDATA register */
#define RXDATA_PAD		BIT(0)
#define RXDATA_TRIG		GENMASK(3, 1)
#define RXDATA_TRIG_NONE	0x0
#define RXDATA_TRIG_RISING	0x1
#define RXDATA_TRIG_FALLING	0x2
#define RXDATA_TRIG_BOTH	0x3
#define RXDATA_TRIG_LOW		0x4
#define RXDATA_TRIG_HIGH	0x5

/* ISR, ISROVF, IER and IMR registers */
#define IXR_IRQ			BIT(0)

/* Time for a pad to settle after its pull resistors are released. */
#define PUPD_OPEN_SETTLE_US	10

/* Mux slot of the GPIO function. */
#define GPIO_FUNC_SLOT		0

struct google_pinctrl;

/**
 * struct google_gpio_parent - a parent interrupt of one pad
 * @gctl: owning pin controller
 * @pin: the pad whose interrupt this is
 */
struct google_gpio_parent {
	struct google_pinctrl *gctl;
	unsigned int pin;
};

struct google_pinctrl {
	struct device *dev;
	void __iomem *base;
	const struct google_pinctrl_soc_data *data;
	struct pinctrl_desc desc;
	struct pinctrl_dev *pctl;
	struct gpio_chip gc;
	raw_spinlock_t lock;

	struct google_gpio_parent *parents;
	unsigned int nparents;
};

static void __iomem *google_pin_reg(struct google_pinctrl *gctl,
				    unsigned int pin, enum google_pinctrl_reg reg)
{
	const struct google_pinctrl_regs *regs = gctl->data->regs;

	return gctl->base + pin * regs->pin_stride + regs->offsets[reg];
}

static u32 google_pin_read(struct google_pinctrl *gctl, unsigned int pin,
			   enum google_pinctrl_reg reg)
{
	return readl(google_pin_reg(gctl, pin, reg));
}

static void google_pin_write(struct google_pinctrl *gctl, unsigned int pin,
			     enum google_pinctrl_reg reg, u32 val)
{
	writel(val, google_pin_reg(gctl, pin, reg));
}

static void google_pin_rmw(struct google_pinctrl *gctl, unsigned int pin,
			   enum google_pinctrl_reg reg, u32 mask, u32 val)
{
	u32 tmp;

	guard(raw_spinlock_irqsave)(&gctl->lock);

	tmp = google_pin_read(gctl, pin, reg);
	tmp = (tmp & ~mask) | (val & mask);
	google_pin_write(gctl, pin, reg, tmp);
}

/* pinctrl_ops */

static int google_get_groups_count(struct pinctrl_dev *pctldev)
{
	struct google_pinctrl *gctl = pinctrl_dev_get_drvdata(pctldev);

	return gctl->data->ngroups;
}

static const char *google_get_group_name(struct pinctrl_dev *pctldev,
					 unsigned int selector)
{
	struct google_pinctrl *gctl = pinctrl_dev_get_drvdata(pctldev);

	return gctl->data->groups[selector].name;
}

static int google_get_group_pins(struct pinctrl_dev *pctldev,
				 unsigned int selector,
				 const unsigned int **pins,
				 unsigned int *npins)
{
	struct google_pinctrl *gctl = pinctrl_dev_get_drvdata(pctldev);

	*pins = gctl->data->groups[selector].pins;
	*npins = gctl->data->groups[selector].npins;

	return 0;
}

static const struct pinctrl_ops google_pinctrl_ops = {
	.get_groups_count	= google_get_groups_count,
	.get_group_name		= google_get_group_name,
	.get_group_pins		= google_get_group_pins,
	.dt_node_to_map		= pinconf_generic_dt_node_to_map_group,
	.dt_free_map		= pinctrl_utils_free_map,
};

/* pinmux_ops */

static int google_get_functions_count(struct pinctrl_dev *pctldev)
{
	struct google_pinctrl *gctl = pinctrl_dev_get_drvdata(pctldev);

	return gctl->data->nfuncs;
}

static const char *google_get_function_name(struct pinctrl_dev *pctldev,
					    unsigned int selector)
{
	struct google_pinctrl *gctl = pinctrl_dev_get_drvdata(pctldev);

	return gctl->data->funcs[selector].name;
}

static int google_get_function_groups(struct pinctrl_dev *pctldev,
				      unsigned int selector,
				      const char * const **groups,
				      unsigned int * const ngroups)
{
	struct google_pinctrl *gctl = pinctrl_dev_get_drvdata(pctldev);

	*groups = gctl->data->funcs[selector].groups;
	*ngroups = gctl->data->funcs[selector].ngroups;

	return 0;
}

static int google_set_mux_slot(struct google_pinctrl *gctl, unsigned int pin,
			       unsigned int slot)
{
	guard(raw_spinlock_irqsave)(&gctl->lock);

	/* DMUX is one-hot: each bit selects one mux slot. */
	google_pin_write(gctl, pin, GOOGLE_PIN_DMUX, BIT(slot));

	return 0;
}

static int google_set_mux(struct pinctrl_dev *pctldev, unsigned int func,
			  unsigned int group)
{
	struct google_pinctrl *gctl = pinctrl_dev_get_drvdata(pctldev);
	const struct google_pingroup *g = &gctl->data->groups[group];
	unsigned int slot;

	for (slot = 0; slot < g->nfuncs; slot++)
		if (g->funcs[slot] == func)
			return google_set_mux_slot(gctl, g->num, slot);

	return -EINVAL;
}

static int google_gpio_request_enable(struct pinctrl_dev *pctldev,
				      struct pinctrl_gpio_range *range,
				      unsigned int offset)
{
	struct google_pinctrl *gctl = pinctrl_dev_get_drvdata(pctldev);
	const struct google_pingroup *g = &gctl->data->groups[offset];

	if (g->funcs[GPIO_FUNC_SLOT] == google_pinmux__)
		return -EINVAL;

	return google_set_mux_slot(gctl, g->num, GPIO_FUNC_SLOT);
}

static const struct pinmux_ops google_pinmux_ops = {
	.get_functions_count	= google_get_functions_count,
	.get_function_name	= google_get_function_name,
	.get_function_groups	= google_get_function_groups,
	.set_mux		= google_set_mux,
	.gpio_request_enable	= google_gpio_request_enable,
};

/* pinconf_ops */

static int google_pinconf_group_get(struct pinctrl_dev *pctldev,
				    unsigned int group, unsigned long *config)
{
	struct google_pinctrl *gctl = pinctrl_dev_get_drvdata(pctldev);
	enum pin_config_param param = pinconf_to_config_param(*config);
	unsigned int pin = gctl->data->groups[group].num;
	u32 p, tx, arg;

	p = google_pin_read(gctl, pin, GOOGLE_PIN_PARAM);
	tx = google_pin_read(gctl, pin, GOOGLE_PIN_TXDATA);

	switch (param) {
	case PIN_CONFIG_BIAS_DISABLE:
		arg = FIELD_GET(TXDATA_PUPD, tx) == TXDATA_PUPD_OPEN;
		break;
	case PIN_CONFIG_BIAS_PULL_DOWN:
		arg = FIELD_GET(TXDATA_PUPD, tx) == TXDATA_PUPD_PD;
		break;
	case PIN_CONFIG_BIAS_PULL_UP:
		arg = FIELD_GET(TXDATA_PUPD, tx) == TXDATA_PUPD_PU;
		break;
	case PIN_CONFIG_DRIVE_STRENGTH:
		*config = pinconf_to_config_packed(param, FIELD_GET(PARAM_DRV, p));
		return 0;
	case PIN_CONFIG_SLEW_RATE:
		arg = FIELD_GET(PARAM_SLEW, p);
		break;
	case PIN_CONFIG_INPUT_ENABLE:
		arg = FIELD_GET(PARAM_IE, p);
		break;
	case PIN_CONFIG_INPUT_SCHMITT_ENABLE:
		arg = FIELD_GET(PARAM_RXMODE, p);
		break;
	case PIN_CONFIG_OUTPUT_ENABLE:
		arg = FIELD_GET(TXDATA_OE, tx);
		break;
	case PIN_CONFIG_LEVEL:
		if (!(tx & TXDATA_OE))
			return -EINVAL;
		*config = pinconf_to_config_packed(param, FIELD_GET(TXDATA_DOUT, tx));
		return 0;
	default:
		return -ENOTSUPP;
	}

	if (!arg)
		return -EINVAL;

	*config = pinconf_to_config_packed(param, arg);

	return 0;
}

static void google_pinconf_set_pull(struct google_pinctrl *gctl,
				    unsigned int pin, u32 pupd)
{
	/* Release the pad before switching between pull-up and pull-down. */
	google_pin_rmw(gctl, pin, GOOGLE_PIN_TXDATA, TXDATA_PUPD,
		       FIELD_PREP(TXDATA_PUPD, TXDATA_PUPD_OPEN));
	udelay(PUPD_OPEN_SETTLE_US);
	google_pin_rmw(gctl, pin, GOOGLE_PIN_TXDATA, TXDATA_PUPD,
		       FIELD_PREP(TXDATA_PUPD, pupd));
}

static int google_pinconf_group_set(struct pinctrl_dev *pctldev,
				    unsigned int group, unsigned long *configs,
				    unsigned int num_configs)
{
	struct google_pinctrl *gctl = pinctrl_dev_get_drvdata(pctldev);
	unsigned int pin = gctl->data->groups[group].num;
	unsigned int i;

	for (i = 0; i < num_configs; i++) {
		enum pin_config_param param = pinconf_to_config_param(configs[i]);
		u32 arg = pinconf_to_config_argument(configs[i]);

		switch (param) {
		case PIN_CONFIG_BIAS_DISABLE:
			google_pin_rmw(gctl, pin, GOOGLE_PIN_TXDATA, TXDATA_PUPD,
				       FIELD_PREP(TXDATA_PUPD, TXDATA_PUPD_OPEN));
			break;
		case PIN_CONFIG_BIAS_PULL_DOWN:
			google_pinconf_set_pull(gctl, pin, TXDATA_PUPD_PD);
			break;
		case PIN_CONFIG_BIAS_PULL_UP:
			google_pinconf_set_pull(gctl, pin, TXDATA_PUPD_PU);
			break;
		case PIN_CONFIG_DRIVE_STRENGTH:
			if (arg > FIELD_MAX(PARAM_DRV))
				return -EINVAL;
			google_pin_rmw(gctl, pin, GOOGLE_PIN_PARAM, PARAM_DRV,
				       FIELD_PREP(PARAM_DRV, arg));
			break;
		case PIN_CONFIG_SLEW_RATE:
			google_pin_rmw(gctl, pin, GOOGLE_PIN_PARAM, PARAM_SLEW,
				       arg ? PARAM_SLEW : 0);
			break;
		case PIN_CONFIG_INPUT_ENABLE:
			google_pin_rmw(gctl, pin, GOOGLE_PIN_PARAM, PARAM_IE,
				       arg ? PARAM_IE : 0);
			break;
		case PIN_CONFIG_INPUT_SCHMITT_ENABLE:
			google_pin_rmw(gctl, pin, GOOGLE_PIN_PARAM, PARAM_RXMODE,
				       arg ? PARAM_RXMODE : 0);
			break;
		case PIN_CONFIG_OUTPUT_ENABLE:
			google_pin_rmw(gctl, pin, GOOGLE_PIN_TXDATA, TXDATA_OE,
				       arg ? TXDATA_OE : 0);
			break;
		case PIN_CONFIG_LEVEL:
			google_pin_rmw(gctl, pin, GOOGLE_PIN_TXDATA,
				       TXDATA_OE | TXDATA_DOUT,
				       TXDATA_OE | (arg ? TXDATA_DOUT : 0));
			break;
		case PIN_CONFIG_PERSIST_STATE:
			break;
		default:
			return -ENOTSUPP;
		}
	}

	return 0;
}

static const struct pinconf_ops google_pinconf_ops = {
	.is_generic		= true,
	.pin_config_group_get	= google_pinconf_group_get,
	.pin_config_group_set	= google_pinconf_group_set,
};

/* gpio_chip */

static int google_gpio_get_direction(struct gpio_chip *gc, unsigned int offset)
{
	struct google_pinctrl *gctl = gpiochip_get_data(gc);

	if (google_pin_read(gctl, offset, GOOGLE_PIN_TXDATA) & TXDATA_OE)
		return GPIO_LINE_DIRECTION_OUT;

	return GPIO_LINE_DIRECTION_IN;
}

static int google_gpio_direction_input(struct gpio_chip *gc, unsigned int offset)
{
	struct google_pinctrl *gctl = gpiochip_get_data(gc);

	google_pin_rmw(gctl, offset, GOOGLE_PIN_PARAM, PARAM_IE, PARAM_IE);
	google_pin_rmw(gctl, offset, GOOGLE_PIN_TXDATA, TXDATA_OE, 0);

	return 0;
}

static int google_gpio_direction_output(struct gpio_chip *gc,
					unsigned int offset, int value)
{
	struct google_pinctrl *gctl = gpiochip_get_data(gc);

	google_pin_rmw(gctl, offset, GOOGLE_PIN_TXDATA, TXDATA_OE | TXDATA_DOUT,
		       TXDATA_OE | (value ? TXDATA_DOUT : 0));

	return 0;
}

static int google_gpio_get(struct gpio_chip *gc, unsigned int offset)
{
	struct google_pinctrl *gctl = gpiochip_get_data(gc);

	return !!(google_pin_read(gctl, offset, GOOGLE_PIN_RXDATA) & RXDATA_PAD);
}

static int google_gpio_set(struct gpio_chip *gc, unsigned int offset, int value)
{
	struct google_pinctrl *gctl = gpiochip_get_data(gc);

	google_pin_rmw(gctl, offset, GOOGLE_PIN_TXDATA, TXDATA_DOUT,
		       value ? TXDATA_DOUT : 0);

	return 0;
}

static int google_gpio_init_valid_mask(struct gpio_chip *gc,
				       unsigned long *valid_mask,
				       unsigned int ngpios)
{
	struct google_pinctrl *gctl = gpiochip_get_data(gc);
	unsigned int i;

	/* Some pads have no GPIO function at all. */
	for (i = 0; i < ngpios; i++)
		if (gctl->data->groups[i].funcs[GPIO_FUNC_SLOT] == google_pinmux__)
			clear_bit(i, valid_mask);

	return 0;
}

/* GPIO interrupts */

static bool google_gpio_irq_is_level(struct google_pinctrl *gctl,
				     unsigned int pin)
{
	u32 trig = FIELD_GET(RXDATA_TRIG,
			     google_pin_read(gctl, pin, GOOGLE_PIN_RXDATA));

	return trig == RXDATA_TRIG_LOW || trig == RXDATA_TRIG_HIGH;
}

static void google_gpio_irq_clear(struct google_pinctrl *gctl, unsigned int pin)
{
	google_pin_write(gctl, pin, GOOGLE_PIN_ISR, IXR_IRQ);
	google_pin_write(gctl, pin, GOOGLE_PIN_ISROVF, IXR_IRQ);
}

static void google_gpio_irq_mask(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct google_pinctrl *gctl = gpiochip_get_data(gc);
	irq_hw_number_t pin = irqd_to_hwirq(d);

	/*
	 * Only gate delivery, IER stays set so that an edge arriving while
	 * masked is still latched in ISR.
	 */
	scoped_guard(raw_spinlock_irqsave, &gctl->lock)
		google_pin_write(gctl, pin, GOOGLE_PIN_IMR, 0);

	gpiochip_disable_irq(gc, pin);
}

static void google_gpio_irq_unmask(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct google_pinctrl *gctl = gpiochip_get_data(gc);
	irq_hw_number_t pin = irqd_to_hwirq(d);

	gpiochip_enable_irq(gc, pin);

	guard(raw_spinlock_irqsave)(&gctl->lock);

	/* Drop a stale level event latched while the pad was masked. */
	if (google_gpio_irq_is_level(gctl, pin))
		google_gpio_irq_clear(gctl, pin);

	google_pin_write(gctl, pin, GOOGLE_PIN_IER, IXR_IRQ);
	google_pin_write(gctl, pin, GOOGLE_PIN_IMR, IXR_IRQ);
}

static void google_gpio_irq_ack(struct irq_data *d)
{
	/* The status is cleared by the parent handler. */
}

static int google_gpio_irq_set_type(struct irq_data *d, unsigned int type)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct google_pinctrl *gctl = gpiochip_get_data(gc);
	irq_hw_number_t pin = irqd_to_hwirq(d);
	u32 trig;

	switch (type & IRQ_TYPE_SENSE_MASK) {
	case IRQ_TYPE_EDGE_RISING:
		trig = RXDATA_TRIG_RISING;
		break;
	case IRQ_TYPE_EDGE_FALLING:
		trig = RXDATA_TRIG_FALLING;
		break;
	case IRQ_TYPE_EDGE_BOTH:
		trig = RXDATA_TRIG_BOTH;
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		trig = RXDATA_TRIG_HIGH;
		break;
	case IRQ_TYPE_LEVEL_LOW:
		trig = RXDATA_TRIG_LOW;
		break;
	default:
		return -EINVAL;
	}

	if (type & IRQ_TYPE_LEVEL_MASK)
		irq_set_handler_locked(d, handle_level_irq);
	else
		irq_set_handler_locked(d, handle_edge_irq);

	google_pin_rmw(gctl, pin, GOOGLE_PIN_RXDATA, RXDATA_TRIG,
		       FIELD_PREP(RXDATA_TRIG, trig));

	return 0;
}

static void google_gpio_irq_print_chip(struct irq_data *d, struct seq_file *p)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);

	seq_puts(p, dev_name(gc->parent));
}

static const struct irq_chip google_gpio_irq_chip = {
	.irq_ack		= google_gpio_irq_ack,
	.irq_mask		= google_gpio_irq_mask,
	.irq_unmask		= google_gpio_irq_unmask,
	.irq_set_type		= google_gpio_irq_set_type,
	.irq_print_chip		= google_gpio_irq_print_chip,
	.flags			= IRQCHIP_IMMUTABLE | IRQCHIP_SKIP_SET_WAKE,
	GPIOCHIP_IRQ_RESOURCE_HELPERS,
};

static void google_gpio_irq_handler(struct irq_desc *desc)
{
	struct google_gpio_parent *parent = irq_desc_get_handler_data(desc);
	struct irq_chip *chip = irq_desc_get_chip(desc);
	struct google_pinctrl *gctl = parent->gctl;
	unsigned int pin = parent->pin;

	chained_irq_enter(chip, desc);

	if (google_gpio_irq_is_level(gctl, pin)) {
		/* Let the consumer quiesce the source before clearing. */
		generic_handle_domain_irq(gctl->gc.irq.domain, pin);
		google_gpio_irq_clear(gctl, pin);
	} else {
		/* Clear first so that a new edge is not lost. */
		google_gpio_irq_clear(gctl, pin);
		generic_handle_domain_irq(gctl->gc.irq.domain, pin);
	}

	chained_irq_exit(chip, desc);
}

static void google_gpio_irq_init_valid_mask(struct gpio_chip *gc,
					    unsigned long *valid_mask,
					    unsigned int ngpios)
{
	struct google_pinctrl *gctl = gpiochip_get_data(gc);
	unsigned int i;

	/* Only pads with their own parent interrupt can interrupt the CPU. */
	bitmap_zero(valid_mask, ngpios);
	for (i = 0; i < gctl->nparents; i++)
		set_bit(gctl->parents[i].pin, valid_mask);
}

static int google_gpio_irq_init_hw(struct gpio_chip *gc)
{
	struct google_pinctrl *gctl = gpiochip_get_data(gc);
	unsigned int i, pin;

	/* Keep every interrupt quiet until a consumer asks for it. */
	for (i = 0; i < gctl->nparents; i++) {
		pin = gctl->parents[i].pin;
		google_pin_write(gctl, pin, GOOGLE_PIN_IMR, 0);
		google_pin_write(gctl, pin, GOOGLE_PIN_IER, 0);
		google_gpio_irq_clear(gctl, pin);
	}

	return 0;
}

static int google_gpio_parse_irqs(struct google_pinctrl *gctl,
				  struct gpio_irq_chip *girq)
{
	struct platform_device *pdev = to_platform_device(gctl->dev);
	unsigned int *parent_irqs;
	void **parent_data;
	u32 *pins;
	int i, n, ret;

	n = platform_irq_count(pdev);
	if (n <= 0)
		return n;

	gctl->parents = devm_kcalloc(gctl->dev, n, sizeof(*gctl->parents),
				     GFP_KERNEL);
	parent_irqs = devm_kcalloc(gctl->dev, n, sizeof(*parent_irqs), GFP_KERNEL);
	parent_data = devm_kcalloc(gctl->dev, n, sizeof(*parent_data), GFP_KERNEL);
	pins = devm_kcalloc(gctl->dev, n, sizeof(*pins), GFP_KERNEL);
	if (!gctl->parents || !parent_irqs || !parent_data || !pins)
		return -ENOMEM;

	ret = device_property_read_u32_array(gctl->dev, "google,irq-pins", pins, n);
	if (ret)
		return dev_err_probe(gctl->dev, ret,
				     "google,irq-pins must have one entry per interrupt\n");

	for (i = 0; i < n; i++) {
		if (pins[i] >= gctl->data->ngpios)
			return dev_err_probe(gctl->dev, -EINVAL,
					     "invalid interrupt pin %u\n", pins[i]);

		ret = platform_get_irq(pdev, i);
		if (ret < 0)
			return ret;

		parent_irqs[i] = ret;
		gctl->parents[i].gctl = gctl;
		gctl->parents[i].pin = pins[i];
		parent_data[i] = &gctl->parents[i];
	}

	gctl->nparents = n;

	gpio_irq_chip_set_chip(girq, &google_gpio_irq_chip);
	girq->parent_handler = google_gpio_irq_handler;
	girq->num_parents = n;
	girq->parents = parent_irqs;
	girq->per_parent_data = true;
	girq->parent_handler_data_array = parent_data;
	girq->default_type = IRQ_TYPE_NONE;
	girq->handler = handle_bad_irq;
	girq->init_valid_mask = google_gpio_irq_init_valid_mask;
	girq->init_hw = google_gpio_irq_init_hw;

	return 0;
}

static int google_gpio_register(struct google_pinctrl *gctl)
{
	struct gpio_chip *gc = &gctl->gc;
	int ret;

	gc->label = dev_name(gctl->dev);
	gc->parent = gctl->dev;
	gc->owner = THIS_MODULE;
	gc->base = -1;
	gc->ngpio = gctl->data->ngpios;
	gc->request = gpiochip_generic_request;
	gc->free = gpiochip_generic_free;
	gc->set_config = gpiochip_generic_config;
	gc->get_direction = google_gpio_get_direction;
	gc->direction_input = google_gpio_direction_input;
	gc->direction_output = google_gpio_direction_output;
	gc->get = google_gpio_get;
	gc->set = google_gpio_set;
	gc->init_valid_mask = google_gpio_init_valid_mask;

	ret = google_gpio_parse_irqs(gctl, &gc->irq);
	if (ret)
		return ret;

	return devm_gpiochip_add_data(gctl->dev, gc, gctl);
}

int google_pinctrl_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct google_pinctrl *gctl;
	struct clk_bulk_data *clks;
	int ret;

	gctl = devm_kzalloc(dev, sizeof(*gctl), GFP_KERNEL);
	if (!gctl)
		return -ENOMEM;

	gctl->dev = dev;
	gctl->data = device_get_match_data(dev);
	if (!gctl->data || gctl->data->ngpios > gctl->data->ngroups)
		return -EINVAL;

	raw_spin_lock_init(&gctl->lock);

	gctl->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(gctl->base))
		return PTR_ERR(gctl->base);

	ret = devm_clk_bulk_get_all_enabled(dev, &clks);
	if (ret < 0)
		return dev_err_probe(dev, ret, "failed to enable clocks\n");

	gctl->desc.name = dev_name(dev);
	gctl->desc.owner = THIS_MODULE;
	gctl->desc.pins = gctl->data->pins;
	gctl->desc.npins = gctl->data->npins;
	gctl->desc.pctlops = &google_pinctrl_ops;
	gctl->desc.pmxops = &google_pinmux_ops;
	gctl->desc.confops = &google_pinconf_ops;

	ret = devm_pinctrl_register_and_init(dev, &gctl->desc, gctl, &gctl->pctl);
	if (ret)
		return dev_err_probe(dev, ret, "failed to register pinctrl\n");

	ret = pinctrl_enable(gctl->pctl);
	if (ret)
		return dev_err_probe(dev, ret, "failed to enable pinctrl\n");

	ret = google_gpio_register(gctl);
	if (ret)
		return dev_err_probe(dev, ret, "failed to register gpiochip\n");

	return 0;
}
EXPORT_SYMBOL_GPL(google_pinctrl_probe);

MODULE_DESCRIPTION("Google Tensor pin controller driver");
MODULE_LICENSE("GPL");
