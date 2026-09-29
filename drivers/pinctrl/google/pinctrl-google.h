/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Google Tensor pin controller
 *
 * Copyright 2022-2025 Google LLC
 */
#ifndef __PINCTRL_GOOGLE_H__
#define __PINCTRL_GOOGLE_H__

#include <linux/types.h>

struct pinctrl_pin_desc;
struct platform_device;

/* Maximum number of mux functions per pin (width of the DMUX register). */
#define GOOGLE_MAX_FUNCS	9

/* Marks an unused mux slot in the per-pin function tables. */
#define google_pinmux__		-1

enum google_pinctrl_reg {
	GOOGLE_PIN_PARAM,
	GOOGLE_PIN_DMUX,
	GOOGLE_PIN_TXDATA,
	GOOGLE_PIN_RXDATA,
	GOOGLE_PIN_ISR,
	GOOGLE_PIN_ISROVF,
	GOOGLE_PIN_IER,
	GOOGLE_PIN_IMR,
	GOOGLE_PIN_NR_REGS,
};

/**
 * struct google_pingroup - a single-pin group and its mux functions
 * @name: group name, identical to the pin name
 * @num: group index, identical to the pin number
 * @pins: the pin in this group
 * @npins: number of entries in @pins
 * @funcs: function index for each DMUX bit, or google_pinmux__ if unused
 * @nfuncs: number of entries in @funcs
 */
struct google_pingroup {
	const char *name;
	unsigned int num;
	const unsigned int *pins;
	unsigned int npins;
	const int *funcs;
	unsigned int nfuncs;
};

/**
 * struct google_pin_function - a pinmux function
 * @name: function name used in the device tree
 * @groups: groups that can select this function
 * @ngroups: number of entries in @groups
 */
struct google_pin_function {
	const char *name;
	const char * const *groups;
	unsigned int ngroups;
};

/**
 * struct google_pinctrl_regs - per-pin register layout of a SoC
 * @offsets: offset of each register inside a pin's register window
 * @pin_stride: size of each pin's register window
 */
struct google_pinctrl_regs {
	u8 offsets[GOOGLE_PIN_NR_REGS];
	u32 pin_stride;
};

/**
 * struct google_pinctrl_soc_data - one pin controller instance
 * @pins: pins of this controller
 * @npins: number of entries in @pins
 * @groups: groups of this controller, one per pin
 * @ngroups: number of entries in @groups
 * @funcs: mux functions of this controller
 * @nfuncs: number of entries in @funcs
 * @ngpios: number of pins exposed as GPIOs
 * @regs: register layout
 */
struct google_pinctrl_soc_data {
	const struct pinctrl_pin_desc *pins;
	unsigned int npins;
	const struct google_pingroup *groups;
	unsigned int ngroups;
	const struct google_pin_function *funcs;
	unsigned int nfuncs;
	unsigned int ngpios;
	const struct google_pinctrl_regs *regs;
};

#define GOOGLE_PINS(num)						\
	static const unsigned int google##num##_pins[] = { num }

#define PIN_GROUP(gnum, gname, f0, f1, f2, f3, f4, f5, f6, f7, f8)	\
	[gnum] = {							\
		.name = gname,						\
		.num = gnum,						\
		.pins = google##gnum##_pins,				\
		.npins = ARRAY_SIZE(google##gnum##_pins),		\
		.funcs = (const int[]){					\
			google_pinmux_##f0, google_pinmux_##f1,		\
			google_pinmux_##f2, google_pinmux_##f3,		\
			google_pinmux_##f4, google_pinmux_##f5,		\
			google_pinmux_##f6, google_pinmux_##f7,		\
			google_pinmux_##f8,				\
		},							\
		.nfuncs = GOOGLE_MAX_FUNCS,				\
	}

#define FUNCTION_GROUPS(func, ...)					\
	static const char * const func##_groups[] = { __VA_ARGS__ }

#define FUNCTION(func)							\
	[google_pinmux_##func] = {					\
		.name = #func,						\
		.groups = func##_groups,				\
		.ngroups = ARRAY_SIZE(func##_groups),			\
	}

int google_pinctrl_probe(struct platform_device *pdev);

#endif /* __PINCTRL_GOOGLE_H__ */
