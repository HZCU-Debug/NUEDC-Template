# 电赛控制题嵌入式部分框架

本仓库提供电子设计竞赛常用的控制基础设施，包括多轴电机驱动、屏幕菜单、程序生命周期、状态机、串口通信和上位机工具。机构布局、运动流程和赛题策略由具体项目实现。

ESP32 固件支持张大头与正点原子两套伺服模块，并通过统一接口管理 X、Y、Z 和旋转轴。仓库还包含 MSPM0G3507 固件和 Python 手柄调试程序。

## 项目结构

```text
firmware/   ESP32 PlatformIO 固件、驱动和本机测试
ti/         MSPM0G3507 工程、逐飞库和本机测试
host/       Python 手柄与通信调试程序
docs/       电机和 IMU 参考资料
```

## ESP32 快速开始

### 1. 准备环境

推荐安装 [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation/index.html) 并使用命令行，也可以在 VS Code 中安装 PlatformIO IDE 插件完成构建、烧录和串口监视。命令行环境可以这样确认：

```shell
pio --version
```

`firmware/platformio.ini` 提供两个构建环境：

| 环境 | 开发板 |
| --- | --- |
| `esp32s3` | ESP32-S3 DevKit，默认环境 |
| `esp32` | ESP32 Dev Module |

### 2. 配置硬件

烧录前检查 [`firmware/include/config/hardware.h`](firmware/include/config/hardware.h)：

1. `kMotorModel` 与四个轴的实际电机型号一致
2. 四个轴的驱动器地址、每圈脉冲数和方向一致
3. 对应开发板分支中的屏幕、按键、IMU 和电机引脚一致

框架默认使用 ESP32-S3 和四轴正点原子电机。

### 3. 编译与烧录

使用 VS Code 时，打开仓库中的 `firmware` 目录作为 PlatformIO 项目，然后通过底部状态栏或 PlatformIO 面板执行 Build、Upload 和 Serial Monitor。需要切换开发板时，在项目环境中选择 `esp32s3` 或 `esp32`。

命令行方式如下。

编译默认的 ESP32-S3 环境：

```shell
pio run -d firmware
```

显式选择开发板：

```shell
pio run -d firmware -e esp32s3
pio run -d firmware -e esp32
```

烧录固件：

```shell
pio run -d firmware -e esp32s3 -t upload
```

`firmware/build.sh` 会生成 clangd 编译数据库并立即烧录指定环境：

```shell
./firmware/build.sh esp32s3
./firmware/build.sh esp32
```

查看文本调试输出：

```shell
pio device monitor -d firmware -e esp32s3
```

`Controller Motor`、`Motor Test` 和通信 Demo 会在 USB 串口传输二进制协议帧，不适合同时作为文本日志查看。

## 菜单操作

固件使用 1.14 英寸、240×135、ST7789V 彩屏。按键低电平有效：

| 按键 | 功能 |
| --- | --- |
| S1 | 上一项 |
| S2 | 下一项 |
| S3 | 进入程序 |
| S4 | 停止程序并返回菜单 |

ESP32-S3 使用内部上拉。ESP32 配置需要按硬件提供外部上拉。

上电后可以直接选择以下程序：

| 菜单项 | 作用 |
| --- | --- |
| `Controller Motor` | 接收 Python 手柄速度消息并控制 X 轴，500 ms 无有效命令时停止 |
| `Motor Ramp` | X 轴在 -300 至 300 RPM 之间往返变速 |
| `Motor Position` | 失能 X 轴，每 500 ms 读取并输出角度 |
| `Motor Test` | 通过 USB 串口测试 X、Y、Z、R 四个轴的速度和绝对位置 |
| `Comm Unreliable` | 每 500 ms 发送一次不等待确认的计数消息 |
| `Comm Reliable` | 发送带确认和自动重试的计数消息 |
| `Quaternion` | 读取 IMU 并输出积分得到的四元数 |
| `State Entry A` | 从状态 A 进入状态机 Demo |
| `State Entry B` | 从状态 B 进入状态机 Demo |

