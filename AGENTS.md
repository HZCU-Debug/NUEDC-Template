# Repository Guidelines

## 项目说明

本仓库是用于全国大学生电子设计竞赛的电机控制项目，包含 ESP32、TI MSPM0G3507 固件和 Python 上位机。ESP32 固件基于 Arduino 框架和 PlatformIO，TI 固件支持 GCC 和 Keil 工程，上位机使用 uv 管理。

## 项目结构

- `firmware/src/main.cpp`：固件入口，包含 `setup()` 和 `loop()`。
- `firmware/include/`、`firmware/src/`：电机 SDK 公开接口和实现。
- `firmware/platformio.ini`：开发板、框架、依赖和串口配置。
- `ti/project/`：MSPM0G3507 应用代码及 GCC、Keil 工程。
- `ti/libraries/`：TI SDK 和逐飞开源库。
- `ti/test/`：可在本机运行的 TI 固件测试。
- `host/`：Python 手柄调试程序。
- `docs/reference/`：电机协议参考资料。
- `firmware/.clangd`、`ti/compile_flags.txt`、`.vscode/`、`.zed/`：编辑器及语言服务配置。

## 常用命令

- `pio run -d firmware`：编译固件。
- `pio run -d firmware -t upload`：编译并烧录固件。
- `pio device monitor -d firmware`：打开串口监视器。
- `pio run -d firmware -t compiledb`：更新 clangd 使用的编译数据库。
- `pio run -d firmware -t clean`：清理固件构建产物。
- `make -C ti/project/gcc all`：编译 TI 固件。
- `make -C ti/project/gcc test`：运行 TI 固件的本机测试。
- `./ti/build.sh app`：通过 pyOCD 编译并烧录 TI 固件。
- `./ti/build.sh smoke-build`：编译 TI 硬件冒烟程序。
- `./ti/build.sh smoke`：编译并烧录 TI 硬件冒烟程序。
- `uv sync --project host`：安装 Python 上位机依赖。
- `uv run --project host host/test_main.py`：运行 Python 上位机测试。

修改 ESP32 固件后运行 `pio run -d firmware`，修改 TI 固件后运行 `make -C ti/project/gcc all` 和 `make -C ti/project/gcc test`。修改 Python 上位机时还需运行对应测试。不要提交 `.pio/`、`ti/project/gcc/build/` 和 `compile_commands.json` 等生成文件。

# Repository Rules

1. 任何模块对外暴露的接口，需要进行注释
2. 注释避免使用行尾注释，对于接口使用 Doxgen 风格块注释 `/* */`，对于过程性代码使用行注释 `//`
3. 注释末尾禁止使用句号

## Agent skills

### Issue tracker

任务与需求使用 GitHub Issues；外部 PR 不作为需求分类入口。详见 `docs/agents/issue-tracker.md`。

### Triage labels

使用 `needs-triage`、`needs-info`、`ready-for-agent`、`ready-for-human`、`wontfix` 五种分类标签。详见 `docs/agents/triage-labels.md`。

### Domain docs

采用单一领域上下文，使用根目录 `CONTEXT.md` 和 `docs/adr/`。详见 `docs/agents/domain.md`。
