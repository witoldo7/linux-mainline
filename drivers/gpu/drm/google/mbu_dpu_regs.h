/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Registers of the VeriSilicon DC9400 display controller in Google Tensor G6
 * (malibu) used by the mbu_dpu driver: layer 0, display 0 and output 0 only.
 *
 * Generated from the downstream Pixel register headers. Offsets are relative
 * to the controller's "fe-be" register window. Field masks are indented by
 * two spaces and field values by four.
 *
 * Copyright (C) 2023 VeriSilicon Holdings Co., Ltd.
 * Copyright 2025 Google LLC
 */

#ifndef __MBU_DPU_REGS_H__
#define __MBU_DPU_REGS_H__

#include <linux/bits.h>

#define DCREG_SH_LAYER0_ADDRESS                                         0x01802c
#define   DCREG_SH_LAYER0_ADDRESS_ADDRESS                               GENMASK(31, 0)

#define DCREG_SH_LAYER0_HIGH_ADDRESS                                    0x018030
#define   DCREG_SH_LAYER0_HIGH_ADDRESS_ADDRESS                          GENMASK(7, 0)

#define DCREG_SH_LAYER0_STRIDE                                          0x018044
#define   DCREG_SH_LAYER0_STRIDE_STRIDE                                 GENMASK(17, 0)

#define DCREG_SH_LAYER0_SIZE                                            0x01805c
#define   DCREG_SH_LAYER0_SIZE_WIDTH                                    GENMASK(15, 0)
#define   DCREG_SH_LAYER0_SIZE_HEIGHT                                   GENMASK(31, 16)

#define DCREG_SH_LAYER0_CONFIG                                          0x018088
#define   DCREG_SH_LAYER0_CONFIG_ENABLE                                 GENMASK(0, 0)
#define     DCREG_SH_LAYER0_CONFIG_ENABLE_DISABLE                       0x0
#define     DCREG_SH_LAYER0_CONFIG_ENABLE_ENABLE                        0x1
#define   DCREG_SH_LAYER0_CONFIG_CLEAR                                  GENMASK(1, 1)
#define     DCREG_SH_LAYER0_CONFIG_CLEAR_DISABLED                       0x0
#define     DCREG_SH_LAYER0_CONFIG_CLEAR_ENABLED                        0x1
#define   DCREG_SH_LAYER0_CONFIG_LUT3D                                  GENMASK(2, 2)
#define     DCREG_SH_LAYER0_CONFIG_LUT3D_DISABLED                       0x0
#define     DCREG_SH_LAYER0_CONFIG_LUT3D_ENABLED                        0x1
#define   DCREG_SH_LAYER0_CONFIG_BYPASS_HDR                             GENMASK(3, 3)
#define     DCREG_SH_LAYER0_CONFIG_BYPASS_HDR_DISABLED                  0x0
#define     DCREG_SH_LAYER0_CONFIG_BYPASS_HDR_ENABLED                   0x1
#define   DCREG_SH_LAYER0_CONFIG_EOTF                                   GENMASK(4, 4)
#define     DCREG_SH_LAYER0_CONFIG_EOTF_DISABLED                        0x0
#define     DCREG_SH_LAYER0_CONFIG_EOTF_ENABLED                         0x1
#define   DCREG_SH_LAYER0_CONFIG_OETF                                   GENMASK(5, 5)
#define     DCREG_SH_LAYER0_CONFIG_OETF_DISABLED                        0x0
#define     DCREG_SH_LAYER0_CONFIG_OETF_ENABLED                         0x1
#define   DCREG_SH_LAYER0_CONFIG_GAMUT_MAPPING                          GENMASK(6, 6)
#define     DCREG_SH_LAYER0_CONFIG_GAMUT_MAPPING_DISABLED               0x0
#define     DCREG_SH_LAYER0_CONFIG_GAMUT_MAPPING_ENABLED                0x1
#define   DCREG_SH_LAYER0_CONFIG_TONE_MAPPING                           GENMASK(7, 7)
#define     DCREG_SH_LAYER0_CONFIG_TONE_MAPPING_DISABLED                0x0
#define     DCREG_SH_LAYER0_CONFIG_TONE_MAPPING_ENABLED                 0x1
#define   DCREG_SH_LAYER0_CONFIG_Y2R                                    GENMASK(8, 8)
#define     DCREG_SH_LAYER0_CONFIG_Y2R_DISABLED                         0x0
#define     DCREG_SH_LAYER0_CONFIG_Y2R_ENABLED                          0x1
#define   DCREG_SH_LAYER0_CONFIG_DE_MULTIPLY                            GENMASK(9, 9)
#define     DCREG_SH_LAYER0_CONFIG_DE_MULTIPLY_DISABLED                 0x0
#define     DCREG_SH_LAYER0_CONFIG_DE_MULTIPLY_ENABLED                  0x1
#define   DCREG_SH_LAYER0_CONFIG_UV_SWIZZLE                             GENMASK(11, 11)
#define     DCREG_SH_LAYER0_CONFIG_UV_SWIZZLE_UV                        0x0
#define     DCREG_SH_LAYER0_CONFIG_UV_SWIZZLE_VU                        0x1
#define   DCREG_SH_LAYER0_CONFIG_SWIZZLE                                GENMASK(13, 12)
#define     DCREG_SH_LAYER0_CONFIG_SWIZZLE_ARGB                         0x0
#define     DCREG_SH_LAYER0_CONFIG_SWIZZLE_RGBA                         0x1
#define     DCREG_SH_LAYER0_CONFIG_SWIZZLE_ABGR                         0x2
#define     DCREG_SH_LAYER0_CONFIG_SWIZZLE_BGRA                         0x3
#define   DCREG_SH_LAYER0_CONFIG_SCALE                                  GENMASK(14, 14)
#define     DCREG_SH_LAYER0_CONFIG_SCALE_DISABLED                       0x0
#define     DCREG_SH_LAYER0_CONFIG_SCALE_ENABLED                        0x1
#define   DCREG_SH_LAYER0_CONFIG_TILE_MODE                              GENMASK(19, 16)
#define     DCREG_SH_LAYER0_CONFIG_TILE_MODE_LINEAR                     0x0
#define     DCREG_SH_LAYER0_CONFIG_TILE_MODE_TILED32X2                  0x1
#define     DCREG_SH_LAYER0_CONFIG_TILE_MODE_TILED16X4                  0x2
#define     DCREG_SH_LAYER0_CONFIG_TILE_MODE_TILED32X4                  0x3
#define     DCREG_SH_LAYER0_CONFIG_TILE_MODE_TILED32X8                  0x4
#define     DCREG_SH_LAYER0_CONFIG_TILE_MODE_TILED16X8                  0x5
#define     DCREG_SH_LAYER0_CONFIG_TILE_MODE_TILED8X8                   0x6
#define     DCREG_SH_LAYER0_CONFIG_TILE_MODE_TILED16X16                 0x7
#define     DCREG_SH_LAYER0_CONFIG_TILE_MODE_TILED32X8_A                0x8
#define     DCREG_SH_LAYER0_CONFIG_TILE_MODE_TILED8X8__UNIT2X2          0x9
#define     DCREG_SH_LAYER0_CONFIG_TILE_MODE_TILED8X4__UNIT2X2          0xa
#define   DCREG_SH_LAYER0_CONFIG_ROT_ANGLE                              GENMASK(22, 20)
#define     DCREG_SH_LAYER0_CONFIG_ROT_ANGLE_ROT0                       0x0
#define     DCREG_SH_LAYER0_CONFIG_ROT_ANGLE_ROT90                      0x1
#define     DCREG_SH_LAYER0_CONFIG_ROT_ANGLE_ROT180                     0x2
#define     DCREG_SH_LAYER0_CONFIG_ROT_ANGLE_ROT270                     0x3
#define     DCREG_SH_LAYER0_CONFIG_ROT_ANGLE_FLIP_X                     0x4
#define     DCREG_SH_LAYER0_CONFIG_ROT_ANGLE_FLIP_Y                     0x5
#define     DCREG_SH_LAYER0_CONFIG_ROT_ANGLE_FLIPX_ROT90                0x6
#define     DCREG_SH_LAYER0_CONFIG_ROT_ANGLE_FLIPY_ROT90                0x7
#define   DCREG_SH_LAYER0_CONFIG_EXTEND_BITS_ALPHA_MODE                 GENMASK(23, 23)
#define     DCREG_SH_LAYER0_CONFIG_EXTEND_BITS_ALPHA_MODE_DISABLED      0x0
#define     DCREG_SH_LAYER0_CONFIG_EXTEND_BITS_ALPHA_MODE_ENABLED       0x1
#define   DCREG_SH_LAYER0_CONFIG_FORMAT                                 GENMASK(29, 24)
#define     DCREG_SH_LAYER0_CONFIG_FORMAT_A8R8G8B8                      0x00
#define     DCREG_SH_LAYER0_CONFIG_FORMAT_X8R8G8B8                      0x01
#define     DCREG_SH_LAYER0_CONFIG_FORMAT_A2R10G10B10                   0x02
#define     DCREG_SH_LAYER0_CONFIG_FORMAT_X2R10G10B10                   0x03
#define     DCREG_SH_LAYER0_CONFIG_FORMAT_R8G8B8                        0x04
#define     DCREG_SH_LAYER0_CONFIG_FORMAT_R5G6B5                        0x05
#define     DCREG_SH_LAYER0_CONFIG_FORMAT_A1R5G5B5                      0x06
#define     DCREG_SH_LAYER0_CONFIG_FORMAT_X1R5G5B5                      0x07
#define     DCREG_SH_LAYER0_CONFIG_FORMAT_A4R4G4B4                      0x08
#define     DCREG_SH_LAYER0_CONFIG_FORMAT_X4R4G4B4                      0x09
#define     DCREG_SH_LAYER0_CONFIG_FORMAT_FP16                          0x0a
#define     DCREG_SH_LAYER0_CONFIG_FORMAT_YUY2                          0x0b
#define     DCREG_SH_LAYER0_CONFIG_FORMAT_UYVY                          0x0c
#define     DCREG_SH_LAYER0_CONFIG_FORMAT_YV12                          0x0d
#define     DCREG_SH_LAYER0_CONFIG_FORMAT_NV12                          0x0e
#define     DCREG_SH_LAYER0_CONFIG_FORMAT_NV16                          0x0f
#define     DCREG_SH_LAYER0_CONFIG_FORMAT_P010                          0x10
#define     DCREG_SH_LAYER0_CONFIG_FORMAT_P210                          0x11
#define     DCREG_SH_LAYER0_CONFIG_FORMAT_YUV420_PACKED_10BIT           0x12
#define     DCREG_SH_LAYER0_CONFIG_FORMAT_YUY2_10BIT                    0x14
#define     DCREG_SH_LAYER0_CONFIG_FORMAT_UYVY_10BIT                    0x15
#define   DCREG_SH_LAYER0_CONFIG_EXTEND_BITS_MODE                       GENMASK(31, 30)
#define     DCREG_SH_LAYER0_CONFIG_EXTEND_BITS_MODE_MODE0               0x0
#define     DCREG_SH_LAYER0_CONFIG_EXTEND_BITS_MODE_MODE1               0x1
#define     DCREG_SH_LAYER0_CONFIG_EXTEND_BITS_MODE_MODE2               0x2
#define   DCREG_SH_LAYER0_CONFIG_EX_CROP_ENABLE                         GENMASK(0, 0)
#define     DCREG_SH_LAYER0_CONFIG_EX_CROP_ENABLE_DISABLE               0x0
#define     DCREG_SH_LAYER0_CONFIG_EX_CROP_ENABLE_ENABLE                0x1
#define   DCREG_SH_LAYER0_CONFIG_EX_EXTEND_BITS_UV_MODE                 GENMASK(1, 1)
#define     DCREG_SH_LAYER0_CONFIG_EX_EXTEND_BITS_UV_MODE_MODE0         0x0
#define     DCREG_SH_LAYER0_CONFIG_EX_EXTEND_BITS_UV_MODE_MODE1         0x1
#define   DCREG_SH_LAYER0_CONFIG_EX_LINE_PADDING                        GENMASK(2, 2)
#define     DCREG_SH_LAYER0_CONFIG_EX_LINE_PADDING_DISABLED             0x0
#define     DCREG_SH_LAYER0_CONFIG_EX_LINE_PADDING_ENABLED              0x1
#define   DCREG_SH_LAYER0_CONFIG_EX_QUANT_MODE                          GENMASK(3, 3)
#define     DCREG_SH_LAYER0_CONFIG_EX_QUANT_MODE_MODE0                  0x0
#define     DCREG_SH_LAYER0_CONFIG_EX_QUANT_MODE_MODE1                  0x1