电机程序会发送真实运动命令。首次测试应断开机构负载或架空电机，并确保急停方式可用。

## ESP32 引脚配置

引脚定义集中在 [`firmware/include/config/hardware.h`](firmware/include/config/hardware.h)。修改后重新编译对应环境。

### ESP32-S3

| 功能 | GPIO |
| --- | ---: |
| 屏幕 SCL | 16 |
| 屏幕 SDA | 17 |
| 屏幕 RST | 4 |
| 屏幕 BL | 15 |
| 屏幕 DC | 5 |
| 屏幕 CS | 14 |
| S1 / 上 | 0 |
| S2 / 下 | 10 |
| S3 / 确认 | 11 |
| S4 / 返回 | 39 |
| IMU SDA | 21 |
| IMU SCL | 18 |
| X 轴 RX / 张大头共享 RX | 41 |
| Y 轴 RX | 42 |
| Z 轴 RX | 1 |
| 旋转轴 RX | 2 |
| 电机共享 TX | 40 |

### ESP32

| 功能 | GPIO |
| --- | ---: |
| 屏幕 SCL | 18 |
| 屏幕 SDA | 23 |
| 屏幕 RST | 33 |
| 屏幕 BL | 12 |
| 屏幕 DC | 27 |
| 屏幕 CS | 32 |
| S1 / 上 | 0 |
| S2 / 下 | 35 |
| S3 / 确认 | 34 |
| S4 / 返回 | 39 |
| IMU SDA | 21 |
| IMU SCL | 22 |
| X 轴 RX / 张大头共享 RX | 25 |
| Y 轴 RX | 13 |
| Z 轴 RX | 14 |
| 旋转轴 RX | 19 |
| 电机共享 TX | 26 |

电机 UART 使用 115200 baud。正点原子电机按轴切换 RX，引脚依次来自 `motorRx`、`motorYRx`、`motorZRx` 和 `motorRollRx`；张大头总线使用 `motorRx` 作为共享 RX。`atkMotorTx` 为 `-1` 时，正点原子电机使用 `motorTx`。

所有模块必须共地，并确认驱动器 UART 电平与控制板兼容。多个推挽输出不得直接并联。

## 电机型号与轴配置

ESP32 的四个轴使用同一种电机型号。`MotorModel::Zdt` 表示张大头，`MotorModel::Atk` 表示正点原子：

```cpp
constexpr MotorModel kMotorModel = MotorModel::Atk;
```

每个轴使用一份 `AxisMotorConfig`：

```cpp
constexpr AxisMotorConfig kXMotor = {
    1,      // 驱动器地址
    3200,   // 张大头电机每圈脉冲数
    false,  // 逻辑方向是否反转
};
```

`kMotorModel` 决定四个轴使用哪套驱动，轴配置决定地址、张大头每圈脉冲数和逻辑方向。默认地址为 X=1、Y=2、Z=3、R=4。

通用电机接口位于 [`firmware/include/motor/motor.h`](firmware/include/motor/motor.h)，业务程序通过 `motor::systemMotor(motor::Axis::X)` 获取配置后的轴，不直接依赖厂商 SDK。

电机测试参数位于 [`firmware/include/config/parameters.h`](firmware/include/config/parameters.h)：

| 配置 | 作用 |
| --- | --- |
| `kMotorTestAcceleration` | 电机测试使用的驱动器原始加速度档位 |
| `kMotorTestMaximumRpm` | 串口电机测试允许的最大转速 |

## 四轴电机测试协议

进入 `Motor Test` 后，固件在 USB 串口上接收 `comm::Link` 可靠消息。轴字段使用 ASCII 字符 `X`、`Y`、`Z` 或 `R`。

