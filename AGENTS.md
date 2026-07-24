# Repository Guidelines

## 项目说明

本仓库是用于全国大学生电子设计竞赛的 ESP32 开发模板，基于 Arduino 框架和 PlatformIO。

## 项目结构

- `src/main.cpp`：程序入口，包含 `setup()` 和 `loop()`。
- `platformio.ini`：开发板、框架、依赖和串口配置。
- `.clangd`、`.vscode/`、`.zed/`：编辑器及语言服务配置。

## 常用命令

- `pio run`：编译项目。
- `pio run -t upload`：编译并烧录固件。
- `pio device monitor`：打开串口监视器。
- `pio run -t compiledb`：更新 clangd 使用的编译数据库。
- `pio run -t clean`：清理构建产物。

提交改动前至少运行一次 `pio run`。不要提交 `.pio/` 和 `compile_commands.json` 等生成文件。
