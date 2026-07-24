# 在 VS Code 中配置 LSP

## 1. 安装扩展

用 VS Code 打开项目根目录，并安装工作区推荐的扩展：

- clangd
- PlatformIO IDE

项目配置会关闭 Microsoft C/C++ 扩展的 IntelliSense，避免两个语言服务重复诊断。

## 2. macOS

安装 PlatformIO 和 LLVM：

```shell
brew install platformio llvm
export PATH="$(brew --prefix llvm)/bin:$PATH"
clangd --version
pio run -t compiledb
code .
```

Homebrew 的 LLVM 不会覆盖系统 clangd，因此需要从设置过 `PATH` 的终端启动 VS Code。

## 3. Windows

使用 PowerShell 安装 LLVM：

```powershell
winget install LLVM.LLVM
```

安装完成后重新打开 PowerShell，并确认 clangd 可用：

```powershell
clangd --version
```

如果命令无法执行，将 `C:\Program Files\LLVM\bin` 加入系统 `Path`。随后在 VS Code 命令面板中运行 `PlatformIO: New Terminal`，执行：

```powershell
pio run -t compiledb
code .
```

PlatformIO IDE 负责安装和管理 Windows 下的 PlatformIO Core。

## 4. 启动 LSP

`pio run -t compiledb` 会生成 `compile_commands.json`。在 VS Code 命令面板中运行 `clangd: Restart language server`，代码补全、跳转和诊断即可生效。

修改 `platformio.ini`、增加依赖或新增源文件后，重新运行 `pio run -t compiledb` 并重启 clangd。
