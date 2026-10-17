# AbcCore

Shared foundation library (`AbcCore.dll`) for the Sven Co-op / MetaHookSv plugin family; the common dependency of AbcEssential and AbcUi.

## Build

Windows + MSVC, CMake >= 4.2 (the default presets target the "Visual Studio 18 2026" generator), Win32 x86 only, C++20, dynamic CRT (`/MD`). The build produces `AbcCore.dll` plus its `AbcCore.lib` import library:

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
| `SteamId.h` | SteamID parsing/rendering (header-only, no steam_api dependency) |

`cmake/AbcCommon.cmake` is shared with the plugin repositories through `add_subdirectory`; `cmake/AbcGameData.cmake` provides the gamedata manifest sync/validation helpers using the bundled Python tooling in `cmake/gamedata/`.

AbcCore is renderer-free. The `IMetaRenderer` bridge and the renderer constant tables live in AbcUi, which owns the direct Renderer dependency.

## Using AbcCore downstream

```cmake
add_subdirectory(thirdparty/AbcCore)
target_link_libraries(MyPlugin PRIVATE Abc::Core)
```

`abc_core` propagates the MetaHook / eventpp / AIGCJson / SteamSDK include paths as PUBLIC. Deploy `AbcCore.dll` to `<game>/svencoop/metahook/dlls/AbcCore/`; MetaHook adds every `metahook/dlls/*` subdirectory to `PATH`, so the plugin import tables resolve it at load time.

Each plugin defines its own MetaHook globals in `src/CoreGlobals.cpp`. The inline `CAbcPluginBase` copies the host pointers into AbcCore.dll through the exported context setters, so both plugin code and the shared core see the same engine state. See `include/abc/Plugin.h` for the entry-point pattern.

## Submodules

| Path | Upstream | Pin |
|---|---|---|
| `thirdparty/MetaHook` | https://github.com/MetaHookSv/MetaHook | `dbefbebe3124c1840cffd9a096ae06689c8e2380` |
| `thirdparty/eventpp` | https://github.com/wqking/eventpp | `v0.1.3` (`d2db8af2a46c79f8dc75759019fba487948e9442`) |
| `thirdparty/AIGCJson` | https://github.com/AIGMix/AIGCJson | `d67ea37e54c47e6b0f65a4d97b564c1b4fd8146c` |
| `thirdparty/SteamSDK` | https://github.com/MetaHookSv/SteamSDK | `3c1abaf6277f9f99fd16ef40557d6852820b848f` |

## License

Licensed under the [BSD 3-Clause License](LICENSE); each dependency keeps its own license.
