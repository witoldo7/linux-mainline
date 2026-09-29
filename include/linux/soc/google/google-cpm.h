/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Google Tensor Central Power Manager (CPM) interface
 *
 * Copyright 2023-2025 Google LLC
 */
#ifndef __LINUX_SOC_GOOGLE_CPM_H__
#define __LINUX_SOC_GOOGLE_CPM_H__

#include <linux/types.h>

struct device;
struct google_cpm;

/* Number of payload words in a CPM message, after the header word. */
#define GOOGLE_CPM_PAYLOAD_WORDS	3

/* CPM services */
#define GOOGLE_CPM_SVC_LPCM		0x08
#define GOOGLE_CPM_SVC_MBFS		0x14

/* AP-side services that the CPM sends notifications to */
#define GOOGLE_CPM_AP_SVC_POWER		0x07

typedef void (*google_cpm_notify_t)(void *data,
				    const u32 msg[GOOGLE_CPM_PAYLOAD_WORDS]);

struct google_cpm *google_cpm_get(struct device *dev);

int google_cpm_request(struct google_cpm *cpm, u8 service,
		       const u32 req[GOOGLE_CPM_PAYLOAD_WORDS],
		       u32 resp[GOOGLE_CPM_PAYLOAD_WORDS]);

int google_cpm_register_notifier(struct google_cpm *cpm, u8 ap_service,
				 google_cpm_notify_t fn, void *data);

int google_cpm_mbfs_get_handle(struct google_cpm *cpm, const char *path,
			       u32 *handle);
int google_cpm_mbfs_lookup(struct google_cpm *cpm, u32 folder,
			   const char *name, u32 *handle);
int google_cpm_mbfs_read(struct google_cpm *cpm, u32 handle, u64 *val);
int google_cpm_mbfs_write(struct google_cpm *cpm, u32 handle, u64 val);

#endif /* __LINUX_SOC_GOOGLE_CPM_H__ */
