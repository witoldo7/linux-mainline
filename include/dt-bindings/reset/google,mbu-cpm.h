/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause) */
/*
 * Resets of the Google Tensor G6 (malibu) SoC, managed by the CPM.
 *
 * A reset is identified by its subsystem clock manager (LPCM), see
 * dt-bindings/clock/google,mbu-cpm.h, and its id within that LPCM. Both are
 * part of the CPM firmware interface.
 *
 * Copyright 2025 Google LLC
 */

#ifndef _DT_BINDINGS_RESET_GOOGLE_MBU_CPM_H
#define _DT_BINDINGS_RESET_GOOGLE_MBU_CPM_H

/* HSION */
#define MBU_HSION_USBC_STICKY_RST              0
#define MBU_HSION_USBC_NON_STICKY_RST          1
#define MBU_HSION_USB2_PHY_RESET               2
#define MBU_HSION_USB3DP_PHY_RESET             3
#define MBU_HSION_USBDP_U3PHY_FW               4
#define MBU_HSION_USBDP_U2PHY_APB              5
#define MBU_HSION_USBDP_USBDRD_BUS             6
#define MBU_HSION_USBDP_TOP                    7
#define MBU_HSION_USBDP_USB2AUX_PHY_APB        8
#define MBU_HSION_USBDP_USB2AUX_CTRL_BUS       9
#define MBU_HSION_USBDP_USB2AUX_CTRL_REF       10
#define MBU_HSION_USBDP_USB2AUX_SUSPEND        11
#define MBU_HSION_USB2AUX_CTRL_VAUX_STICKY     12
#define MBU_HSION_USB2AUX_CTRL_VCC_NON_STICKY  13
#define MBU_HSION_USB2AUX_PHY_RESET            14
#define MBU_HSION_DP_VCC                       15
#define MBU_HSION_VIDEO_0                      16
#define MBU_HSION_PRESETN0                     17
#define MBU_HSION_SRESETN0                     18
#define MBU_HSION_SDP_0                        19
#define MBU_HSION_HDCP22                       20
#define MBU_HSION_TRNG                         21
#define MBU_HSION_MAXI                         22
#define MBU_HSION_VIDEO_BUFF_RSTN              23
#define MBU_HSION_USBDP_SCLK0                  24
#define MBU_HSION_USBDP_AUX16M                 25
#define MBU_HSION_USBDP_PIXEL_0                26
#define MBU_HSION_DPTOP                        27
#define MBU_HSION_ESM                          28
#define MBU_HSION_EBU_RST                      29

/* HSIOS */
#define MBU_HSIOS_UFS_PHY_RESET                0