| 类型 | 载荷 | 说明 |
| --- | --- | --- |
| `01` | `AXIS + INT16_BE RPM` | 设置速度，RPM 为 0 时停止该轴 |
| `02` | `AXIS + FLOAT32_BE DEGREES + UINT16_BE RPM` | 移动到绝对角度 |

速度范围由 `kMotorTestMaximumRpm` 限制，绝对位置命令的 RPM 必须大于 0。按 S4 退出时会停止四个轴并取消待确认消息。

## Python 上位机

安装 [uv](https://docs.astral.sh/uv/) 并安装依赖：

```shell
uv sync --project host
```

进入 `Controller Motor`，连接手柄后运行：

```shell
uv run --project host host/main.py --port /dev/cu.usbserial-0001
```

Windows 串口可以写为 `COM3`。按需选择摇杆轴和方向：

```shell
uv run --project host host/main.py --port COM3 --axis 3 --invert
```

程序每 50 ms 发送一次速度，范围为 -1000 至 1000，固件映射到 -300 至 300 RPM。按 `Ctrl-C` 退出时会发送零速度命令。

进入任一通信 Demo 后，可以运行计数接收程序：

```shell
uv run --project host host/receive_counter.py --port /dev/cu.usbserial-0001
```

运行上位机测试：

```shell
uv run --project host host/test_link.py
uv run --project host host/test_main.py
uv run --project host host/test_receive_counter.py
```

## 串口通信

ESP32、TI 固件和 Python 上位机使用相同的 `comm::Link` 协议。串口参数为 115200 baud、8N1，线格式为：

```text
COBS(原始包) + 00
```

| 包类型 | 原始包格式 |
| --- | --- |
| 非可靠消息 | `TYPE PAYLOAD CRC` |
| 可靠消息 | `TYPE_WITH_RELIABLE SEQ PAYLOAD CRC` |
| 确认包 | `00 SEQ CRC` |

CRC-8 使用多项式 `07`、初始值 `00`，覆盖此前全部原始包字节。业务消息类型范围为 `01` 至 `7F`；可靠消息把类型最高位置 1。

可靠消息采用单包等待确认：发送端每 50 ms 重发，直到收到相同序列号的确认或调用 `cancel()`。接收端确认重复帧，但同一序列号只向业务程序交付一次。双方必须持续调用 `poll()` 才能接收、确认和重传。

该机制用于应对临时丢包，不提供断电持久化或严格的 exactly-once 保证。

## TI MSPM0G3507

TI 固件只支持张大头电机，不支持正点原子电机。电机驱动器默认地址为 1，每圈脉冲数为 3200。

GCC 工程默认从 `PATH` 查找 Arm GNU Toolchain。编译固件并运行本机测试：

```shell
make -C ti/project/gcc all
make -C ti/project/gcc test
```

指定工具链目录：

```shell
make -C ti/project/gcc TOOLCHAIN=/path/to/arm-none-eabi/bin all
```

安装调试器并通过 pyOCD 烧录应用：

```shell
./ti/build.sh app
```

硬件冒烟程序：

```shell
./ti/build.sh smoke-build
./ti/build.sh smoke-flash
./ti/build.sh smoke
```

TI 板使用 UART0 与上位机通信，使用 UART2 与电机通信：

| 功能 | 引脚 |
| --- | --- |
| 上位机 UART0 TX / RX | A10 / A11 |
| 电机 UART2 TX / RX | B15 / B16 |
| S1 / S2 / S3 / S4 | A30 / A31 / B0 / B1 |

Keil 工程位于 `ti/project/keil/SeekFree_MSPM0G3507_Device_Library.uvprojx`。

## 开发辅助

生成 ESP32 的 clangd 编译数据库：

```shell
pio run -d firmware -e esp32s3 -t compiledb
```

修改开发板、依赖或源文件后需要重新生成。TI 工程的语言服务参数位于 `ti/compile_flags.txt`。

协议手册和器件资料位于 [`docs/reference`](docs/reference)。
