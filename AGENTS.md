# AGENTS.md

Guidance for AI/automation collaborators working in this repository. Read `README.md` first - it is the only document distributed with the release.

## Hard rules

- Nothing committed (code, comments, docs, commit messages, tags) may contain local machine paths, personal directories, session logs or any credentials; when a deployment destination is requested it is provided through the `ABC_GAME_DIR` environment variable, never baked into the tree.
- Do not commit test scaffolding, acceptance scripts, evidence screenshots, plans or progress logs; those are session artifacts that do not belong in the release.
- The README is the single public document; interface or behaviour changes must update it in the same commit.
- Submodule pins may only point at commits that are already public upstream; confirm the commit is pushed before bumping a pin.

## Build

Win32 x86 only / C++20 / dynamic CRT (/MD); the target is `AbcCore.dll`
(`OUTPUT_NAME AbcCore`, `WINDOWS_EXPORT_ALL_SYMBOLS`). `ABC_GAME_DIR` is
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
- AbcCore is a shared library. The stable plugin-facing surface is the
  versioned C ABI table in `include/abc/AbcCoreInterface.h`, reached through
  `AbcCore_CreateInterface`; append fields instead of changing existing ones.
  The legacy C++ symbols still exported by `WINDOWS_EXPORT_ALL_SYMBOLS` are
  for in-tree consumers and must not be treated as a new ABI promise.
- Plugins define the five metahook globals in their own `src/CoreGlobals.cpp`;
  `CAbcPluginBase` copies them into AbcCore.dll via the interface setters.
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
