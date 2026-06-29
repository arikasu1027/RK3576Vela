# KICKPI-K7 Board Support for NuttX/Vela

This directory contains the board support package (BSP) for the KICKPI-K7 development board based on the Rockchip RK3576 SoC.

## Directory Structure

```
rk3576/
├── kickpi-k7_bringup.c    # Board initialization
├── Makefile               # Build configuration
└── README.md              # This file
```

## Hardware Requirements

- KICKPI-K7 development board
- USB-TTL module for serial debugging
- MicroSD card (for booting)
- Power supply (5V/3A via USB-C)

## Building

### 1. Set up the development environment

```bash
# Extract the GCC toolchain
cd /home/arisa/RK3576Vela/rk3576/3-SoftwareData/GCC_Cross_toolchains/
tar -xf gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu.tar.xz
export PATH=$PWD/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin:$PATH
```

### 2. Clone the NuttX source code

```bash
cd /home/arisa/RK3576Vela/smart_car_headunit
git clone https://github.com/open-vela/nuttx.git
git clone https://github.com/open-vela/apps.git
```

### 3. Configure NuttX for KICKPI-K7

```bash
cd nuttx
./tools/configure.sh ../bsp/defconfig/kickpi-k7_nsh_defconfig
```

### 4. Build NuttX

```bash
make -j$(nproc)
```

## Flashing

### Option 1: SD Card

1. Format the SD card as FAT32
2. Copy the `nuttx.bin` file to the SD card
3. Insert the SD card into the KICKPI-K7
4. Set the boot jumper to SD card boot
5. Power on the board

### Option 2: eMMC (using RKDevTool)

1. Install RKDevTool on Windows
2. Connect the board via USB OTG
3. Use RKDevTool to flash the firmware

## Serial Debugging

1. Connect the USB-TTL module to the UART2 header on the KICKPI-K7
2. Open a terminal emulator (e.g., minicom, PuTTY)
3. Set the baud rate to 1500000
4. Power on the board

```bash
minicom -D /dev/ttyUSB0 -b 1500000
```

## GPIO Pin Mapping

### UART2 (Debug Console)
- UART2_TX: Pin 8
- UART2_RX: Pin 10
- GND: Pin 6

### GPIO (Example)
- GPIO1_A0: Pin 11 (旋钮 A 相)
- GPIO1_A1: Pin 12 (旋钮 B 相)
- GPIO1_B0: Pin 13 (返回键)
- GPIO1_B1: Pin 15 (主页键)
- GPIO1_B2: Pin 16 (音量+)
- GPIO1_B3: Pin 18 (音量-)

## Testing

After flashing, you should see the NuttX boot log on the serial console:

```
NuttX Shell (NSH) NuttX-10.x.x
nsh>
```

## Troubleshooting

### No output on serial console
- Check the UART wiring
- Verify the baud rate (1500000)
- Check if the board is powered on

### Board doesn't boot
- Verify the boot jumper setting
- Check the power supply
- Try reflashing the firmware

## Next Steps

1. Implement GPIO driver for buttons and rotary encoder
2. Implement MIPI DSI display driver
3. Implement I2C touch driver
4. Add Bluetooth HCI support