#define DCREG_SH_LAYER0_OT_NUMBER                                       0x018020
#define   DCREG_SH_LAYER0_OT_NUMBER_VALUE                               GENMASK(6, 0)

#define DCREG_SH_LAYER0_BLEND_STACK_ID                                  0x0e7000
#define   DCREG_SH_LAYER0_BLEND_STACK_ID_EX_ID0                         GENMASK(4, 0)
#define   DCREG_SH_LAYER0_BLEND_STACK_ID_EX_ID1                         GENMASK(12, 8)
#define   DCREG_SH_LAYER0_BLEND_STACK_ID_ID0                            GENMASK(4, 0)
#define   DCREG_SH_LAYER0_BLEND_STACK_ID_ID1                            GENMASK(12, 8)

#define DCREG_SH_LAYER0_OUT_ROI_ORIGIN                                  0x01829c
#define   DCREG_SH_LAYER0_OUT_ROI_ORIGIN_X                              GENMASK(15, 0)
#define   DCREG_SH_LAYER0_OUT_ROI_ORIGIN_Y                              GENMASK(31, 16)
#define   DCREG_SH_LAYER0_OUT_ROI_ORIGIN_EX_X                           GENMASK(15, 0)
#define   DCREG_SH_LAYER0_OUT_ROI_ORIGIN_EX_Y                           GENMASK(31, 16)
#define   DCREG_SH_LAYER0_OUT_ROI_ORIGIN_EX1_X                          GENMASK(15, 0)
#define   DCREG_SH_LAYER0_OUT_ROI_ORIGIN_EX1_Y                          GENMASK(31, 16)
#define   DCREG_SH_LAYER0_OUT_ROI_ORIGIN_EX2_X                          GENMASK(15, 0)
#define   DCREG_SH_LAYER0_OUT_ROI_ORIGIN_EX2_Y                          GENMASK(31, 16)

#define DCREG_SH_LAYER0_OUT_ROI_SIZE                                    0x0182a0
#define   DCREG_SH_LAYER0_OUT_ROI_SIZE_WIDTH                            GENMASK(15, 0)
#define   DCREG_SH_LAYER0_OUT_ROI_SIZE_HEIGHT                           GENMASK(31, 16)
#define   DCREG_SH_LAYER0_OUT_ROI_SIZE_EX_WIDTH                         GENMASK(15, 0)
#define   DCREG_SH_LAYER0_OUT_ROI_SIZE_EX_HEIGHT                        GENMASK(31, 16)
#define   DCREG_SH_LAYER0_OUT_ROI_SIZE_EX1_WIDTH                        GENMASK(15, 0)
#define   DCREG_SH_LAYER0_OUT_ROI_SIZE_EX1_HEIGHT                       GENMASK(31, 16)
#define   DCREG_SH_LAYER0_OUT_ROI_SIZE_EX2_WIDTH                        GENMASK(15, 0)
#define   DCREG_SH_LAYER0_OUT_ROI_SIZE_EX2_HEIGHT                       GENMASK(31, 16)

