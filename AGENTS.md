# AGENTS.md

面向在本仓库中工作的 AI/自动化协作者。开始前先读 `README.md`（本仓库唯一随发布分发的说明文档）。

## 硬性约束

- 提交内容（代码、注释、文档、提交信息、标签）不得出现本机路径、个人目录、会话记录或任何凭证；游戏目录一律通过 `ABC_GAME_DIR` 环境变量提供。
- 不提交测试脚手架、验收脚本、证据截图、计划与进度记录等会话产物。
- README 是唯一对外文档；接口或行为变化必须同步更新 README。
- 子模块 pin 只指向对应上游已公开的提交；升级 pin 前先确认该提交已推送。

## 构建

仅 Win32 x86 / C++20 / 静态 CRT：

```bash
cmake --preset win32-debug
cmake --build --preset win32-debug    # Release：win32-release
```

## 约定

- 命名遵循 LLVM 风格；公共 API 放 `include/abc/`，实现放 `src/`，公共头文件必须可单独包含。
- `RenderCommon.h` 常量与 `thirdparty/Renderer` 逐字同步，由 `check-rendercommon-sync` 构建目标把关。
- 提交信息使用 `type(scope): 摘要`（feat / fix / refactor / docs / build / chore），正文说明动机与验证方式。
- 交付前自查：无本机路径与凭证；README 与实际行为一致；Debug 与 Release 均可构建。
