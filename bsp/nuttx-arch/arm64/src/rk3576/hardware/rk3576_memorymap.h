/****************************************************************************
 * arch/arm64/src/rk3576/hardware/rk3576_memorymap.h
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __ARCH_ARM64_SRC_RK3576_HARDWARE_RK3576_MEMORYMAP_H
#define __ARCH_ARM64_SRC_RK3576_HARDWARE_RK3576_MEMORYMAP_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* RK3576 Peripheral Base Addresses (from TRM) */

/* GPIO Controllers */
#define RK3576_GPIO0_ADDR          0xff750000
#define RK3576_GPIO1_ADDR          0xff760000
#define RK3576_GPIO2_ADDR          0xff770000
#define RK3576_GPIO3_ADDR          0xff780000
#define RK3576_GPIO4_ADDR          0xff790000

/* UART Controllers (12 ports total, DW APB UART, TRM Chapter 10)
 * UART0-UART4 confirmed from TRM. UART5-UART11 exist but addresses
 * need verification from the full TRM.
 */
#define RK3576_UART0_ADDR          0xff170000
#define RK3576_UART1_ADDR          0xff180000
#define RK3576_UART2_ADDR          0xff190000  /* Debug console */
#define RK3576_UART3_ADDR          0xff1a0000
#define RK3576_UART4_ADDR          0xff1b0000

/* I2C Controllers */
#define RK3576_I2C0_ADDR           0xff1c0000
#define RK3576_I2C1_ADDR           0xff1d0000
#define RK3576_I2C2_ADDR           0xff1e0000
#define RK3576_I2C3_ADDR           0xff1f0000
#define RK3576_I2C4_ADDR           0xff200000

/* SPI Controllers */
#define RK3576_SPI0_ADDR           0xff210000
#define RK3576_SPI1_ADDR           0xff220000
#define RK3576_SPI2_ADDR           0xff230000
#define RK3576_SPI3_ADDR           0xff240000

/* PWM Controllers */
#define RK3576_PWM0_ADDR           0xff250000
#define RK3576_PWM1_ADDR           0xff260000
#define RK3576_PWM2_ADDR           0xff270000
#define RK3576_PWM3_ADDR           0xff280000

/* Display Controllers */
#define RK3576_VOP0_ADDR           0xff2a0000
#define RK3576_VOP1_ADDR           0xff2b0000
#define RK3576_DSI0_ADDR           0xff2c0000
#define RK3576_DSI1_ADDR           0xff2d0000
#define RK3576_HDMI_ADDR           0xff2e0000

/* Camera Interfaces (MIPI CSI) */
#define RK3576_CSI0_ADDR           0xff300000
#define RK3576_CSI1_ADDR           0xff310000

/* Audio Controllers */
#define RK3576_I2S0_ADDR           0xff2f0000
#define RK3576_I2S1_ADDR           0xff300000
#define RK3576_I2S2_ADDR           0xff310000

/* USB Controllers */
#define RK3576_USB0_ADDR           0xff320000
#define RK3576_USB1_ADDR           0xff330000

/* SDMMC Controllers */
#define RK3576_SDMMC0_ADDR         0xff340000
#define RK3576_SDMMC1_ADDR         0xff350000

/* EMMC Controller */
#define RK3576_EMMC_ADDR           0xff360000

/* DMA Controller - verify against TRM */
#define RK3576_DMAC0_ADDR          0xff1e8000

/* SPI NOR Flash Controller */
#define RK3576_SPIFLASH_ADDR       0xff370000

/* Timer Controllers */
#define RK3576_TIMER0_ADDR         0xff380000
#define RK3576_TIMER1_ADDR         0xff390000

/* Watchdog Timer */
#define RK3576_WDT_ADDR            0xff3a0000

/* Temperature Sensor */
#define RK3576_TSADC_ADDR          0xff3b0000

/* SARADC (General Purpose ADC) - verify against TRM */
#define RK3576_SARADC_ADDR         0xff3b0000

/* Crypto Engine */
#define RK3576_CRYPTO_ADDR         0xff3c0000

/* NPU (Neural Processing Unit) */
#define RK3576_NPU_ADDR            0xff3d0000

/* GPU (Mali-G51) */
#define RK3576_GPU_ADDR            0xff3e0000

/* DDR Controller */
#define RK3576_DDRPCTL_ADDR        0xff3f0000

/* Ethernet MAC Controllers (GMAC) - verify against TRM */
#define RK3576_ETH0_ADDR           0xfe1c0000
#define RK3576_ETH1_ADDR           0xfe010000

/* RTC (Real-Time Clock, in PMU) */
#define RK3576_RTC_ADDR            0xff400000

/* PMU (Power Management Unit) */
#define RK3576_PMU_ADDR            0xff400000

/* CRU (Clock and Reset Unit) */
#define RK3576_CRU_ADDR            0xff410000

/* GRF (General Register File) */
#define RK3576_GRF_ADDR            0xff420000

/* PMUGRF */
#define RK3576_PMUGRF_ADDR         0xff430000

/* SysGrf */
#define RK3576_SYSGRF_ADDR         0xff440000

/* Default GPIO base for PIO operations */
#define RK3576_PIO_ADDR            RK3576_GPIO0_ADDR

/****************************************************************************
 * Public Types
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Public Functions Prototypes
 ****************************************************************************/

#endif /* __ARCH_ARM64_SRC_RK3576_HARDWARE_RK3576_MEMORYMAP_H */