#define DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG                              0x088000
#define   DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_EX_ALPHA_BLEND             GENMASK(0, 0)
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_EX_ALPHA_BLEND_DISABLED  0x0
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_EX_ALPHA_BLEND_ENABLED   0x1
#define   DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_EX_ALPHA_BLEND_FAST        GENMASK(1, 1)
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_EX_ALPHA_BLEND_FAST_DISABLED 0x0
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_EX_ALPHA_BLEND_FAST_ENABLED 0x1
#define   DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_EX_ALPHA_BLEND_FAST_MODE   GENMASK(7, 4)
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_EX_ALPHA_BLEND_FAST_MODE_CLEAR 0x0
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_EX_ALPHA_BLEND_FAST_MODE_SRC 0x1
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_EX_ALPHA_BLEND_FAST_MODE_DST 0x2
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_EX_ALPHA_BLEND_FAST_MODE_SRC_OVER 0x3
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_EX_ALPHA_BLEND_FAST_MODE_DST_OVER 0x4
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_EX_ALPHA_BLEND_FAST_MODE_SRC_IN 0x5
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_EX_ALPHA_BLEND_FAST_MODE_DST_IN 0x6
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_EX_ALPHA_BLEND_FAST_MODE_SRC_OUT 0x7
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_EX_ALPHA_BLEND_FAST_MODE_DST_OUT 0x8
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_EX_ALPHA_BLEND_FAST_MODE_SRC_ATOP 0x9
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_EX_ALPHA_BLEND_FAST_MODE_DST_ATOP 0xa
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_EX_ALPHA_BLEND_FAST_MODE_XOR 0xb
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_EX_ALPHA_BLEND_FAST_MODE_PLUS 0xc
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_EX_ALPHA_BLEND_FAST_MODE_BLEND 0xd
#define   DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_ALPHA_MODE             GENMASK(0, 0)
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_ALPHA_MODE_NORMAL    0x0
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_ALPHA_MODE_INVERSED  0x1
#define   DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_GLOBAL_ALPHA_MODE      GENMASK(2, 1)
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_GLOBAL_ALPHA_MODE_NORMAL 0x0
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_GLOBAL_ALPHA_MODE_GLOBAL 0x1
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_GLOBAL_ALPHA_MODE_SCALED 0x2
#define   DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_FACTOR_MODE            GENMASK(3, 3)
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_FACTOR_MODE_NORMAL   0x0
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_FACTOR_MODE_DEST     0x1
#define   DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_COLOR_BLEND_MODE       GENMASK(6, 4)
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_COLOR_BLEND_MODE_ZERO 0x0
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_COLOR_BLEND_MODE_ONE 0x1
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_COLOR_BLEND_MODE_NORMAL 0x2
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_COLOR_BLEND_MODE_INVERSED 0x3
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_COLOR_BLEND_MODE_FACTOR_MODE 0x4
#define   DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_ALPHA_FACTOR_MODE      GENMASK(7, 7)
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_ALPHA_FACTOR_MODE_NORMAL 0x0
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_ALPHA_FACTOR_MODE_DEST 0x1
#define   DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_ALPHA_BLEND_MODE       GENMASK(10, 8)
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_ALPHA_BLEND_MODE_ZERO 0x0
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_ALPHA_BLEND_MODE_ONE 0x1
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_ALPHA_BLEND_MODE_NORMAL 0x2
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_ALPHA_BLEND_MODE_INVERSED 0x3
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_SRC_ALPHA_BLEND_MODE_FACTOR_MODE 0x4
#define   DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND                GENMASK(13, 13)
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND_DISABLED     0x0
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND_ENABLED      0x1
#define   DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_ALPHA_MODE             GENMASK(16, 16)
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_ALPHA_MODE_NORMAL    0x0
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_ALPHA_MODE_INVERSED  0x1
#define   DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_GLOBAL_ALPHA_MODE      GENMASK(18, 17)
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_GLOBAL_ALPHA_MODE_NORMAL 0x0
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_GLOBAL_ALPHA_MODE_GLOBAL 0x1
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_GLOBAL_ALPHA_MODE_SCALED 0x2
#define   DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_FACTOR_MODE            GENMASK(19, 19)
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_FACTOR_MODE_NORMAL   0x0
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_FACTOR_MODE_DEST     0x1
#define   DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_COLOR_BLEND_MODE       GENMASK(22, 20)
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_COLOR_BLEND_MODE_ZERO 0x0
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_COLOR_BLEND_MODE_ONE 0x1
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_COLOR_BLEND_MODE_NORMAL 0x2
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_COLOR_BLEND_MODE_INVERSED 0x3
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_COLOR_BLEND_MODE_FACTOR_MODE 0x4
#define   DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_ALPHA_FACTOR_MODE      GENMASK(23, 23)
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_ALPHA_FACTOR_MODE_NORMAL 0x0
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_ALPHA_FACTOR_MODE_DEST 0x1
#define   DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_ALPHA_BLEND_MODE       GENMASK(26, 24)
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_ALPHA_BLEND_MODE_ZERO 0x0
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_ALPHA_BLEND_MODE_ONE 0x1
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_ALPHA_BLEND_MODE_NORMAL 0x2
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_ALPHA_BLEND_MODE_INVERSED 0x3
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_DST_ALPHA_BLEND_MODE_FACTOR_MODE 0x4
#define   DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND_FAST           GENMASK(27, 27)
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND_FAST_DISABLED 0x0
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND_FAST_ENABLED 0x1
#define   DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND_FAST_MODE      GENMASK(31, 28)
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND_FAST_MODE_CLEAR 0x0
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND_FAST_MODE_SRC 0x1
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND_FAST_MODE_DST 0x2
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND_FAST_MODE_SRC_OVER 0x3
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND_FAST_MODE_DST_OVER 0x4
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND_FAST_MODE_SRC_IN 0x5
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND_FAST_MODE_DST_IN 0x6
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND_FAST_MODE_SRC_OUT 0x7
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND_FAST_MODE_DST_OUT 0x8
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND_FAST_MODE_SRC_ATOP 0x9
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND_FAST_MODE_DST_ATOP 0xa
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND_FAST_MODE_XOR 0xb
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND_FAST_MODE_PLUS 0xc
#define     DCREG_SH_LAYER0_ALPHA_BLEND_CONFIG_ALPHA_BLEND_FAST_MODE_BLEND 0xd

#define DCREG_SH_LAYER0_DMA_SRAM_SIZE                                   0x018290
#define   DCREG_SH_LAYER0_DMA_SRAM_SIZE_VALUE                           GENMASK(7, 0)
#define     DCREG_SH_LAYER0_DMA_SRAM_SIZE_VALUE_KBYTE64                 0x00
#define     DCREG_SH_LAYER0_DMA_SRAM_SIZE_VALUE_KBYTE128                0x01
#define     DCREG_SH_LAYER0_DMA_SRAM_SIZE_VALUE_KBYTE192                0x02
#define     DCREG_SH_LAYER0_DMA_SRAM_SIZE_VALUE_KBYTE256                0x03
#define     DCREG_SH_LAYER0_DMA_SRAM_SIZE_VALUE_KBYTE320                0x04
#define     DCREG_SH_LAYER0_DMA_SRAM_SIZE_VALUE_KBYTE384                0x05
#define   DCREG_SH_LAYER0_DMA_SRAM_SIZE_VALUE_EX                        GENMASK(15, 8)
#define     DCREG_SH_LAYER0_DMA_SRAM_SIZE_VALUE_EX_KBYTE32              0x00
#define     DCREG_SH_LAYER0_DMA_SRAM_SIZE_VALUE_EX_KBYTE64              0x01
#define     DCREG_SH_LAYER0_DMA_SRAM_SIZE_VALUE_EX_KBYTE96              0x02
#define     DCREG_SH_LAYER0_DMA_SRAM_SIZE_VALUE_EX_KBYTE128             0x03
#define     DCREG_SH_LAYER0_DMA_SRAM_SIZE_VALUE_EX_KBYTE160             0x04
#define     DCREG_SH_LAYER0_DMA_SRAM_SIZE_VALUE_EX_KBYTE192             0x05

#define DCREG_SH_LAYER0_SCALER_SRAM_SIZE                                0x018288
#define   DCREG_SH_LAYER0_SCALER_SRAM_SIZE_VALUE                        GENMASK(7, 0)
#define     DCREG_SH_LAYER0_SCALER_SRAM_SIZE_VALUE_KBYTE36              0x00
#define     DCREG_SH_LAYER0_SCALER_SRAM_SIZE_VALUE_KBYTE72              0x01
#define     DCREG_SH_LAYER0_SCALER_SRAM_SIZE_VALUE_KBYTE108             0x02
#define     DCREG_SH_LAYER0_SCALER_SRAM_SIZE_VALUE_KBYTE144             0x03
#define     DCREG_SH_LAYER0_SCALER_SRAM_SIZE_VALUE_KBYTE180             0x04

#define DCREG_LAYER0_OUTPUT_PATH_ID                                     0x0d7000
#define   DCREG_LAYER0_OUTPUT_PATH_ID_ID                                GENMASK(2, 0)
#define     DCREG_LAYER0_OUTPUT_PATH_ID_ID_PANEL0                       0x0
#define     DCREG_LAYER0_OUTPUT_PATH_ID_ID_PANEL1                       0x1
#define     DCREG_LAYER0_OUTPUT_PATH_ID_ID_PANEL2                       0x2
#define     DCREG_LAYER0_OUTPUT_PATH_ID_ID_PANEL3                       0x3
#define     DCREG_LAYER0_OUTPUT_PATH_ID_ID_WRITE_BACK                   0x4

#define DCREG_LAYER0_CONFIG                                             0x018000
#define   DCREG_LAYER0_CONFIG_REG_SWITCH                                GENMASK(0, 0)
#define     DCREG_LAYER0_CONFIG_REG_SWITCH_WAIT                         0x0
#define     DCREG_LAYER0_CONFIG_REG_SWITCH_OK                           0x1
#define   DCREG_LAYER0_CONFIG_RESET                                     GENMASK(1, 1)
#define     DCREG_LAYER0_CONFIG_RESET_RESET                             0x1
#define   DCREG_LAYER0_CONFIG_FORCE_FLIP                                GENMASK(2, 2)
#define   DCREG_LAYER0_CONFIG_AXI_BURST_LEN_SEL                         GENMASK(3, 3)
#define     DCREG_LAYER0_CONFIG_AXI_BURST_LEN_SEL_BURST8                0x0
#define     DCREG_LAYER0_CONFIG_AXI_BURST_LEN_SEL_BURST4                0x1

#define DCREG_SH_PANEL0_FORMAT                                          0x08c028
#define   DCREG_SH_PANEL0_FORMAT_OUTPUT_FORMAT                          GENMASK(5, 0)
#define     DCREG_SH_PANEL0_FORMAT_OUTPUT_FORMAT_RGB888                 0x00
#define     DCREG_SH_PANEL0_FORMAT_OUTPUT_FORMAT_RGB101010              0x01
#define     DCREG_SH_PANEL0_FORMAT_OUTPUT_FORMAT_RGB666                 0x02
#define     DCREG_SH_PANEL0_FORMAT_OUTPUT_FORMAT_YUV422_8BIT            0x03
#define     DCREG_SH_PANEL0_FORMAT_OUTPUT_FORMAT_YUV444_8BIT            0x04
#define     DCREG_SH_PANEL0_FORMAT_OUTPUT_FORMAT_YUV422_10BIT           0x05
#define     DCREG_SH_PANEL0_FORMAT_OUTPUT_FORMAT_YUV444_10BIT           0x06
#define     DCREG_SH_PANEL0_FORMAT_OUTPUT_FORMAT_RGB121212              0x07

