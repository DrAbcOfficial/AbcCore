# AGENTS.md

Guidance for AI/automation collaborators working in this repository. Read `README.md` first - it is the only document distributed with the release.

## Hard rules

- Nothing committed (code, comments, docs, commit messages, tags) may contain local machine paths, personal directories, session logs or any credentials; the game directory is always provided through the `ABC_GAME_DIR` environment variable.
- Do not commit test scaffolding, acceptance scripts, evidence screenshots, plans or progress logs; those are session artifacts that do not belong in the release.
- The README is the single public document; interface or behaviour changes must update it in the same commit.
- Submodule pins may only point at commits that are already public upstream; confirm the commit is pushed before bumping a pin.

## Build

Win32 x86 only / C++20 / dynamic CRT (/MD):

```bash
cmake --preset win32-debug
cmake --build --preset win32-debug    # Release: win32-release
```

## Conventions

- Public headers expose their contract types (`AIGCJson` for `Config`,
  eventpp for `Events`, SteamSDK for `SteamId`); this is intentional. New
  headers still include the standard headers they use directly.
- C-style casts remain in ABCEnchance-derived code; new code uses named casts.

- Legacy public naming: UpperCamelCase functions, `C`-prefixed types and `m_`
  members are kept for ABCEnchance source compatibility. New code uses
  lowerCamel functions/variables and k-prefixed constants; do not mass-rename
  the existing surface.

- Naming follows the LLVM style; public APIs live in `include/abc/`, implementations in `src/`, and public headers must be self-contained.
- AbcCore stays renderer-free: do not add Renderer headers, submodules, include paths or build checks here. `IMetaRenderer` glue belongs to AbcUi under `src/render/`.
- `cmake/gamedata/` is the self-contained gamedata tooling used by `abc_add_gamedata`; keep it independent of plugin checkouts.
- Commit messages use `type(scope): summary` (feat / fix / refactor / docs / build / chore) with motivation and verification in the body.
- Before handing off: no local paths or credentials; README matches actual behaviour; Debug and Release both build.
