/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause) */
/*
 * Power domains of the Google Tensor G6 (malibu) SoC, managed by the CPM.
 *
 * Copyright 2025 Google LLC
 */

#ifndef _DT_BINDINGS_POWER_GOOGLE_MBU_POWER_H
#define _DT_BINDINGS_POWER_GOOGLE_MBU_POWER_H

#define MBU_PD_AURDSP             0
#define MBU_PD_AOSS_PG            1
#define MBU_PD_CODEC_3P           2
#define MBU_PD_CPUACC_GPDMA       3
#define MBU_PD_DPU                4
#define MBU_PD_DPU_BE             5
#define MBU_PD_DPU_FE0            6
#define MBU_PD_DPU_FE1            7
#define MBU_PD_DPU_DSI0           8
#define MBU_PD_DPU_DSI1           9
#define MBU_PD_DPU_DP0            10
#define MBU_PD_G2D                11
#define MBU_PD_G2D_CORE           12
#define MBU_PD_GCV                13
#define MBU_PD_GPU                14
#define MBU_PD_HSIO_N             15
#define MBU_PD_HSIO_N_USB         16
#define MBU_PD_HSIO_N_EBU         17
#define MBU_PD_HSIO_N_DP          18
#define MBU_PD_HSIO_N_USB2AUX     19
#define MBU_PD_HSIO_N_USB2AUX_PSW 20
#define MBU_PD_HSIO_S             21
#define MBU_PD_HSIO_S_UFS_PREP    22
#define MBU_PD_HSIO_S_UFS         23
#define MBU_PD_HSIO_S_SD          24
#define MBU_PD_ISPFE              25
#define MBU_PD_ISPFE_CORE0        26
#define MBU_PD_ISPFE_CORE1        27
#define MBU_PD_ISPFE_CORE2        28
#define MBU_PD_ISPFE_CSIS         29
#define MBU_PD_ISPBE              30
#define MBU_PD_LSIO_E             31
#define MBU_PD_LSIO_E_CLI_GPIO    32
#define MBU_PD_LSIO_S             33
#define MBU_PD_LSIO_S_CLI_GPIO    34
#define MBU_PD_PCIE               35
#define MBU_PD_PCIE_TOP           36
#define MBU_PD_PCIE_CTRL0         37
#define MBU_PD_PCIE_CTRL1         38
#define MBU_PD_TPU                39
#define MBU_PD_MEMSS_DTA          40

#endif