#define DCREG_SH_PANEL0_WIDTH                                           0x0909cc
#define   DCREG_SH_PANEL0_WIDTH_VALUE                                   GENMASK(15, 0)

#define DCREG_SH_PANEL0_HEIGHT                                          0x0909d0
#define   DCREG_SH_PANEL0_HEIGHT_VALUE                                  GENMASK(15, 0)

#define DCREG_SH_PANEL0_IMAGE_WIDTH                                     0x088294
#define   DCREG_SH_PANEL0_IMAGE_WIDTH_VALUE                             GENMASK(15, 0)

#define DCREG_SH_PANEL0_IMAGE_HEIGHT                                    0x088298
#define   DCREG_SH_PANEL0_IMAGE_HEIGHT_VALUE                            GENMASK(15, 0)

#define DCREG_SH_PANEL0_DITHER_WIDTH                                    0x08c044
#define   DCREG_SH_PANEL0_DITHER_WIDTH_VALUE                            GENMASK(15, 0)

#define DCREG_SH_PANEL0_DITHER_HEIGHT                                   0x08c048
#define   DCREG_SH_PANEL0_DITHER_HEIGHT_VALUE                           GENMASK(15, 0)

#define DCREG_SH_PANEL0_SPLIT_CONFIG                                    0x0909bc
#define   DCREG_SH_PANEL0_SPLIT_CONFIG_ENABLE                           GENMASK(0, 0)
#define     DCREG_SH_PANEL0_SPLIT_CONFIG_ENABLE_DISABLED                0x0
#define     DCREG_SH_PANEL0_SPLIT_CONFIG_ENABLE_ENABLED                 0x1
#define   DCREG_SH_PANEL0_SPLIT_CONFIG_SOURCE                           GENMASK(2, 1)
#define     DCREG_SH_PANEL0_SPLIT_CONFIG_SOURCE_PIPE                    0x0
#define     DCREG_SH_PANEL0_SPLIT_CONFIG_SOURCE_DSC                     0x1
#define     DCREG_SH_PANEL0_SPLIT_CONFIG_SOURCE_VDC                     0x2

#define DCREG_PANEL0_CONFIG                                             0x08c000
#define   DCREG_PANEL0_CONFIG_REG_SWITCH                                GENMASK(0, 0)
#define     DCREG_PANEL0_CONFIG_REG_SWITCH_WAIT                         0x0
#define     DCREG_PANEL0_CONFIG_REG_SWITCH_OK                           0x1
#define   DCREG_PANEL0_CONFIG_RESET                                     GENMASK(1, 1)
#define     DCREG_PANEL0_CONFIG_RESET_RESET                             0x1
#define   DCREG_PANEL0_CONFIG_FORCE_FLIP                                GENMASK(2, 2)

#define DCREG_SH_PANEL0_CONFIG                                          0x08c020
#define   DCREG_SH_PANEL0_CONFIG_LTM                                    GENMASK(0, 0)
#define     DCREG_SH_PANEL0_CONFIG_LTM_DISABLED                         0x0
#define     DCREG_SH_PANEL0_CONFIG_LTM_ENABLED                          0x1
#define   DCREG_SH_PANEL0_CONFIG_SHARPNESS                              GENMASK(1, 1)
#define     DCREG_SH_PANEL0_CONFIG_SHARPNESS_DISABLED                   0x0
#define     DCREG_SH_PANEL0_CONFIG_SHARPNESS_ENABLED                    0x1
#define   DCREG_SH_PANEL0_CONFIG_MATRIX                                 GENMASK(2, 2)
#define     DCREG_SH_PANEL0_CONFIG_MATRIX_DISABLED                      0x0
#define     DCREG_SH_PANEL0_CONFIG_MATRIX_ENABLED                       0x1
#define   DCREG_SH_PANEL0_CONFIG_DE_GAMMA                               GENMASK(3, 3)
#define     DCREG_SH_PANEL0_CONFIG_DE_GAMMA_DISABLED                    0x0
#define     DCREG_SH_PANEL0_CONFIG_DE_GAMMA_ENABLED                     0x1
#define   DCREG_SH_PANEL0_CONFIG_BRIGHTNESS                             GENMASK(4, 4)
#define     DCREG_SH_PANEL0_CONFIG_BRIGHTNESS_DISABLED                  0x0
#define     DCREG_SH_PANEL0_CONFIG_BRIGHTNESS_ENABLED                   0x1
#define   DCREG_SH_PANEL0_CONFIG_GAMUT_MATRIX                           GENMASK(5, 5)
#define     DCREG_SH_PANEL0_CONFIG_GAMUT_MATRIX_DISABLED                0x0
#define     DCREG_SH_PANEL0_CONFIG_GAMUT_MATRIX_ENABLED                 0x1
#define   DCREG_SH_PANEL0_CONFIG_LUT3D                                  GENMASK(6, 6)
#define     DCREG_SH_PANEL0_CONFIG_LUT3D_DISABLED                       0x0
#define     DCREG_SH_PANEL0_CONFIG_LUT3D_ENABLED                        0x1
#define   DCREG_SH_PANEL0_CONFIG_LUT3D_EX                               GENMASK(7, 7)
#define     DCREG_SH_PANEL0_CONFIG_LUT3D_EX_DISABLED                    0x0
#define     DCREG_SH_PANEL0_CONFIG_LUT3D_EX_ENABLED                     0x1
#define   DCREG_SH_PANEL0_CONFIG_LUT3D_EX1                              GENMASK(8, 8)
#define     DCREG_SH_PANEL0_CONFIG_LUT3D_EX1_DISABLED                   0x0
#define     DCREG_SH_PANEL0_CONFIG_LUT3D_EX1_ENABLED                    0x1
#define   DCREG_SH_PANEL0_CONFIG_CCROI_PRIORITY                         GENMASK(9, 9)
#define   DCREG_SH_PANEL0_CONFIG_GAMMA                                  GENMASK(10, 10)
#define     DCREG_SH_PANEL0_CONFIG_GAMMA_DISABLED                       0x0
#define     DCREG_SH_PANEL0_CONFIG_GAMMA_ENABLED                        0x1
#define   DCREG_SH_PANEL0_CONFIG_GAMMA_EX                               GENMASK(11, 11)
#define     DCREG_SH_PANEL0_CONFIG_GAMMA_EX_DISABLED                    0x0
#define     DCREG_SH_PANEL0_CONFIG_GAMMA_EX_ENABLED                     0x1
#define   DCREG_SH_PANEL0_CONFIG_GAMMA_EX1                              GENMASK(12, 12)
#define     DCREG_SH_PANEL0_CONFIG_GAMMA_EX1_DISABLED                   0x0
#define     DCREG_SH_PANEL0_CONFIG_GAMMA_EX1_ENABLED                    0x1
#define   DCREG_SH_PANEL0_CONFIG_GAMMA_DITHER                           GENMASK(13, 13)
#define     DCREG_SH_PANEL0_CONFIG_GAMMA_DITHER_DISABLED                0x0
#define     DCREG_SH_PANEL0_CONFIG_GAMMA_DITHER_ENABLED                 0x1
#define   DCREG_SH_PANEL0_CONFIG_BLUR                                   GENMASK(14, 14)
#define     DCREG_SH_PANEL0_CONFIG_BLUR_DISABLED                        0x0
#define     DCREG_SH_PANEL0_CONFIG_BLUR_ENABLED                         0x1
#define   DCREG_SH_PANEL0_CONFIG_DITHER                                 GENMASK(15, 15)
#define     DCREG_SH_PANEL0_CONFIG_DITHER_DISABLED                      0x0
#define     DCREG_SH_PANEL0_CONFIG_DITHER_ENABLED                       0x1
#define   DCREG_SH_PANEL0_CONFIG_DITHER_MODE                            GENMASK(16, 16)
#define     DCREG_SH_PANEL0_CONFIG_DITHER_MODE_BIT8                     0x0
#define     DCREG_SH_PANEL0_CONFIG_DITHER_MODE_BIT6                     0x1
#define   DCREG_SH_PANEL0_CONFIG_LOW_LEVEL_DITHER                       GENMASK(17, 17)
#define     DCREG_SH_PANEL0_CONFIG_LOW_LEVEL_DITHER_DISABLED            0x0
#define     DCREG_SH_PANEL0_CONFIG_LOW_LEVEL_DITHER_ENABLED             0x1
#define   DCREG_SH_PANEL0_CONFIG_RCD                                    GENMASK(18, 18)
#define     DCREG_SH_PANEL0_CONFIG_RCD_DISABLED                         0x0
#define     DCREG_SH_PANEL0_CONFIG_RCD_ENABLED                          0x1
#define   DCREG_SH_PANEL0_CONFIG_RCD_INVERSE                            GENMASK(19, 19)
#define     DCREG_SH_PANEL0_CONFIG_RCD_INVERSE_DISABLED                 0x0
#define     DCREG_SH_PANEL0_CONFIG_RCD_INVERSE_ENABLED                  0x1
#define   DCREG_SH_PANEL0_CONFIG_R2Y_MODE                               GENMASK(22, 20)
#define     DCREG_SH_PANEL0_CONFIG_R2Y_MODE_PROGRAMMABLE                0x0
#define     DCREG_SH_PANEL0_CONFIG_R2Y_MODE_LIMIT_RGB_2_LIMIT_YUV       0x1
#define     DCREG_SH_PANEL0_CONFIG_R2Y_MODE_LIMIT_RGB_2_FULL_YUV        0x2
#define     DCREG_SH_PANEL0_CONFIG_R2Y_MODE_FULL_RGB_2_LIMIT_YUV        0x3
#define     DCREG_SH_PANEL0_CONFIG_R2Y_MODE_FULL_RGB_2_FULL_YUV         0x4
#define   DCREG_SH_PANEL0_CONFIG_R2Y                                    GENMASK(23, 23)
#define     DCREG_SH_PANEL0_CONFIG_R2Y_DISABLED                         0x0
#define     DCREG_SH_PANEL0_CONFIG_R2Y_ENABLED                          0x1
#define   DCREG_SH_PANEL0_CONFIG_EOTF                                   GENMASK(24, 24)
#define     DCREG_SH_PANEL0_CONFIG_EOTF_DISABLED                        0x0
#define     DCREG_SH_PANEL0_CONFIG_EOTF_ENABLED                         0x1
#define   DCREG_SH_PANEL0_CONFIG_OETF                                   GENMASK(25, 25)
#define     DCREG_SH_PANEL0_CONFIG_OETF_DISABLED                        0x0
#define     DCREG_SH_PANEL0_CONFIG_OETF_ENABLED                         0x1
#define   DCREG_SH_PANEL0_CONFIG_EXTEND_BITS_MODE                       GENMASK(26, 26)
#define     DCREG_SH_PANEL0_CONFIG_EXTEND_BITS_MODE_MODE0               0x0
#define     DCREG_SH_PANEL0_CONFIG_EXTEND_BITS_MODE_MODE1               0x1
#define   DCREG_SH_PANEL0_CONFIG_R2Y_GAMUT                              GENMASK(30, 27)
#define     DCREG_SH_PANEL0_CONFIG_R2Y_GAMUT_BT601                      0x0
#define     DCREG_SH_PANEL0_CONFIG_R2Y_GAMUT_BT709                      0x1
#define     DCREG_SH_PANEL0_CONFIG_R2Y_GAMUT_BT2020                     0x2
#define     DCREG_SH_PANEL0_CONFIG_R2Y_GAMUT_P3                         0x3
#define     DCREG_SH_PANEL0_CONFIG_R2Y_GAMUT_SRGB                       0x4
#define   DCREG_SH_PANEL0_CONFIG_OUTPUT_PATH                            GENMASK(31, 31)
#define     DCREG_SH_PANEL0_CONFIG_OUTPUT_PATH_DISABLED                 0x0
#define     DCREG_SH_PANEL0_CONFIG_OUTPUT_PATH_ENABLED                  0x1
#define   DCREG_SH_PANEL0_CONFIG_EX_DITHER_MODE                         GENMASK(1, 0)
#define     DCREG_SH_PANEL0_CONFIG_EX_DITHER_MODE_BIT8                  0x0
#define     DCREG_SH_PANEL0_CONFIG_EX_DITHER_MODE_BIT10                 0x1
#define     DCREG_SH_PANEL0_CONFIG_EX_DITHER_MODE_BIT6                  0x2
#define   DCREG_SH_PANEL0_CONFIG_EX_CC_ROI0                             GENMASK(2, 2)
#define     DCREG_SH_PANEL0_CONFIG_EX_CC_ROI0_DISABLED                  0x0
#define     DCREG_SH_PANEL0_CONFIG_EX_CC_ROI0_ENABLED                   0x1
#define   DCREG_SH_PANEL0_CONFIG_EX_CC_ROI1                             GENMASK(3, 3)
#define     DCREG_SH_PANEL0_CONFIG_EX_CC_ROI1_DISABLED                  0x0
#define     DCREG_SH_PANEL0_CONFIG_EX_CC_ROI1_ENABLED                   0x1

