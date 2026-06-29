# KICKPI-K7 NuttX BSP 快速入门

## 一、开发环境准备

### 1. 安装工具链

```bash
# 下载 ARM64 ELF 工具链
wget https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-aarch64-none-elf.tar.xz

# 解压到工具目录
tar xf arm-gnu-toolchain-13.2.rel1-x86_64-aarch64-none-elf.tar.xz

# 添加到 PATH
export PATH=$PWD/arm-gnu-toolchain-13.2.Rel1-x86_64-aarch64-none-elf/bin:$PATH
```

### 2. 获取 NuttX 源码

```bash
git clone https://github.com/openvela/nuttx.git
git clone https://github.com/openvela/apps.git
```

### 3. 应用 BSP 补丁

```bash
# 复制 arch 支持文件
cp -r bsp/nuttx-arch/arm64/src/rk3576/* nuttx/arch/arm64/src/rk3576/

# 复制 board 支持文件
cp -r bsp/nuttx-boards/arm64/rk3576/* nuttx/boards/arm64/rk3576/
```

## 二、编译固件

```bash
cd nuttx

# 配置
tools/configure.sh kickpi-k7:nsh

# 编译
make -j$(nproc)
```

编译成功后生成 `nuttx` 固件文件（约 3MB）。

## 三、烧录运行

### 方法 1: SD 卡启动

1. 将 `nuttx` 固件复制到 SD 卡
2. 插入 KICKPI-K7 开发板 SD 卡槽
3. 上电启动

### 方法 2: 串口调试

1. 连接 USB-TTL 模块到开发板 UART2
2. 波特率: 115200
3. 使用 minicom/screen 连接:

```bash
sudo minicom -D /dev/ttyUSB0 -b 115200
```

## 四、驱动说明

本项目为 RK3576 提供 35 个 NuttX 驱动:

| 类别 | 驱动 |
|------|------|
| 通信 | UART×12, I2C×5, SPI×4, USB×2, Ethernet×2 |
| 显示 | VOP, DSI, HDMI, Framebuffer, LVGL |
| 摄像头 | MIPI-CSI |
| 输入 | Touch (GT911), GPIO×160 |
| 存储 | SDMMC, SPI-Flash |
| 音频 | I2S×3, S/PDIF |
| 总线 | CAN, PCIe |
| 无线 | Bluetooth HCI |
| 系统 | CRU, GRF, PMU, Timer, WDT, RTC, DMA |

详细 API 请参考各驱动头文件。