/* LSIO_S */
#define MBU_RST_LSIO_S_CG_PWM_CLK              0
#define MBU_RST_LSIO_S_CG_M0_CLK               1
#define MBU_RST_LSIO_S_CG_M1_CLK               2
#define MBU_RST_LSIO_S_CG_M2_CLK               3
#define MBU_RST_LSIO_S_CG_M3_CLK               4
#define MBU_RST_LSIO_S_CG_M4_CLK               5
#define MBU_RST_LSIO_S_CG_M5_CLK               6
#define MBU_RST_LSIO_S_CG_M6_CLK               7
#define MBU_RST_LSIO_S_CG_M7_CLK               8
#define MBU_RST_LSIO_S_CG_I2C_0_CLK            9
#define MBU_RST_LSIO_S_CG_I3C_0_CLK            10
#define MBU_RST_LSIO_S_CG_I2C_1_CLK            11
#define MBU_RST_LSIO_S_CG_I3C_1_CLK            12
#define MBU_RST_LSIO_S_CG_I2C_2_CLK            13
#define MBU_RST_LSIO_S_CG_I3C_2_CLK            14
#define MBU_RST_LSIO_S_CG_I2C_3_CLK            15
#define MBU_RST_LSIO_S_CG_I3C_3_CLK            16
#define MBU_RST_LSIO_S_CG_I2C_4_CLK            17
#define MBU_RST_LSIO_S_CG_I3C_4_CLK            18
#define MBU_RST_LSIO_S_CG_SPI_0_CLK            19
#define MBU_RST_LSIO_S_CG_UART_0_CLK           20
#define MBU_RST_LSIO_S_CG_SPI_1_CLK            21
#define MBU_RST_LSIO_S_CG_UART_1_CLK           22
#define MBU_RST_LSIO_S_CG_SPI_2_CLK            23
#define MBU_RST_LSIO_S_CG_UART_2_CLK           24
#define MBU_RST_LSIO_S_CG_SPI_3_CLK            25
#define MBU_RST_LSIO_S_CG_UART_3_CLK           26
#define MBU_RST_LSIO_S_CG_QSPI_CLK             27
#define MBU_RST_LSIO_S_CG_I2C_0_CFG_CLK        28
#define MBU_RST_LSIO_S_CG_I3C_0_CFG_CLK        29
#define MBU_RST_LSIO_S_CG_I2C_1_CFG_CLK        30
#define MBU_RST_LSIO_S_CG_I3C_1_CFG_CLK        31
#define MBU_RST_LSIO_S_CG_I2C_2_CFG_CLK        32
#define MBU_RST_LSIO_S_CG_I3C_2_CFG_CLK        33
#define MBU_RST_LSIO_S_CG_I2C_3_CFG_CLK        34
#define MBU_RST_LSIO_S_CG_I3C_3_CFG_CLK        35
#define MBU_RST_LSIO_S_CG_I2C_4_CFG_CLK        36
#define MBU_RST_LSIO_S_CG_I3C_4_CFG_CLK        37
#define MBU_RST_LSIO_S_CG_SPI_0_CFG_CLK        38
#define MBU_RST_LSIO_S_CG_UART_0_CFG_CLK       39
#define MBU_RST_LSIO_S_CG_SPI_1_CFG_CLK        40
#define MBU_RST_LSIO_S_CG_UART_1_CFG_CLK       41
#define MBU_RST_LSIO_S_CG_SPI_2_CFG_CLK        42
#define MBU_RST_LSIO_S_CG_UART_2_CFG_CLK       43
#define MBU_RST_LSIO_S_CG_SPI_3_CFG_CLK        44
#define MBU_RST_LSIO_S_CG_UART_3_CFG_CLK       45
#define MBU_RST_LSIO_S_CG_QSPI_CFG_CLK         46
#define MBU_RST_LSIO_S_CG_PWM_CFG_CLK          47
#define MBU_RST_LSIO_S_CG_GPIO_CLK             48
#define MBU_RST_LSIO_S_I2C_0_PERI_CFG_CLK      49
#define MBU_RST_LSIO_S_I3C_0_PERI_CFG_CLK      50
#define MBU_RST_LSIO_S_I2C_1_PERI_CFG_CLK      51
#define MBU_RST_LSIO_S_I3C_1_PERI_CFG_CLK      52
#define MBU_RST_LSIO_S_I2C_2_PERI_CFG_CLK      53
#define MBU_RST_LSIO_S_I3C_2_PERI_CFG_CLK      54
#define MBU_RST_LSIO_S_I2C_3_PERI_CFG_CLK      55
#define MBU_RST_LSIO_S_I3C_3_PERI_CFG_CLK      56
#define MBU_RST_LSIO_S_I2C_4_PERI_CFG_CLK      57
#define MBU_RST_LSIO_S_I3C_4_PERI_CFG_CLK      58
#define MBU_RST_LSIO_S_SPI_0_PERI_CFG_CLK      59
#define MBU_RST_LSIO_S_UART_0_PERI_CFG_CLK     60
#define MBU_RST_LSIO_S_SPI_1_PERI_CFG_CLK      61
#define MBU_RST_LSIO_S_UART_1_PERI_CFG_CLK     62
#define MBU_RST_LSIO_S_SPI_2_PERI_CFG_CLK      63
#define MBU_RST_LSIO_S_UART_2_PERI_CFG_CLK     64
#define MBU_RST_LSIO_S_SPI_3_PERI_CFG_CLK      65
#define MBU_RST_LSIO_S_UART_3_PERI_CFG_CLK     66
#define MBU_RST_LSIO_S_QSPI_PERI_CFG_CLK       67