#define DCREG_PANEL0_DSC                                                0x0f9000
#define   DCREG_PANEL0_DSC_MODE_STAT_ACTIVE_HS_NUM                      GENMASK(3, 0)
#define   DCREG_PANEL0_DSC_MODE_STAT_SS_NUM                             GENMASK(5, 4)
#define   DCREG_PANEL0_DSC_MODE_STAT_SPLIT_PANEL                        GENMASK(6, 6)
#define   DCREG_PANEL0_DSC_MODE_STAT_MULTIPLEX_MODE                     GENMASK(7, 7)
#define   DCREG_PANEL0_DSC_MODE_STAT_DE_RASTER                          GENMASK(8, 8)
#define   DCREG_PANEL0_DSC_ADDRESS                                      GENMASK(31, 0)
#define   DCREG_PANEL0_DSC_END_ADDRESS                                  GENMASK(31, 0)

#define DCREG_SH_OUTPUT0_CLK_EN                                         0x0b60b4
#define   DCREG_SH_OUTPUT0_CLK_EN_VALUE                                 GENMASK(0, 0)
#define     DCREG_SH_OUTPUT0_CLK_EN_VALUE_DISABLED                      0x0
#define     DCREG_SH_OUTPUT0_CLK_EN_VALUE_ENABLED                       0x1

#define DCREG_SH_OUTPUT0_IPI_FORMAT                                     0x0b6060
#define   DCREG_SH_OUTPUT0_IPI_FORMAT_VALUE                             GENMASK(4, 0)
#define     DCREG_SH_OUTPUT0_IPI_FORMAT_VALUE_RGB                       0x00
#define     DCREG_SH_OUTPUT0_IPI_FORMAT_VALUE_YCBCR422                  0x01
#define     DCREG_SH_OUTPUT0_IPI_FORMAT_VALUE_YCBCR444                  0x02
#define     DCREG_SH_OUTPUT0_IPI_FORMAT_VALUE_YCBCR420                  0x03
#define     DCREG_SH_OUTPUT0_IPI_FORMAT_VALUE_YCBCR422_LOOSELY          0x04
#define     DCREG_SH_OUTPUT0_IPI_FORMAT_VALUE_RGB_LOOSELY               0x05
#define     DCREG_SH_OUTPUT0_IPI_FORMAT_VALUE_RAW                       0x06
#define     DCREG_SH_OUTPUT0_IPI_FORMAT_VALUE_COMPRESS_DATA             0x0b
#define     DCREG_SH_OUTPUT0_IPI_FORMAT_VALUE_YONLY                     0x19

#define DCREG_SH_OUTPUT0_IPI_COLOR_DEPTH                                0x0b6064
#define   DCREG_SH_OUTPUT0_IPI_COLOR_DEPTH_VALUE                        GENMASK(4, 0)
#define     DCREG_SH_OUTPUT0_IPI_COLOR_DEPTH_VALUE_BITS565              0x02
#define     DCREG_SH_OUTPUT0_IPI_COLOR_DEPTH_VALUE_BITS6                0x03
#define     DCREG_SH_OUTPUT0_IPI_COLOR_DEPTH_VALUE_BITS8                0x05
#define     DCREG_SH_OUTPUT0_IPI_COLOR_DEPTH_VALUE_BITS10               0x06
#define     DCREG_SH_OUTPUT0_IPI_COLOR_DEPTH_VALUE_BITS12               0x07
#define     DCREG_SH_OUTPUT0_IPI_COLOR_DEPTH_VALUE_BITS16               0x09
#define     DCREG_SH_OUTPUT0_IPI_COLOR_DEPTH_VALUE_BITS48               0x0e

#define DCREG_OUTPUT0_TIMING_HSYNC                                      0x0b601c
#define   DCREG_OUTPUT0_TIMING_HSYNC_POLARITY                           GENMASK(0, 0)

#define DCREG_OUTPUT0_TIMING_VSYNC                                      0x0b6030
#define   DCREG_OUTPUT0_TIMING_VSYNC_POLARITY                           GENMASK(0, 0)

#define DCREG_SH_OUTPUT0_TIMING_HS_WIDTH                                0x0b602c
#define   DCREG_SH_OUTPUT0_TIMING_HS_WIDTH_VALUE                        GENMASK(15, 0)

#define DCREG_SH_OUTPUT0_TIMING_HBP_WIDTH                               0x0b6028
#define   DCREG_SH_OUTPUT0_TIMING_HBP_WIDTH_VALUE                       GENMASK(15, 0)

#define DCREG_SH_OUTPUT0_TIMING_HA_WIDTH                                0x0b6024
#define   DCREG_SH_OUTPUT0_TIMING_HA_WIDTH_VALUE                        GENMASK(15, 0)

#define DCREG_SH_OUTPUT0_TIMING_HFP_WIDTH                               0x0b6020
#define   DCREG_SH_OUTPUT0_TIMING_HFP_WIDTH_VALUE                       GENMASK(15, 0)

#define DCREG_SH_OUTPUT0_TIMING_VS_HEIGHT                               0x0b6040
#define   DCREG_SH_OUTPUT0_TIMING_VS_HEIGHT_VALUE                       GENMASK(15, 0)

#define DCREG_SH_OUTPUT0_TIMING_VBP_HEIGHT                              0x0b603c
#define   DCREG_SH_OUTPUT0_TIMING_VBP_HEIGHT_VALUE                      GENMASK(15, 0)

