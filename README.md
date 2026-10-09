# AbcCore

Sven Co-op / MetaHookSv 插件群共享基础库（静态库 `abc_core`），AbcEssential 与 AbcUi 的公共依赖。

## 构建

Windows + MSVC（默认 VS 2026 预设；手选生成器时要求 CMake ≥ 3.24，预设要求 ≥ 4.2），仅 Win32 x86，C++20，静态 CRT：

```bash
git clone --recursive https://github.com/DrAbcOfficial/AbcCore.git
cmake --preset win32-debug
cmake --build --preset win32-debug   # Release：win32-release
```

`ABC_WITH_HTTP` 默认 OFF；HTTP 运行时未锁定前，配置阶段会拒绝开启。

## 模块（include/abc/）

| 头文件 | 内容 |
|---|---|
| `Plugin.h` | `CAbcPluginBase`（IPluginsV4 骨架、引擎侧检查与信息）；metahook.h 四个全局符号（每 DLL 一份） |
| `Format.h` / `NetMsg.h` | 位置占位符安全格式化（非 RML/命令转义）；严格 NUL/边界消息读取 |
| `Console.h` | `ConsolePrint/Warn/Error/Writeline`（经 `gEngfuncs.Con_Printf`） |
| `CvarCmd.h` | `CREATE_CVAR`、`ABC_HOOK_COMMAND`、`ServerCmd`、`abc::SigScan`、`CCvarRegistrar` |
| `Events.h` | eventpp 词汇、`CChainedCommand`（钩命令→订阅者投票→链回原命令）、`DispatchToMainThread` |
| `Math.h` | `vec2_t` / `vec3_t` / `mat4` 等 typedef + `abc::CMathlib` |
| `Config.h` | `JsonLoad` / `JsonSave`（注解结构体 ↔ `<mod>/config.json`；读取经 IFileSystem，写入经 Win32 原子替换并保留 .bak） |
| `Task.h` | 后台线程任务 + 主线程续延（宿主逐帧 `abc::GetTaskManager().CheckAll()`） |
| `Paths.h` | `GameDir` / `ModPath` / `EnsureDir` / `EnsureParentDir` / `CheckAsset` |
| `Platform.h` | Win32 杂项（注册表、图标提取） |
| `Render.h` / `RenderCommon.h` | `IMetaRenderer` 桥（Bind / DrawTexturePos / CompileShaderFile）与渲染常量表；零 VGUI2/OpenGL 依赖；`check-rendercommon-sync` 目标校验其常量与 `thirdparty/Renderer` 保持同步 |
| `SteamId.h` | SteamID 解析/渲染（header-only，无 steam_api 依赖） |

`cmake/AbcCommon.cmake` 供各插件仓库以 `add_subdirectory` 共用；`cmake/AbcGameData.cmake` 提供 gamedata 清单的校验/部署辅助。

## 下游使用

```cmake
add_subdirectory(thirdparty/AbcCore)
target_link_libraries(MyPlugin PRIVATE Abc::Core)
```

`abc_core` 以 PUBLIC 传播 MetaHook / Renderer / eventpp / AIGCJson / SteamSDK 的 include 路径；插件 DLL 内不要再定义 `g_pInterface`、`gEngfuncs`、`g_pMetaHookAPI`、`g_pMetaSave`、`gExportfuncs`（`Plugin.cpp` 已各提供一份）。入口写法与导出宏见 `include/abc/Plugin.h`。

## 子模块

| 路径 | 上游 | pin |
|---|---|---|
| `thirdparty/MetaHook` | https://github.com/MetaHookSv/MetaHook | `dbefbebe3124c1840cffd9a096ae06689c8e2380` |
| `thirdparty/Renderer` | https://github.com/MetaHookSv/Renderer | `e2c3c45e20f320abc0e2c392fef4a175a93aa2ab` |
| `thirdparty/eventpp` | https://github.com/wqking/eventpp | `v0.1.3`（`d2db8af2a46c79f8dc75759019fba487948e9442`） |
| `thirdparty/AIGCJson` | https://github.com/AIGMix/AIGCJson | `d67ea37e54c47e6b0f65a4d97b564c1b4fd8146c` |
| `thirdparty/SteamSDK` | https://github.com/MetaHookSv/SteamSDK | `3c1abaf6277f9f99fd16ef40557d6852820b848f` |
