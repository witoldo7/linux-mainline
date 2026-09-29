/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright 2025 Google LLC
 */
#ifndef __MBU_DSI_H__
#define __MBU_DSI_H__

struct device_node;
struct drm_dsc_config;

const struct drm_dsc_config *mbu_dsi_get_dsc(struct device_node *np);

#endif /* __MBU_DSI_H__ */