#define DCREG_SH_OUTPUT0_TIMING_VA_HEIGHT                               0x0b6038
#define   DCREG_SH_OUTPUT0_TIMING_VA_HEIGHT_VALUE                       GENMASK(15, 0)

#define DCREG_SH_OUTPUT0_TIMING_VFP_HEIGHT                              0x0b6034
#define   DCREG_SH_OUTPUT0_TIMING_VFP_HEIGHT_VALUE                      GENMASK(31, 0)

#define DCREG_OUTPUT0                                                   0x0b6008
#define   DCREG_OUTPUT0_WRITE_BACK                                      GENMASK(0, 0)
#define     DCREG_OUTPUT0_WRITE_BACK_DISABLED                           0x0
#define     DCREG_OUTPUT0_WRITE_BACK_ENABLED                            0x1
#define   DCREG_OUTPUT0_WORK_MODE                                       GENMASK(1, 1)
#define     DCREG_OUTPUT0_WORK_MODE_VIDEO                               0x0
#define     DCREG_OUTPUT0_WORK_MODE_COMMAND                             0x1
#define   DCREG_OUTPUT0_SOF_GEN_SEL                                     GENMASK(2, 2)
#define   DCREG_OUTPUT0_CURRENT_VFP_VALUE                               GENMASK(30, 0)
#define   DCREG_OUTPUT0_CURRENT_VFP_UPDATE                              GENMASK(31, 31)
#define   DCREG_OUTPUT0_TE_SEL_ENABLE                                   GENMASK(0, 0)
#define   DCREG_OUTPUT0_TE_POLARITY_POLARITY                            GENMASK(0, 0)
#define   DCREG_OUTPUT0_TE_POLARITY_TE_DELAY_POLARITY                   GENMASK(1, 1)
#define   DCREG_OUTPUT0_SKIP_FRAME_NUM_NUM                              GENMASK(15, 0)
#define   DCREG_OUTPUT0_TSTE_RISING_COUNTER                             GENMASK(31, 0)
#define   DCREG_OUTPUT0_TSTE_SOF_COUNTER                                GENMASK(31, 0)
#define   DCREG_OUTPUT0_TSTE_FALLING_COUNTER                            GENMASK(31, 0)
#define   DCREG_OUTPUT0_SOF_DELAY_COUNTER_VALUE                         GENMASK(31, 0)
#define   DCREG_OUTPUT0_SYSTEM_DELAY_COUNTER_VALUE                      GENMASK(31, 0)
#define   DCREG_OUTPUT0_OFIFO_TP_CURSOR_PIXEL_ALPHA_VALUE               GENMASK(15, 0)
#define   DCREG_OUTPUT0_OFIFO_TP_CURSOR_PIXEL_RED_VALUE                 GENMASK(15, 0)
#define   DCREG_OUTPUT0_OFIFO_TP_CURSOR_PIXEL_GREEN_VALUE               GENMASK(15, 0)
#define   DCREG_OUTPUT0_OFIFO_TP_CURSOR_PIXEL_BLUE_VALUE                GENMASK(15, 0)
#define   DCREG_OUTPUT0_OFIFO_ALPHA0_CRC_SEED_VALUE                     GENMASK(31, 0)
#define   DCREG_OUTPUT0_OFIFO_RED0_CRC_SEED_VALUE                       GENMASK(31, 0)
#define   DCREG_OUTPUT0_OFIFO_GREEN0_CRC_SEED_VALUE                     GENMASK(31, 0)
#define   DCREG_OUTPUT0_OFIFO_BLUE0_CRC_SEED_VALUE                      GENMASK(31, 0)
#define   DCREG_OUTPUT0_OFIFO_ALPHA0_CRC_VALUE_DATA                     GENMASK(31, 0)
#define   DCREG_OUTPUT0_OFIFO_RED0_CRC_VALUE_DATA                       GENMASK(31, 0)
#define   DCREG_OUTPUT0_OFIFO_GREEN0_CRC_VALUE_DATA                     GENMASK(31, 0)
#define   DCREG_OUTPUT0_OFIFO_BLUE0_CRC_VALUE_DATA                      GENMASK(31, 0)
#define   DCREG_OUTPUT0_OFIFO_ALPHA1_CRC_SEED_VALUE                     GENMASK(31, 0)
#define   DCREG_OUTPUT0_OFIFO_RED1_CRC_SEED_VALUE                       GENMASK(31, 0)
#define   DCREG_OUTPUT0_OFIFO_GREEN1_CRC_SEED_VALUE                     GENMASK(31, 0)
#define   DCREG_OUTPUT0_OFIFO_BLUE1_CRC_SEED_VALUE                      GENMASK(31, 0)
#define   DCREG_OUTPUT0_OFIFO_ALPHA1_CRC_VALUE_DATA                     GENMASK(31, 0)
#define   DCREG_OUTPUT0_OFIFO_RED1_CRC_VALUE_DATA                       GENMASK(31, 0)
#define   DCREG_OUTPUT0_OFIFO_GREEN1_CRC_VALUE_DATA                     GENMASK(31, 0)
#define   DCREG_OUTPUT0_OFIFO_BLUE1_CRC_VALUE_DATA                      GENMASK(31, 0)
#define   DCREG_OUTPUT0_FREE_SYNC_CONFIG_ENABLE                         GENMASK(0, 0)
#define     DCREG_OUTPUT0_FREE_SYNC_CONFIG_ENABLE_DISABLED              0x0
#define     DCREG_OUTPUT0_FREE_SYNC_CONFIG_ENABLE_ENABLED               0x1
#define   DCREG_OUTPUT0_FREE_SYNC_CONFIG_MAX_DELAY                      GENMASK(31, 16)
#define   DCREG_OUTPUT0_FREE_SYNC_FINISH_VALUE                          GENMASK(0, 0)

#define DCREG_OUTPUT0_CONFIG_COMMAND_OPT                                0x0b6014
#define   DCREG_OUTPUT0_CONFIG_COMMAND_OPT_OPTION                       GENMASK(0, 0)
#define     DCREG_OUTPUT0_CONFIG_COMMAND_OPT_OPTION_TRIGGER_MODE        0x0
#define     DCREG_OUTPUT0_CONFIG_COMMAND_OPT_OPTION_AUTO_MODE           0x1
#define   DCREG_OUTPUT0_CONFIG_COMMAND_OPT_SYNC                         GENMASK(1, 1)
#define     DCREG_OUTPUT0_CONFIG_COMMAND_OPT_SYNC_DISABLED              0x0
#define     DCREG_OUTPUT0_CONFIG_COMMAND_OPT_SYNC_ENABLED               0x1

#define DCREG_OUTPUT0_DE_SYNC_MODE                                      0x0b6018
#define   DCREG_OUTPUT0_DE_SYNC_MODE_ENABLE                             GENMASK(0, 0)
#define     DCREG_OUTPUT0_DE_SYNC_MODE_ENABLE_DISABLED                  0x0
#define     DCREG_OUTPUT0_DE_SYNC_MODE_ENABLE_ENABLED                   0x1

#define DCREG_SH_OUTPUT0_SCANOUT_COUNTER                                0x0b6094
#define   DCREG_SH_OUTPUT0_SCANOUT_COUNTER_WIDTH                        GENMASK(15, 0)
#define   DCREG_SH_OUTPUT0_SCANOUT_COUNTER_HEIGHT                       GENMASK(31, 16)

#define DCREG_OUTPUT0_SCANOUT_DELAY_COUNTER                             0x0b609c
#define   DCREG_OUTPUT0_SCANOUT_DELAY_COUNTER_VALUE                     GENMASK(31, 0)

#define DCREG_SH_OUTPUT0_URGENT_VALUE                                   0x0b60a0
#define   DCREG_SH_OUTPUT0_URGENT_VALUE_VALUE                           GENMASK(1, 0)
#define   DCREG_SH_OUTPUT0_URGENT_VALUE_ENABLE                          GENMASK(2, 2)
#define     DCREG_SH_OUTPUT0_URGENT_VALUE_ENABLE_DISABLED               0x0
#define     DCREG_SH_OUTPUT0_URGENT_VALUE_ENABLE_ENABLED                0x1

#define DCREG_OUTPUT0_CONFIG                                            0x0b606c
#define   DCREG_OUTPUT0_CONFIG_VIDEO_OPT_OPTION                         GENMASK(0, 0)
#define     DCREG_OUTPUT0_CONFIG_VIDEO_OPT_OPTION_VIDEO_MODE            0x0
#define     DCREG_OUTPUT0_CONFIG_VIDEO_OPT_OPTION_LISTENING_TE_MODE     0x1
#define   DCREG_OUTPUT0_CONFIG_REG_SWITCH                               GENMASK(0, 0)
#define     DCREG_OUTPUT0_CONFIG_REG_SWITCH_WAIT                        0x0
#define     DCREG_OUTPUT0_CONFIG_REG_SWITCH_OK                          0x1
#define   DCREG_OUTPUT0_CONFIG_RESET                                    GENMASK(1, 1)
#define     DCREG_OUTPUT0_CONFIG_RESET_RESET                            0x1
#define   DCREG_OUTPUT0_CONFIG_FORCE_FLIP                               GENMASK(2, 2)

