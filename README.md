# NUEDC 电机控制

本仓库包含 ESP32、TI MSPM0G3507 电机固件和用于手柄调试的 Python 上位机

## 项目结构

```text
firmware/   PlatformIO 固件、SDK 和测试
TI/         MSPM0G3507 工程、逐飞库和测试
host/       Python 手柄调试程序
docs/       电机协议参考资料
```

## ESP32 固件

固件使用 1.14 英寸、240×135、ST7789V 彩屏显示菜单。当前提供以下 Demo 菜单项：

| 菜单项 | 用途 |
| --- | --- |
| `Controller Motor` | 接收 Python 手柄速度消息并控制电机 |
| `Motor Ramp` | 每 100 ms 改变 10 RPM，在 -300 至 300 RPM 之间往返 |
| `Motor Position` | 失能电机，每 500 ms 显示和输出转子角度 |
| `Comm Unreliable` | 每 500 ms 发送一次非可靠计数消息 |
| `Comm Reliable` | 可靠发送计数消息，确认后等待 500 ms 再发送下一条 |

使用 S1/S2 上下移动，S3 进入 Demo，S4 返回菜单。按钮和屏幕引脚如下：

| 功能 | GPIO |
| --- | --- |
| S1 / 上 | 0 |
| S2 / 下 | 35 |
| S3 / 确认 | 34 |
| S4 / 返回 | 39 |
| 屏幕 SCL | 18 |
| 屏幕 SDA | 23 |
| 屏幕 RST | 33 |
| 屏幕 BL | 12 |
| 屏幕 DC | 27 |
| 屏幕 CS | 32 |

使用 PlatformIO 编译或烧录：

```shell
pio run -d firmware
pio run -d firmware -t upload
```

电机 Demo 使用 `Serial2` 连接驱动器，RX 为 GPIO25，TX 为 GPIO26。进入 `Motor Position` 后可通过串口监视器查看角度：

```shell
pio device monitor -d firmware
```

`Controller Motor` 使用 `comm::Link` 接收消息类型 `1` 的速度命令。载荷是一个 2 字节大端有符号整数：

```text
消息类型: 1
发送模式: 非可靠
载荷: int16 大端序
```

速度量范围为 `-1000` 到 `1000`，映射到 `-300` 到 `300 RPM`。无效类型、长度或数值会被忽略。连续 500 ms 没有收到有效命令时，电机自动停止。

启动流程等待电机上电 2 秒，发送使能命令并读取一次位置。USB 串口只承载二进制协议帧，不输出文本日志。

## TI MSPM0G3507 固件

TI 工程提供与 ESP32 固件一致的电机和通信 Demo，另有 `State Entry A`、`State Entry B` 两个状态机入口。使用 S1/S2 上下移动，S3 进入 Demo，S4 返回菜单。

macOS GCC 工程默认使用安装在 `/Applications/ArmGNUToolchain/15.2.rel1/arm-none-eabi/bin` 的 Arm GNU Toolchain 15.2.Rel1。只编译固件或运行本机测试：

```shell
make -C TI/project/gcc all
make -C TI/project/gcc test
```

使用其他安装位置时，通过 `TOOLCHAIN` 指定工具链的 `bin` 目录：

```shell
make -C TI/project/gcc TOOLCHAIN=/path/to/arm-none-eabi/bin all
```

安装 [uv](https://docs.astral.sh/uv/) 并连接调试器后，可以通过 pyOCD 安装器件包并烧录应用：

```shell
./TI/build.sh app
```

硬件冒烟程序会让 A14 蜂鸣器响两次，可分别编译、烧录已生成的固件，或重新编译后烧录：

```shell
./TI/build.sh smoke-build
./TI/build.sh smoke-flash
./TI/build.sh smoke
```

Keil 工程位于 `TI/project/keil/SeekFree_MSPM0G3507_Device_Library.uvprojx`。

上位机通信使用 UART0（TX A10、RX A11），电机通信使用 UART2（TX B15、RX B16），波特率均为 115200。四个菜单按键 S1 至 S4 依次使用 A30、A31、B0、B1。

## Python 上位机

安装 [uv](https://docs.astral.sh/uv/) 后执行：

```shell
uv sync --project host
uv run --project host host/main.py --port /dev/cu.usbserial-0001
```

Windows 串口可以写为 `COM3`。程序默认读取第一个手柄的 axis 3，每 50 ms 向开发板发送一次速度命令：

```shell
uv run --project host host/main.py --port COM3 --axis 3
uv run --project host host/main.py --port COM3 --axis 3 --invert
```

按 `Ctrl-C` 退出时会发送零速度命令

手柄采集行为参考 [ZhiGrip-Joystick](https://github.com/LanternCX/ZhiGrip-Joystick/blob/main/main.py)，串口通信使用 `host/link.py` 中与两套固件一致的 COBS 与 CRC-8 协议。

## 通信 Demo 接收端

进入 `Comm Unreliable` 或 `Comm Reliable` 后，运行同一个 Python 接收程序：

```shell
uv run --project host host/receive_counter.py --port /dev/cu.usbserial-0001
```

程序显示消息的可靠性和递增计数。可靠模式的确认由 `Link` 自动返回。

## LSP

安装 clangd 和 PlatformIO 后生成固件编译数据库：

```shell
pio run -d firmware -t compiledb
```

在编辑器中重启 clangd 即可。修改 `firmware/platformio.ini`、依赖或源文件后需要重新生成编译数据库。TI 工程使用仓库内的 `TI/compile_flags.txt`
