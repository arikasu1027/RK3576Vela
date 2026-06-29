# RK3576 NuttX BSP for KICKPI-K7

RK3576 (Rockchip) NuttX RTOS Board Support Package for the KICKPI-K7 development board.

## Features

### Drivers (35 total)

| Category | Drivers |
|----------|---------|
| **Communication** | UART×12, I2C×5, SPI×4, USB×2, Ethernet×2 |
| **Display** | VOP (4-layer), MIPI-DSI, HDMI, Framebuffer, LVGL |
| **Camera** | MIPI-CSI (RAW8/10/12, RGB888, YUV422) |
| **Input** | Touch (GT911), GPIO×160 pins |
| **Storage** | SDMMC (SD/eMMC), SPI NOR Flash |
| **Audio** | I2S×3, S/PDIF |
| **Bus** | CAN Bus (M_CAN), PCIe (Gen2 x1) |
| **Wireless** | Bluetooth HCI (UART-based) |
| **HDMI** | CEC (Consumer Electronics Control) |
| **System** | CRU, GRF, PMU, Timer, Watchdog, RTC, DMA |
| **Security** | Crypto (AES/SHA) |
| **AI** | GPU (Mali-G51), NPU (6 TOPS) |

### Hardware

- **SoC**: Rockchip RK3576 (Cortex-A72 + Cortex-A53)
- **Board**: KICKPI-K7 development board
- **RAM**: Up to 8GB LPDDR4/5
- **Storage**: eMMC + SD card + SPI NOR Flash
- **Display**: MIPI-DSI + HDMI + MIPI-CSI
- **Connectivity**: Ethernet, USB, WiFi/BT (optional)

## Quick Start

### Prerequisites

- ARM64 ELF toolchain: `aarch64-none-elf-gcc 13.2`
- NuttX source tree (openvela fork)

### Build

```bash
# Configure
cd nuttx
tools/configure.sh kickpi-k7:nsh

# Build
make -j$(nproc)
```

### Flash

1. Copy `nuttx` binary to SD card
2. Insert SD card into KICKPI-K7
3. Power on — serial console on UART2 (115200 baud)

## Project Structure

```
├── bsp/
│   ├── nuttx-arch/         # Chip-level drivers (RK3576)
│   │   └── arm64/src/rk3576/
│   │       ├── rk3576_*.c  # Driver source files
│   │       ├── rk3576_*.h  # Driver headers
│   │       └── hardware/   # Register definitions
│   ├── nuttx-boards/       # Board configuration
│   │   └── arm64/rk3576/kickpi-k7/
│   └── defconfig/          # Default configurations
├── apps/                   # Test applications
├── docs/                   # Documentation
└── hardware/               # Hardware documentation
```

## Driver Source Files

| File | Lines | Peripheral |
|------|-------|------------|
| `rk3576_serial.c` | 1529 | UART (DW APB, 12 ports) |
| `rk3576_i2c.c` | 813 | I2C Master |
| `rk3576_gpio.c` | 719 | GPIO (5 banks, 160 pins) |
| `rk3576_eth.c` | 717 | Ethernet MAC (GMAC) |
| `rk3576_usb.c` | 684 | USB 2.0 OTG (DWC2) |
| `rk3576_vop.c` | 600 | Display Pipeline (4 layers) |
| `rk3576_hdmi.c` | 577 | HDMI TX |
| `rk3576_dsi.c` | 561 | MIPI DSI Host |
| `rk3576_dmac.c` | 508 | DMA Controller |
| `rk3576_sdmmc.c` | 1016 | SD/eMMC |
| `rk3576_i2s.c` | 763 | I2S Audio |
| `rk3576_spi.c` | 498 | SPI Master |
| `rk3576_adc.c` | 407 | SARADC (6ch) |
| `rk3576_gpu.c` | 401 | Mali-G51 GPU |
| `rk3576_csi.c` | 392 | MIPI CSI-2 Camera |
| `rk3576_pwm.c` | 378 | PWM (4 controllers) |
| `rk3576_timer.c` | 357 | General Purpose Timer |
| `rk3576_rtc.c` | 360 | Real-Time Clock |
| `rk3576_wdt.c` | 314 | Watchdog Timer |
| `rk3576_fb.c` | 201 | Framebuffer (/dev/fb0) |
| `rk3576_pcie.c` | 185 | PCIe (Gen2 x1) |
| `rk3576_cec.c` | 200 | HDMI CEC |
| `rk3576_spdif.c` | 141 | S/PDIF Audio |
| `rk3576_bluetooth.c` | 117 | Bluetooth HCI |
| `rk3576_can.c` | 243 | CAN Bus (M_CAN) |
| `rk3576_lvgl.c` | 189 | LVGL Integration |
| `rk3576_boot.c` | 213 | Boot/Init |
| `rk3576_cru.c` | 100 | Clock/Reset |
| `rk3576_crypto.c` | 97 | AES/SHA |
| `rk3576_spiflash.c` | 98 | SPI NOR Flash |
| `rk3576_npu.c` | 67 | Neural Processing |
| `rk3576_pmu.c` | 69 | Power Management |
| `rk3576_grf.c` | 55 | General Register File |
| `rk3576_touch.c` | 56 | Touch Screen (GT911) |
| `rk3576_ddr.c` | 44 | DDR Controller |

**Total**: ~18,000 lines of driver code

## License

Apache License 2.0
