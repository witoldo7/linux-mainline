// SPDX-License-Identifier: GPL-2.0-only
/*
 * Google Tensor watchdog timer
 *
 * A down counter clocked by the always-on oscillator. Writes to its
 * registers only take effect while the watchdog is unlocked with a key.
 * On expiry it either interrupts the AP, which traps to EL3 to collect CPU
 * state, or signals the GDMC, which resets the system.
 *
 * Based on the downstream Pixel driver.
 *
 * Copyright 2023-2025 Google LLC
 */

#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/cleanup.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/math64.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/spinlock.h>
#include <linux/watchdog.h>

#define WDT_ID			0x00
#define WDT_ID_MAGIC		0x574454	/* "WDT" */
#define WDT_CONTROL		0x08
#define WDT_CONTROL_EXPIRY_GDMC	BIT(3)
#define WDT_CONTROL_KEY_ENABLE	BIT(2)
#define WDT_CONTROL_ENABLE	BIT(0)
#define WDT_VALUE		0x0c
#define WDT_KEY			0x10

#define WDT_KEY_UNLOCK		0x00c0ffee
#define WDT_KEY_LOCK		0x00000000

#define WDT_DEFAULT_TIMEOUT	30

struct google_wdt {
	struct watchdog_device wdd;
	void __iomem *base;
	unsigned long rate;
	bool expire_to_gdmc;
	/* Serialises unlocked register sequences. */
	spinlock_t lock;
};

static u32 google_wdt_count(struct google_wdt *wdt)
{
	return min_t(u64, (u64)wdt->wdd.timeout * wdt->rate, U32_MAX);
}

static int google_wdt_ping(struct watchdog_device *wdd)
{
	struct google_wdt *wdt = watchdog_get_drvdata(wdd);

	guard(spinlock_irqsave)(&wdt->lock);

	writel(WDT_KEY_UNLOCK, wdt->base + WDT_KEY);
	writel(google_wdt_count(wdt), wdt->base + WDT_VALUE);
	writel(WDT_KEY_LOCK, wdt->base + WDT_KEY);

	return 0;
}

static int google_wdt_start(struct watchdog_device *wdd)
{
	struct google_wdt *wdt = watchdog_get_drvdata(wdd);
	u32 ctrl;

	guard(spinlock_irqsave)(&wdt->lock);

	writel(WDT_KEY_UNLOCK, wdt->base + WDT_KEY);
	writel(google_wdt_count(wdt), wdt->base + WDT_VALUE);

	ctrl = readl(wdt->base + WDT_CONTROL);
	ctrl &= ~WDT_CONTROL_EXPIRY_GDMC;
	if (wdt->expire_to_gdmc)
		ctrl |= WDT_CONTROL_EXPIRY_GDMC;
	ctrl |= WDT_CONTROL_ENABLE;
	writel(ctrl, wdt->base + WDT_CONTROL);

	writel(WDT_KEY_LOCK, wdt->base + WDT_KEY);

	return 0;
}

static int google_wdt_stop(struct watchdog_device *wdd)
{
	struct google_wdt *wdt = watchdog_get_drvdata(wdd);
	u32 ctrl;

	guard(spinlock_irqsave)(&wdt->lock);

	writel(WDT_KEY_UNLOCK, wdt->base + WDT_KEY);
	ctrl = readl(wdt->base + WDT_CONTROL);
	writel(ctrl & ~WDT_CONTROL_ENABLE, wdt->base + WDT_CONTROL);
	writel(WDT_KEY_LOCK, wdt->base + WDT_KEY);

	return 0;
}

static int google_wdt_set_timeout(struct watchdog_device *wdd,
				  unsigned int timeout)
{
	wdd->timeout = timeout;

	if (watchdog_hw_running(wdd))
		return google_wdt_ping(wdd);

	return 0;
}

static const struct watchdog_info google_wdt_info = {
	.identity	= "Google Tensor watchdog",
	.options	= WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING |
			  WDIOF_MAGICCLOSE,
};

static const struct watchdog_ops google_wdt_ops = {
	.owner		= THIS_MODULE,
	.start		= google_wdt_start,
	.stop		= google_wdt_stop,
	.ping		= google_wdt_ping,
	.set_timeout	= google_wdt_set_timeout,
};

static int google_wdt_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct google_wdt *wdt;
	struct clk *clk;
	int ret;

	wdt = devm_kzalloc(dev, sizeof(*wdt), GFP_KERNEL);
	if (!wdt)
		return -ENOMEM;

	spin_lock_init(&wdt->lock);

	wdt->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(wdt->base))
		return PTR_ERR(wdt->base);

	if (readl(wdt->base + WDT_ID) != WDT_ID_MAGIC)
		return dev_err_probe(dev, -ENODEV, "unexpected id %#x\n",
				     readl(wdt->base + WDT_ID));

	clk = devm_clk_get_enabled(dev, NULL);
	if (IS_ERR(clk))
		return dev_err_probe(dev, PTR_ERR(clk), "failed to get clock\n");

	wdt->rate = clk_get_rate(clk);
	if (wdt->rate < 1000)
		return dev_err_probe(dev, -EINVAL, "invalid clock rate %lu\n",
				     wdt->rate);

	wdt->expire_to_gdmc = device_property_read_bool(dev, "google,expire-to-gdmc");

	wdt->wdd.parent = dev;
	wdt->wdd.info = &google_wdt_info;
	wdt->wdd.ops = &google_wdt_ops;
	wdt->wdd.min_timeout = 1;
	wdt->wdd.max_hw_heartbeat_ms = div_u64((u64)U32_MAX * 1000, wdt->rate);
	wdt->wdd.timeout = WDT_DEFAULT_TIMEOUT;
	watchdog_init_timeout(&wdt->wdd, 0, dev);
	watchdog_set_drvdata(&wdt->wdd, wdt);
	watchdog_stop_on_unregister(&wdt->wdd);

	/*
	 * The bootloader may have left the watchdog running. Reload it with
	 * our timeout and let the watchdog core keep it fed until userspace
	 * takes over.
	 */
	if (readl(wdt->base + WDT_CONTROL) & WDT_CONTROL_ENABLE) {
		ret = google_wdt_start(&wdt->wdd);
		if (ret)
			return ret;
		set_bit(WDOG_HW_RUNNING, &wdt->wdd.status);
	}

	return devm_watchdog_register_device(dev, &wdt->wdd);
}

static const struct of_device_id google_wdt_of_match[] = {
	{ .compatible = "google,mbu-wdt" },
	{ }
};
MODULE_DEVICE_TABLE(of, google_wdt_of_match);

static struct platform_driver google_wdt_driver = {
	.probe = google_wdt_probe,
	.driver = {
		.name = "google-wdt",
		.of_match_table = google_wdt_of_match,
	},
};
module_platform_driver(google_wdt_driver);

MODULE_DESCRIPTION("Google Tensor watchdog driver");
MODULE_LICENSE("GPL");
