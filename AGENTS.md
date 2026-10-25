# AGENTS.md

Guidance for AI/automation collaborators working in this repository. Read `README.md` first - it is the only document distributed with the release.

## Hard rules

- Nothing committed (code, comments, docs, commit messages, tags) may contain local machine paths, personal directories, session logs or any credentials; when a deployment destination is requested it is provided through the `ABC_GAME_DIR` environment variable, never baked into the tree.
- Do not commit test scaffolding, acceptance scripts, evidence screenshots, plans or progress logs; those are session artifacts that do not belong in the release.
- The README is the single public document; interface or behaviour changes must update it in the same commit.
- Submodule pins may only point at commits that are already public upstream; confirm the commit is pushed before bumping a pin.

## README

- The README is the single public document and is written for end users.
- Keep it short: features, installation and user-facing commands or settings
  only. Do not document architecture, module layout, ABI or interface
  contracts, design conventions or internal implementation, and never add
  parenthesised technical remarks.
- Do not describe changelogs, TODOs or FIXMEs.
- The only technical content is how to build the dist package, including its
  prerequisites.

## Build

Win32 x86 only / C++20 / dynamic CRT (/MD); the target is `AbcCore.dll`
(`OUTPUT_NAME AbcCore`, `WINDOWS_EXPORT_ALL_SYMBOLS OFF`, one C ABI export). `ABC_GAME_DIR` is
optional: unset means the gamedata catalog is still generated under
`PROJECT_BINARY_DIR/assets/...` and `install` uses its relative destination
instead of a game root.

```bash
cmake --preset win32-debug
cmake --build --preset win32-debug    # Release: win32-release
```

## Conventions

- Public headers expose their contract types (`AIGCJson` for `Config`,
  eventpp for `Events`, SteamSDK for `SteamId`); this is intentional. New
  headers still include the standard headers they use directly.
- AbcCore is a shared library. The plugin-facing surface is the versioned C ABI
  table in `include/abc/AbcCoreInterface.h`, reached through
  `AbcCore_CreateInterface`; append fields and bump the interface version.
  `WINDOWS_EXPORT_ALL_SYMBOLS` stays OFF - do not add exported C++ symbols.
- Plugins define the five metahook globals and their local `abc::g_*` copies in
  their own `src/CoreGlobals.cpp`; `CAbcPluginBase` refreshes them and forwards
  host state to AbcCore.dll.
- Formatting follows LLVM (2 spaces, 80 columns, attached braces). Identifier
  casing follows the repository conventions: public API functions are
  UpperCamelCase, types are `C`-prefixed or UpperCamelCase, legacy members keep
  `m_`, implementation-local helpers use lowerCamelCase and constants use the
  `k` prefix. Do not mass-rename the existing surface.
- C-style casts remain in ABCEnchance-derived code; new code uses named casts.
- Public APIs live in `include/abc/`, implementations in `src/`, and public
  headers must be self-contained.
- AbcCore stays renderer-free: no Renderer headers, submodule, include path or
  link dependency. Gamedata manifests may name Renderer symbols as data;
  `IMetaRenderer` glue still belongs to AbcUi under `src/render/`.
- `cmake/gamedata/` is the self-contained gamedata tooling used by `abc_add_gamedata`; keep it independent of plugin checkouts.
- Commit messages use `type(scope): summary` (feat / fix / refactor / docs / build / chore) with motivation and verification in the body.
- Before handing off: no local paths or credentials; README matches actual behaviour; Debug and Release both build.

## Contract notes

- The C ABI table is the only DLL surface. Every consumer-side wrapper in
  `include/abc/` must degrade when `AbcCoreApiTable()` is null (a machine with
  an older `AbcCore.dll`): return the documented neutral value instead of
  dereferencing the table. `CAbcPluginBase::Init` reports the mismatch once
  through a message box. New wrappers follow the same pattern; do not mix
  checked and unchecked access in one header.
- `SteamId.h` is header-only on purpose: its helpers call the header-only SDK
  and are not part of the ABI table, so a DLL-side implementation would be an
  unresolved external for consumers. Keep `TryParseSteamID`/`ParseSteamID`/
  `RenderSteamID` inline; only the SDK's out-of-line `CSteamID` methods stay in
  `src/detail/SteamIdCompat.cpp` for AbcCore.dll itself.
- `GameDir()` caches the engine mod directory under a mutex because
  `abc::Task` exposes worker threads to `ModPath`/`EnsureDir`/`JsonSave`. Once
  resolved the string is never reassigned; before that, callers get a literal.
- Config persistence bounds JSON nesting (`detail::MaxJsonNesting`) before
  rapidjson's recursive descent, writes atomically with
  `REPLACEFILE_WRITE_THROUGH`, and sweeps crash-leftover
  `<file>.tmp.<pid>.<seq>` entries older than an hour on the next save.
- `FloorDivMod` defines a zero result for a zero/NaN denominator (the HLSDK
  original had undefined int-cast behavior). Keep the guard when porting more
  math helpers.
- `CTaskItem<void>` is a compile-time error; fire-and-forget work uses
  `CTaskManager::Post`.
