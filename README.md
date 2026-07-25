# NUEDC ESP32 电机控制

本仓库包含 ESP32 电机固件和用于手柄调试的 Python 上位机

## 项目结构

```text
firmware/   PlatformIO 固件、SDK 和测试
host/       Python 手柄调试程序
docs/       电机协议参考资料
```

## 固件

在仓库根目录执行：

```shell
pio run -d firmware
pio run -d firmware -t upload
pio device monitor -d firmware
```

Demo 使用 `Serial2` 连接电机，RX 为 GPIO25，TX 为 GPIO26。USB 串口使用 `comm::Link` 接收消息类型 `1` 的速度命令。载荷是一个 2 字节大端有符号整数：

```text
消息类型: 1
发送模式: 非可靠
载荷: int16 大端序
```

速度量范围为 `-1000` 到 `1000`，映射到 `-300` 到 `300 RPM`。无效类型、长度或数值会被忽略。连续 500 ms 没有收到有效命令时，电机自动停止。

启动流程等待电机上电 2 秒，发送使能命令并读取一次位置。USB 串口只承载二进制协议帧，不输出文本日志。

## Python 上位机

安装 [uv](https://docs.astral.sh/uv/) 后执行：

```shell
uv sync --project host
uv run --project host host/main.py --port /dev/cu.usbserial-0001
```

Windows 串口可以写为 `COM3`。程序默认读取第一个手柄的 axis 3，每 50 ms 发送一次速度命令：

```shell
uv run --project host host/main.py --port COM3 --axis 3
uv run --project host host/main.py --port COM3 --axis 3 --invert
```

按 `Ctrl-C` 退出时会发送零速度命令

手柄采集行为参考 [ZhiGrip-Joystick](https://github.com/LanternCX/ZhiGrip-Joystick/blob/main/main.py)，串口通信使用 `host/link.py` 中与固件一致的 COBS 与 CRC-8 协议。

## LSP

安装 clangd 和 PlatformIO 后生成固件编译数据库：

```shell
pio run -d firmware -t compiledb
```

在编辑器中重启 clangd 即可。修改 `firmware/platformio.ini`、依赖或源文件后需要重新生成编译数据库
