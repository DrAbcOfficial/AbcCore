# AbcCore

Shared foundation library (`AbcCore.dll`) for the Sven Co-op / MetaHookSv plugin family; the common dependency of AbcEssential and AbcUi.

## Build

Windows + MSVC, CMake >= 3.24; the shipped presets declare CMake 4.2 and target the "Visual Studio 18 2026" generator, Win32 x86 only, C++20, dynamic CRT (/MD). The target is a DLL named `AbcCore.dll`:

```bash
git clone --recursive https://github.com/DrAbcOfficial/AbcCore.git
cmake --preset win32-debug
cmake --build --preset win32-debug   # Release: win32-release
```

`ABC_WITH_HTTP` defaults to OFF; the configure step rejects enabling it while the HTTP runtime stays unpinned.

## Modules (include/abc/)

| Header | Contents |
|---|---|
| `Plugin.h` | Inline `CAbcPluginBase` (IPluginsV4 skeleton, engine-side checks and info) and the setters that copy host state into AbcCore.dll. Each plugin still defines the five metahook globals in its own `src/CoreGlobals.cpp`. |
| `Format.h` / `NetMsg.h` | Type-safe positional-placeholder formatting (not an RML/command escaper); strict NUL/boundary message reads |
| `AbcCoreInterface.h` | Valve-style C ABI: `AbcCore_CreateInterface` returns the versioned `AbcCoreApi` function table (interface version `AbcCore_API_002`) |
| `Parse.h` | Strict, locale-independent `ParseNumber` / `ParseInt` / `ParseFloat` (whole-view consumption, `std::optional` result, no exceptions) |
| `Console.h` | `ConsolePrint/Warn/Error/Writeline` (through `gEngfuncs.Con_Printf`) |
| `CvarCmd.h` | `CREATE_CVAR`, `ADD_COMMAND`, `ServerCmd` / `EngineClientCmd`, `abc::SigScan`, `CCvarRegistrar` |
| `Events.h` | eventpp vocabulary, `CChainedCommand` (hook a command, let subscribers vote, chain back to the original), `DispatchToMainThread` |
| `Math.h` | `vec2_t` / `vec3_t` / `mat4` typedefs + `abc::CMathlib`, with an `abc::math` forwarding namespace; `k`-prefixed constants (`kPi`, `kDegToRad`, ...) with deprecated legacy spellings; `AngleVectorsTranspose` returns the transposed basis |
| `Config.h` | `JsonLoad` / `JsonSave` (annotated struct <-> `<mod>/config.json`; reads through IFileSystem, writes through Win32 atomic replace keeping a `.bak`) |
| `Task.h` | Background worker tasks + main-thread continuations (the host calls `abc::GetTaskManager().CheckAll()` every frame) |
| `Paths.h` | `GameDir` / `ModPath` / `EnsureDir` / `EnsureParentDir` / `CheckAsset` |
| `Platform.h` | Win32 odds and ends (registry, icon extraction) |
| `SteamId.h` | SteamID parsing/rendering (no steam_api dependency; the pinned SDK's out-of-line CSteamID methods live in `src/detail/SteamIdCompat.cpp`) |

`Math.h` keeps the HLSDK vocabulary. `AngleVectors`, `AngleVectorsTranspose`,
`AngleQuaternion` and `Matrix4x4_CreateFromEntity` take engine-order
`[pitch, yaw, roll]` degrees; `AngleMatrix` / `AngleIMatrix` keep the legacy
GoldSrc `[roll, pitch, yaw]` mapping. New code should use the canonical helper
names (`VectorMultiply`, `ColorApplyAlpha`, `PointInQuad`, `AngleMod`, `IsNaN`,
`DegToRad`, `RadToDeg`, `Log2Floor`, `RoundHalfUp`, `Vector2Copy`, `ResetBounds`,
`ScalePixelLength`); the legacy spellings remain as forwards. New code may
also use the `abc::math` namespace (`abc::math::VectorCopy`,
`abc::math::kPi`), which forwards to the same helpers; `abc::CMathlib` remains
the compatibility surface. The reserved `_Vector*` aliases and the unused
`fixed*_t` / angle constants were removed.

`cmake/AbcCommon.cmake` is shared with the plugin repositories through `add_subdirectory`; `cmake/AbcGameData.cmake` provides the gamedata manifest sync/validation helpers using the bundled Python tooling in `cmake/gamedata/`.

AbcCore is renderer-free. The `IMetaRenderer` bridge and the renderer constant tables live in AbcUi, which owns the direct Renderer dependency.

## Dynamic interface

`AbcCore.dll` exports a Valve-style, versioned C ABI factory:

```cpp
const AbcCoreApi *AbcCore_CreateInterface(const char *name, int *returnCode);
```

`AbcCoreApi` (in `include/abc/AbcCoreInterface.h`) is a plain function table:
`size`, `version`, core version, plugin name, and the host-context setters. The
table is append-only within an interface version, so plugin binaries do not
depend on C++ class layout, STL types or the MSVC C++ ABI. `CAbcPluginBase`
resolves the table once and uses it for the lifecycle calls.

`WINDOWS_EXPORT_ALL_SYMBOLS` is OFF: AbcCore.dll exports only the C ABI
factory. Task/Events/Math/Config are header-only per plugin; Console/Cvar/Paths
and lifecycle calls go through `AbcCoreApi`. New cross-DLL surface must be
appended to the C table.

## Using AbcCore downstream

```cmake
add_subdirectory(thirdparty/AbcCore)
target_link_libraries(MyPlugin PRIVATE Abc::Core)
```

`abc_core` propagates the MetaHook / eventpp / AIGCJson / SteamSDK include paths
as PUBLIC. The plugin DLL defines the five metahook globals and its local
`abc::g_*` copies in `src/CoreGlobals.cpp`; `CAbcPluginBase` refreshes them and
forwards the host state to AbcCore.dll through the C ABI. At runtime place `AbcCore.dll` where the loader resolves
it (the aggregate package uses `svencoop/metahook/dlls/AbcCore/`). See
`include/abc/Plugin.h` for the entry-point pattern and the export macro.

## Submodules

| Path | Upstream | Pin |
|---|---|---|
| `thirdparty/MetaHook` | https://github.com/MetaHookSv/MetaHook | `dbefbebe3124c1840cffd9a096ae06689c8e2380` |
| `thirdparty/eventpp` | https://github.com/wqking/eventpp | `v0.1.3` (`d2db8af2a46c79f8dc75759019fba487948e9442`) |
| `thirdparty/AIGCJson` | https://github.com/AIGMix/AIGCJson | `d67ea37e54c47e6b0f65a4d97b564c1b4fd8146c` |
| `thirdparty/SteamSDK` | https://github.com/MetaHookSv/SteamSDK | `3c1abaf6277f9f99fd16ef40557d6852820b848f` |

## License

Licensed under the [BSD 3-Clause License](LICENSE); each dependency keeps its own license.