/* LSIO_E */
#define MBU_RST_LSIO_E_CG_I2C_0_CLK            0
#define MBU_RST_LSIO_E_CG_I3C_0_CLK            1
#define MBU_RST_LSIO_E_CG_I2C_1_CLK            2
#define MBU_RST_LSIO_E_CG_I3C_1_CLK            3
#define MBU_RST_LSIO_E_CG_I2C_2_CLK            4
#define MBU_RST_LSIO_E_CG_I3C_2_CLK            5
#define MBU_RST_LSIO_E_CG_I2C_3_CLK            6
#define MBU_RST_LSIO_E_CG_I3C_3_CLK            7
#define MBU_RST_LSIO_E_CG_I2C_4_CLK            8
#define MBU_RST_LSIO_E_CG_I3C_4_CLK            9
#define MBU_RST_LSIO_E_CG_SPI_0_CLK            10
#define MBU_RST_LSIO_E_CG_UART_0_CLK           11
#define MBU_RST_LSIO_E_CG_SPI_1_CLK            12
#define MBU_RST_LSIO_E_CG_UART_1_CLK           13
#define MBU_RST_LSIO_E_CG_I2C_0_CFG_CLK        14
#define MBU_RST_LSIO_E_CG_I3C_0_CFG_CLK        15
#define MBU_RST_LSIO_E_CG_I2C_1_CFG_CLK        16
#define MBU_RST_LSIO_E_CG_I3C_1_CFG_CLK        17
#define MBU_RST_LSIO_E_CG_I2C_2_CFG_CLK        18
#define MBU_RST_LSIO_E_CG_I3C_2_CFG_CLK        19
#define MBU_RST_LSIO_E_CG_I2C_3_CFG_CLK        20
#define MBU_RST_LSIO_E_CG_I3C_3_CFG_CLK        21
#define MBU_RST_LSIO_E_CG_I2C_4_CFG_CLK        22
#define MBU_RST_LSIO_E_CG_I3C_4_CFG_CLK        23
#define MBU_RST_LSIO_E_CG_SPI_0_CFG_CLK        24
#define MBU_RST_LSIO_E_CG_UART_0_CFG_CLK       25
#define MBU_RST_LSIO_E_CG_SPI_1_CFG_CLK        26
#define MBU_RST_LSIO_E_CG_UART_1_CFG_CLK       27
#define MBU_RST_LSIO_E_CG_GPIO_CLK             28
#define MBU_RST_LSIO_E_I2C_0_PERI_CFG_CLK      29
#define MBU_RST_LSIO_E_I3C_0_PERI_CFG_CLK      30
#define MBU_RST_LSIO_E_I2C_1_PERI_CFG_CLK      31
#define MBU_RST_LSIO_E_I3C_1_PERI_CFG_CLK      32
#define MBU_RST_LSIO_E_I2C_2_PERI_CFG_CLK      33
#define MBU_RST_LSIO_E_I3C_2_PERI_CFG_CLK      34
#define MBU_RST_LSIO_E_I2C_3_PERI_CFG_CLK      35
#define MBU_RST_LSIO_E_I3C_3_PERI_CFG_CLK      36
#define MBU_RST_LSIO_E_I2C_4_PERI_CFG_CLK      37
#define MBU_RST_LSIO_E_I3C_4_PERI_CFG_CLK      38
#define MBU_RST_LSIO_E_SPI_0_PERI_CFG_CLK      39
#define MBU_RST_LSIO_E_UART_0_PERI_CFG_CLK     40
#define MBU_RST_LSIO_E_SPI_1_PERI_CFG_CLK      41
#define MBU_RST_LSIO_E_UART_1_PERI_CFG_CLK     42

