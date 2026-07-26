# Repository Guidelines

## 项目说明

本仓库是用于全国大学生电子设计竞赛的电机控制项目，包含 ESP32 固件和 Python 上位机。固件基于 Arduino 框架和 PlatformIO，上位机使用 uv 管理。

## 项目结构

- `firmware/src/main.cpp`：固件入口，包含 `setup()` 和 `loop()`。
- `firmware/include/`、`firmware/src/`：电机 SDK 公开接口和实现。
- `firmware/platformio.ini`：开发板、框架、依赖和串口配置。
- `host/`：Python 手柄调试程序。
- `docs/reference/`：电机协议参考资料。
- `firmware/.clangd`、`.vscode/`、`.zed/`：编辑器及语言服务配置。

## 常用命令

- `pio run -d firmware`：编译固件。
- `pio run -d firmware -t upload`：编译并烧录固件。
- `pio device monitor -d firmware`：打开串口监视器。
- `pio run -d firmware -t compiledb`：更新 clangd 使用的编译数据库。
- `pio run -d firmware -t clean`：清理固件构建产物。
- `uv sync --project host`：安装 Python 上位机依赖。
- `uv run --project host host/test_main.py`：运行 Python 上位机测试。

提交改动前至少运行一次 `pio run -d firmware`。修改 Python 上位机时还需运行对应测试。不要提交 `.pio/` 和 `compile_commands.json` 等生成文件。

## Agent skills

### Issue tracker

任务与需求使用 GitHub Issues；外部 PR 不作为需求分类入口。详见 `docs/agents/issue-tracker.md`。

### Triage labels

使用 `needs-triage`、`needs-info`、`ready-for-agent`、`ready-for-human`、`wontfix` 五种分类标签。详见 `docs/agents/triage-labels.md`。

### Domain docs

采用单一领域上下文，使用根目录 `CONTEXT.md` 和 `docs/adr/`。详见 `docs/agents/domain.md`。