#define DCREG_OUTPUT0_START                                             0x0b6000
#define   DCREG_OUTPUT0_START_START                                     GENMASK(0, 0)
#define     DCREG_OUTPUT0_START_START_TRIGGER                           0x1

#define DCREG_OUTPUT0_SW_CONFIG                                         0x0b6004
#define   DCREG_OUTPUT0_SW_CONFIG_READY                                 GENMASK(0, 0)
#define     DCREG_OUTPUT0_SW_CONFIG_READY_TRIGGER                       0x1

#define DCREG_OUTPUT0_CROSSBAR4_TO4                                     0x0d4008
#define   DCREG_OUTPUT0_CROSSBAR4_TO4_MUX_OUT                           GENMASK(1, 0)
#define   DCREG_OUTPUT0_CROSSBAR4_TO4_PATH                              GENMASK(2, 2)
#define     DCREG_OUTPUT0_CROSSBAR4_TO4_PATH_NOT_VALID                  0x0
#define     DCREG_OUTPUT0_CROSSBAR4_TO4_PATH_VALID                      0x1
#define   DCREG_OUTPUT0_CROSSBAR4_TO4_EX_MUX_OUT                        GENMASK(1, 0)

#define DCREG_POST_PROCESS_OUT                                          0x0d4004
#define   DCREG_POST_PROCESS_OUT_MUX_OUT0_SEL                           GENMASK(2, 0)
#define   DCREG_POST_PROCESS_OUT_MUX_OUT1_SEL                           GENMASK(6, 4)
#define   DCREG_POST_PROCESS_OUT_MUX_OUT2_SEL                           GENMASK(10, 8)
#define   DCREG_POST_PROCESS_OUT_MUX_OUT3_SEL                           GENMASK(14, 12)

#define DCREG_BE_INTR_ENABLE                                            0x080004
#define   DCREG_BE_INTR_ENABLE_OUTPATH0_FRM_START                       GENMASK(0, 0)
#define   DCREG_BE_INTR_ENABLE_OUTPATH0_FRM_DONE                        GENMASK(1, 1)
#define   DCREG_BE_INTR_ENABLE_OUTPATH0_UNDERRUN                        GENMASK(2, 2)
#define   DCREG_BE_INTR_ENABLE_OUTPATH0_INTRA_FRM_INT                   GENMASK(3, 3)
#define   DCREG_BE_INTR_ENABLE_OUTPATH0_INTRA_FRM_EXT                   GENMASK(4, 4)
#define   DCREG_BE_INTR_ENABLE_OUTPATH0_LTM_FRM_START                   GENMASK(5, 5)
#define   DCREG_BE_INTR_ENABLE_OUTPATH0_LTM_FRM_DONE                    GENMASK(6, 6)
#define   DCREG_BE_INTR_ENABLE_OUTPATH0_HIST_WB_DONE                    GENMASK(7, 7)
#define   DCREG_BE_INTR_ENABLE_OUTPATH0_HISTRGB_WB_DONE                 GENMASK(8, 8)
#define   DCREG_BE_INTR_ENABLE_OUTPATH0_DSC_ENC0_INTR                   GENMASK(9, 9)
#define   DCREG_BE_INTR_ENABLE_OUTPATH0_DSC_ENC1_INTR                   GENMASK(10, 10)
#define   DCREG_BE_INTR_ENABLE_OUTPATH0_VDC_INTR                        GENMASK(11, 11)
#define   DCREG_BE_INTR_ENABLE_OUTPATH0_PVRIC_DECODE_INTR               GENMASK(12, 12)
#define   DCREG_BE_INTR_ENABLE_OUTPATH1_FRM_START                       GENMASK(16, 16)
#define   DCREG_BE_INTR_ENABLE_OUTPATH1_FRM_DONE                        GENMASK(17, 17)
#define   DCREG_BE_INTR_ENABLE_OUTPATH1_UNDERRUN                        GENMASK(18, 18)
#define   DCREG_BE_INTR_ENABLE_OUTPATH1_INTRA_FRM_INT                   GENMASK(19, 19)
#define   DCREG_BE_INTR_ENABLE_OUTPATH1_INTRA_FRM_EXT                   GENMASK(20, 20)
#define   DCREG_BE_INTR_ENABLE_OUTPATH1_HIST_WB_DONE                    GENMASK(21, 21)
#define   DCREG_BE_INTR_ENABLE_OUTPATH1_DSC_ENC0_INTR                   GENMASK(22, 22)
#define   DCREG_BE_INTR_ENABLE_OUTPATH1_DSC_ENC1_INTR                   GENMASK(23, 23)
#define   DCREG_BE_INTR_ENABLE_OUTPATH1_PVRIC_DECODE_INTR               GENMASK(24, 24)
#define   DCREG_BE_INTR_ENABLE_OUTPATH0_TE_RISING_EDGE                  GENMASK(25, 25)
#define   DCREG_BE_INTR_ENABLE_OUTPATH0_TE_FALLING_EDGE                 GENMASK(26, 26)
#define   DCREG_BE_INTR_ENABLE_OUTPATH0_TE_SOF                          GENMASK(27, 27)
#define   DCREG_BE_INTR_ENABLE_OUTPATH1_TE_RISING_EDGE                  GENMASK(28, 28)
#define   DCREG_BE_INTR_ENABLE_OUTPATH1_TE_FALLING_EDGE                 GENMASK(29, 29)
#define   DCREG_BE_INTR_ENABLE_OUTPATH1_TE_SOF                          GENMASK(30, 30)
#define   DCREG_BE_INTR_ENABLE_OUTPATH1_LTM_FRM_DONE                    GENMASK(31, 31)

#define DCREG_BE_INTR_STATUS                                            0x080008
#define   DCREG_BE_INTR_STATUS_OUTPATH0_FRM_START                       GENMASK(0, 0)
#define   DCREG_BE_INTR_STATUS_OUTPATH0_FRM_DONE                        GENMASK(1, 1)
#define   DCREG_BE_INTR_STATUS_OUTPATH0_UNDERRUN                        GENMASK(2, 2)
#define   DCREG_BE_INTR_STATUS_OUTPATH0_INTRA_FRM_INT                   GENMASK(3, 3)
#define   DCREG_BE_INTR_STATUS_OUTPATH0_INTRA_FRM_EXT                   GENMASK(4, 4)
#define   DCREG_BE_INTR_STATUS_OUTPATH0_LTM_FRM_START                   GENMASK(5, 5)
#define   DCREG_BE_INTR_STATUS_OUTPATH0_LTM_FRM_DONE                    GENMASK(6, 6)
#define   DCREG_BE_INTR_STATUS_OUTPATH0_HIST_WB_DONE                    GENMASK(7, 7)
#define   DCREG_BE_INTR_STATUS_OUTPATH0_HISTRGB_WB_DONE                 GENMASK(8, 8)
#define   DCREG_BE_INTR_STATUS_OUTPATH0_DSC_ENC0_INTR                   GENMASK(9, 9)
#define   DCREG_BE_INTR_STATUS_OUTPATH0_DSC_ENC1_INTR                   GENMASK(10, 10)
#define   DCREG_BE_INTR_STATUS_OUTPATH0_VDC_INTR                        GENMASK(11, 11)
#define   DCREG_BE_INTR_STATUS_OUTPATH0_PVRIC_DECODE_INTR               GENMASK(12, 12)
#define   DCREG_BE_INTR_STATUS_OUTPATH1_FRM_START                       GENMASK(16, 16)
#define   DCREG_BE_INTR_STATUS_OUTPATH1_FRM_DONE                        GENMASK(17, 17)
#define   DCREG_BE_INTR_STATUS_OUTPATH1_UNDERRUN                        GENMASK(18, 18)
#define   DCREG_BE_INTR_STATUS_OUTPATH1_INTRA_FRM_INT                   GENMASK(19, 19)
#define   DCREG_BE_INTR_STATUS_OUTPATH1_INTRA_FRM_EXT                   GENMASK(20, 20)
#define   DCREG_BE_INTR_STATUS_OUTPATH1_HIST_WB_DONE                    GENMASK(21, 21)
#define   DCREG_BE_INTR_STATUS_OUTPATH1_DSC_ENC0_INTR                   GENMASK(22, 22)
#define   DCREG_BE_INTR_STATUS_OUTPATH1_DSC_ENC1_INTR                   GENMASK(23, 23)
#define   DCREG_BE_INTR_STATUS_OUTPATH1_PVRIC_DECODE_INTR               GENMASK(24, 24)
#define   DCREG_BE_INTR_STATUS_OUTPATH0_TE_RISING_EDGE                  GENMASK(25, 25)
#define   DCREG_BE_INTR_STATUS_OUTPATH0_TE_FALLING_EDGE                 GENMASK(26, 26)
#define   DCREG_BE_INTR_STATUS_OUTPATH0_TE_SOF                          GENMASK(27, 27)
#define   DCREG_BE_INTR_STATUS_OUTPATH1_TE_RISING_EDGE                  GENMASK(28, 28)
#define   DCREG_BE_INTR_STATUS_OUTPATH1_TE_FALLING_EDGE                 GENMASK(29, 29)
#define   DCREG_BE_INTR_STATUS_OUTPATH1_TE_SOF                          GENMASK(30, 30)
#define   DCREG_BE_INTR_STATUS_OUTPATH1_LTM_FRM_DONE                    GENMASK(31, 31)