/* AOSS_PG_AMB */
#define MBU_RST_AOSS_PG_AMB_CG_I2C0_APB_CLK    0
#define MBU_RST_AOSS_PG_AMB_CG_I3C0_APB_CLK    1
#define MBU_RST_AOSS_PG_AMB_CG_I3C1_APB_CLK    2
#define MBU_RST_AOSS_PG_AMB_CG_I3C2_APB_CLK    3
#define MBU_RST_AOSS_PG_AMB_CG_UART0_APB_CLK   4
#define MBU_RST_AOSS_PG_AMB_CG_UART1_APB_CLK   5
#define MBU_RST_AOSS_PG_AMB_CG_UART2_APB_CLK   6
#define MBU_RST_AOSS_PG_AMB_CG_UART3_APB_CLK   7
#define MBU_RST_AOSS_PG_AMB_CG_UART4_APB_CLK   8
#define MBU_RST_AOSS_PG_AMB_CG_UART5_APB_CLK   9
#define MBU_RST_AOSS_PG_AMB_CG_SPI0_APB_CLK    10
#define MBU_RST_AOSS_PG_AMB_CG_SPI1_APB_CLK    11
#define MBU_RST_AOSS_PG_AMB_CG_SPI2_APB_CLK    12
#define MBU_RST_AOSS_PG_AMB_CG_SPI3_APB_CLK    13
#define MBU_RST_AOSS_PG_AMB_CG_SPI4_APB_CLK    14
#define MBU_RST_AOSS_PG_AMB_CG_SPI5_APB_CLK    15
#define MBU_RST_AOSS_PG_AMB_CG_SPI6_APB_CLK    16
#define MBU_RST_AOSS_PG_AMB_CG_TSPI_APB_CLK    17
#define MBU_RST_AOSS_PG_AMB_CG_I2C0_PERI_CLK   18
#define MBU_RST_AOSS_PG_AMB_CG_I3C0_PERI_CLK   19
#define MBU_RST_AOSS_PG_AMB_CG_I3C1_PERI_CLK   20
#define MBU_RST_AOSS_PG_AMB_CG_I3C2_PERI_CLK   21
#define MBU_RST_AOSS_PG_AMB_CG_UART0_PERI_CLK  22
#define MBU_RST_AOSS_PG_AMB_CG_UART1_PERI_CLK  23
#define MBU_RST_AOSS_PG_AMB_CG_UART2_PERI_CLK  24
#define MBU_RST_AOSS_PG_AMB_CG_UART3_PERI_CLK  25
#define MBU_RST_AOSS_PG_AMB_CG_UART4_PERI_CLK  26
#define MBU_RST_AOSS_PG_AMB_CG_UART5_PERI_CLK  27
#define MBU_RST_AOSS_PG_AMB_CG_SPI0_PERI_CLK   28
#define MBU_RST_AOSS_PG_AMB_CG_SPI1_PERI_CLK   29
#define MBU_RST_AOSS_PG_AMB_CG_SPI2_PERI_CLK   30
#define MBU_RST_AOSS_PG_AMB_CG_SPI3_PERI_CLK   31
#define MBU_RST_AOSS_PG_AMB_CG_SPI4_PERI_CLK   32
#define MBU_RST_AOSS_PG_AMB_CG_SPI5_PERI_CLK   33
#define MBU_RST_AOSS_PG_AMB_CG_SPI6_PERI_CLK   34
#define MBU_RST_AOSS_PG_AMB_CG_TSPI_PERI_CLK   35
#define MBU_RST_AOSS_PG_AMB_I2C0_PERI_CFG_CLK  36
#define MBU_RST_AOSS_PG_AMB_I3C0_PERI_CFG_CLK  37
#define MBU_RST_AOSS_PG_AMB_I3C1_PERI_CFG_CLK  38
#define MBU_RST_AOSS_PG_AMB_I3C2_PERI_CFG_CLK  39
#define MBU_RST_AOSS_PG_AMB_UART0_PERI_CFG_CLK 40
#define MBU_RST_AOSS_PG_AMB_UART1_PERI_CFG_CLK 41
#define MBU_RST_AOSS_PG_AMB_UART2_PERI_CFG_CLK 42
#define MBU_RST_AOSS_PG_AMB_UART3_PERI_CFG_CLK 43
#define MBU_RST_AOSS_PG_AMB_UART4_PERI_CFG_CLK 44
#define MBU_RST_AOSS_PG_AMB_UART5_PERI_CFG_CLK 45
#define MBU_RST_AOSS_PG_AMB_SPI0_PERI_CFG_CLK  46
#define MBU_RST_AOSS_PG_AMB_SPI1_PERI_CFG_CLK  47
#define MBU_RST_AOSS_PG_AMB_SPI2_PERI_CFG_CLK  48
#define MBU_RST_AOSS_PG_AMB_SPI3_PERI_CFG_CLK  49
#define MBU_RST_AOSS_PG_AMB_SPI4_PERI_CFG_CLK  50
#define MBU_RST_AOSS_PG_AMB_SPI5_PERI_CFG_CLK  51
#define MBU_RST_AOSS_PG_AMB_SPI6_PERI_CFG_CLK  52
#define MBU_RST_AOSS_PG_AMB_TSPI_PERI_CFG_CLK  53

/* PCIE */
#define MBU_RST_PCIE_DPA_UART_CLK              0

/* CODEC_3P */
#define MBU_RST_CODEC_3P_CG_C3P_CORE_CLK       0
#define MBU_RST_CODEC_3P_CG_C3P_JPEG_CLK       1

#endif
