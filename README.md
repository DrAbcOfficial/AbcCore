# AbcCore

Shared foundation library (static `abc_core`) for the Sven Co-op / MetaHookSv plugin family; the common dependency of AbcEssential and AbcUi.

## Build

Windows + MSVC, CMake >= 4.2 (the default presets target the "Visual Studio 18 2026" generator), Win32 x86 only, C++20, static CRT:

```bash
git clone --recursive https://github.com/DrAbcOfficial/AbcCore.git
cmake --preset win32-debug
cmake --build --preset win32-debug   # Release: win32-release
```

`ABC_WITH_HTTP` defaults to OFF; the configure step rejects enabling it while the HTTP runtime stays unpinned.

## Modules (include/abc/)

| Header | Contents |
|---|---|
| `Plugin.h` | `CAbcPluginBase` (IPluginsV4 skeleton, engine-side checks and info); the four metahook.h globals (one per DLL) |
| `Format.h` / `NetMsg.h` | Type-safe positional-placeholder formatting (not an RML/command escaper); strict NUL/boundary message reads |
| `Console.h` | `ConsolePrint/Warn/Error/Writeline` (through `gEngfuncs.Con_Printf`) |
| `CvarCmd.h` | `CREATE_CVAR`, `ABC_HOOK_COMMAND`, `ServerCmd`, `abc::SigScan`, `CCvarRegistrar` |
| `Events.h` | eventpp vocabulary, `CChainedCommand` (hook a command, let subscribers vote, chain back to the original), `DispatchToMainThread` |
| `Math.h` | `vec2_t` / `vec3_t` / `mat4` typedefs + `abc::CMathlib` |
| `Config.h` | `JsonLoad` / `JsonSave` (annotated struct <-> `<mod>/config.json`; reads through IFileSystem, writes through Win32 atomic replace keeping a `.bak`) |
| `Task.h` | Background worker tasks + main-thread continuations (the host calls `abc::GetTaskManager().CheckAll()` every frame) |
| `Paths.h` | `GameDir` / `ModPath` / `EnsureDir` / `EnsureParentDir` / `CheckAsset` |
| `Platform.h` | Win32 odds and ends (registry, icon extraction) |
| `Render.h` / `RenderCommon.h` | `IMetaRenderer` bridge (Bind / DrawTexturePos / CompileShaderFile) and the render constant tables; zero VGUI2/OpenGL dependencies; the `check-rendercommon-sync` target verifies the constants stay in sync with `thirdparty/Renderer` |
| `SteamId.h` | SteamID parsing/rendering (header-only, no steam_api dependency) |

`cmake/AbcCommon.cmake` is shared with the plugin repositories through `add_subdirectory`; `cmake/AbcGameData.cmake` provides the gamedata manifest sync/validation helpers.

## Using AbcCore downstream

```cmake
add_subdirectory(thirdparty/AbcCore)
target_link_libraries(MyPlugin PRIVATE Abc::Core)
```

`abc_core` propagates the MetaHook / Renderer / eventpp / AIGCJson / SteamSDK include paths as PUBLIC; plugin DLLs must not define `g_pInterface`, `gEngfuncs`, `g_pMetaHookAPI`, `g_pMetaSave` or `gExportfuncs` again (`Plugin.cpp` already provides one copy of each). See `include/abc/Plugin.h` for the entry-point pattern and the export macro.

## Submodules

| Path | Upstream | Pin |
|---|---|---|
| `thirdparty/MetaHook` | https://github.com/MetaHookSv/MetaHook | `dbefbebe3124c1840cffd9a096ae06689c8e2380` |
| `thirdparty/Renderer` | https://github.com/MetaHookSv/Renderer | `e2c3c45e20f320abc0e2c392fef4a175a93aa2ab` |
| `thirdparty/eventpp` | https://github.com/wqking/eventpp | `v0.1.3` (`d2db8af2a46c79f8dc75759019fba487948e9442`) |
| `thirdparty/AIGCJson` | https://github.com/AIGMix/AIGCJson | `d67ea37e54c47e6b0f65a4d97b564c1b4fd8146c` |
| `thirdparty/SteamSDK` | https://github.com/MetaHookSv/SteamSDK | `3c1abaf6277f9f99fd16ef40557d6852820b848f` |

## License

Licensed under the [BSD 3-Clause License](LICENSE); each dependency keeps its own license.