#define DCREG_BE_INTR_OVERFLOW                                          0x080014
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH0_FRM_START                     GENMASK(0, 0)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH0_FRM_DONE                      GENMASK(1, 1)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH0_UNDERRUN                      GENMASK(2, 2)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH0_INTRA_FRM_INT                 GENMASK(3, 3)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH0_INTRA_FRM_EXT                 GENMASK(4, 4)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH0_LTM_FRM_START                 GENMASK(5, 5)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH0_LTM_FRM_DONE                  GENMASK(6, 6)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH0_HIST_WB_DONE                  GENMASK(7, 7)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH0_HISTRGB_WB_DONE               GENMASK(8, 8)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH0_DSC_ENC0_INTR                 GENMASK(9, 9)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH0_DSC_ENC1_INTR                 GENMASK(10, 10)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH0_VDC_INTR                      GENMASK(11, 11)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH0_PVRIC_DECODE_INTR             GENMASK(12, 12)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH1_FRM_START                     GENMASK(16, 16)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH1_FRM_DONE                      GENMASK(17, 17)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH1_UNDERRUN                      GENMASK(18, 18)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH1_INTRA_FRM_INT                 GENMASK(19, 19)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH1_INTRA_FRM_EXT                 GENMASK(20, 20)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH1_HIST_WB_DONE                  GENMASK(21, 21)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH1_DSC_ENC0_INTR                 GENMASK(22, 22)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH1_DSC_ENC1_INTR                 GENMASK(23, 23)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH1_PVRIC_DECODE_INTR             GENMASK(24, 24)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH0_TE_RISING_EDGE                GENMASK(25, 25)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH0_TE_FALLING_EDGE               GENMASK(26, 26)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH0_TE_SOF                        GENMASK(27, 27)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH1_TE_RISING_EDGE                GENMASK(28, 28)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH1_TE_FALLING_EDGE               GENMASK(29, 29)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH1_TE_SOF                        GENMASK(30, 30)
#define   DCREG_BE_INTR_OVERFLOW_OUTPATH1_LTM_FRM_DONE                  GENMASK(31, 31)

#define DCREG_CHIP_ID                                                   0x000020
#define   DCREG_CHIP_ID_ID                                              GENMASK(31, 0)

#define DCREG_CHIP_REV                                                  0x000024
#define   DCREG_CHIP_REV_REV                                            GENMASK(31, 0)

#define DCREG_CHIP_CUSTOMER                                             0x000030
#define   DCREG_CHIP_CUSTOMER_COMPANY                                   GENMASK(31, 16)
#define   DCREG_CHIP_CUSTOMER_GROUP                                     GENMASK(15, 0)

#define DCREG_SH_LINK_NODE0_RESOURCE                                    0x104064
#define   DCREG_SH_LINK_NODE0_RESOURCE_LAYER0                           GENMASK(0, 0)
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER0_DISABLED                0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER0_ENABLED                 0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_LAYER1                           GENMASK(1, 1)
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER1_DISABLED                0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER1_ENABLED                 0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_LAYER2                           GENMASK(2, 2)
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER2_DISABLED                0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER2_ENABLED                 0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_LAYER3                           GENMASK(3, 3)
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER3_DISABLED                0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER3_ENABLED                 0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_LAYER4                           GENMASK(4, 4)
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER4_DISABLED                0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER4_ENABLED                 0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_LAYER5                           GENMASK(5, 5)
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER5_DISABLED                0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER5_ENABLED                 0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_FE0                              GENMASK(6, 6)
#define     DCREG_SH_LINK_NODE0_RESOURCE_FE0_DISABLED                   0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_FE0_ENABLED                    0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_LAYER8                           GENMASK(7, 7)
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER8_DISABLED                0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER8_ENABLED                 0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_LAYER9                           GENMASK(8, 8)
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER9_DISABLED                0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER9_ENABLED                 0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_LAYER10                          GENMASK(9, 9)
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER10_DISABLED               0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER10_ENABLED                0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_LAYER11                          GENMASK(10, 10)
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER11_DISABLED               0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER11_ENABLED                0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_LAYER12                          GENMASK(11, 11)
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER12_DISABLED               0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER12_ENABLED                0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_LAYER13                          GENMASK(12, 12)
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER13_DISABLED               0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_LAYER13_ENABLED                0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_FE1                              GENMASK(13, 13)
#define     DCREG_SH_LINK_NODE0_RESOURCE_FE1_DISABLED                   0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_FE1_ENABLED                    0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_POST0                            GENMASK(14, 14)
#define     DCREG_SH_LINK_NODE0_RESOURCE_POST0_DISABLED                 0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_POST0_ENABLED                  0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_POST1                            GENMASK(15, 15)
#define     DCREG_SH_LINK_NODE0_RESOURCE_POST1_DISABLED                 0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_POST1_ENABLED                  0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_POST2                            GENMASK(16, 16)
#define     DCREG_SH_LINK_NODE0_RESOURCE_POST2_DISABLED                 0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_POST2_ENABLED                  0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_POST3                            GENMASK(17, 17)
#define     DCREG_SH_LINK_NODE0_RESOURCE_POST3_DISABLED                 0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_POST3_ENABLED                  0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_WB0                              GENMASK(18, 18)
#define     DCREG_SH_LINK_NODE0_RESOURCE_WB0_DISABLED                   0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_WB0_ENABLED                    0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_WB1                              GENMASK(19, 19)
#define     DCREG_SH_LINK_NODE0_RESOURCE_WB1_DISABLED                   0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_WB1_ENABLED                    0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_WB_SPLITER0                      GENMASK(20, 20)
#define     DCREG_SH_LINK_NODE0_RESOURCE_WB_SPLITER0_DISABLED           0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_WB_SPLITER0_ENABLED            0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_WB_SPLITER1                      GENMASK(21, 21)
#define     DCREG_SH_LINK_NODE0_RESOURCE_WB_SPLITER1_DISABLED           0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_WB_SPLITER1_ENABLED            0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_DSC0                             GENMASK(22, 22)
#define     DCREG_SH_LINK_NODE0_RESOURCE_DSC0_DISABLED                  0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_DSC0_ENABLED                   0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_DSC1                             GENMASK(23, 23)
#define     DCREG_SH_LINK_NODE0_RESOURCE_DSC1_DISABLED                  0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_DSC1_ENABLED                   0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_DSC2                             GENMASK(24, 24)
#define     DCREG_SH_LINK_NODE0_RESOURCE_DSC2_DISABLED                  0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_DSC2_ENABLED                   0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_DBUFFER0                         GENMASK(25, 25)
#define     DCREG_SH_LINK_NODE0_RESOURCE_DBUFFER0_DISABLED              0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_DBUFFER0_ENABLED               0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_DBUFFER1                         GENMASK(26, 26)
#define     DCREG_SH_LINK_NODE0_RESOURCE_DBUFFER1_DISABLED              0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_DBUFFER1_ENABLED               0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_DBUFFER2                         GENMASK(27, 27)
#define     DCREG_SH_LINK_NODE0_RESOURCE_DBUFFER2_DISABLED              0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_DBUFFER2_ENABLED               0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_TM0                              GENMASK(28, 28)
#define     DCREG_SH_LINK_NODE0_RESOURCE_TM0_DISABLED                   0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_TM0_ENABLED                    0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_TM1                              GENMASK(29, 29)
#define     DCREG_SH_LINK_NODE0_RESOURCE_TM1_DISABLED                   0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_TM1_ENABLED                    0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_TM2                              GENMASK(30, 30)
#define     DCREG_SH_LINK_NODE0_RESOURCE_TM2_DISABLED                   0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_TM2_ENABLED                    0x1
#define   DCREG_SH_LINK_NODE0_RESOURCE_RESERVED                         GENMASK(31, 31)
#define     DCREG_SH_LINK_NODE0_RESOURCE_RESERVED_DISABLED              0x0
#define     DCREG_SH_LINK_NODE0_RESOURCE_RESERVED_ENABLED               0x1

#define DCREG_LINK_NODE0_CONFIG                                         0x104304
#define   DCREG_LINK_NODE0_CONFIG_PDMA_WEIGHT                           GENMASK(2, 0)
#define   DCREG_LINK_NODE0_CONFIG_RESET                                 GENMASK(3, 3)

#define DCREG_LINK_NODE0_SW_DONE                                        0x1040b4
#define   DCREG_LINK_NODE0_SW_DONE_VALUE                                GENMASK(0, 0)
#define     DCREG_LINK_NODE0_SW_DONE_VALUE_INVALID                      0x0
#define     DCREG_LINK_NODE0_SW_DONE_VALUE_VALID                        0x1

#define DCREG_LINK_NODE0_START                                          0x1040a0
#define   DCREG_LINK_NODE0_START_VALUE                                  GENMASK(0, 0)

#endif /* __MBU_DPU_REGS_H__ */
