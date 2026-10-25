# AbcCore

Shared library for the ABC plugins. AbcCore has no interface and no settings of its own: it is installed once and used by [AbcEssential](https://github.com/DrAbcOfficial/AbcEssential) and [AbcUi](https://github.com/DrAbcOfficial/AbcUi).

## Install

The simplest way is the [ABCEnchance](https://github.com/DrAbcOfficial/ABCEnchance) release package, which already contains AbcCore and both plugins.

To install AbcCore on its own, place the file in the Sven Co-op folder:

```text
Sven Co-op/
└── svencoop/
    └── metahook/
        └── dlls/
            └── AbcCore/
                └── AbcCore.dll
```

MetaHookSV is required. AbcCore.dll is a library, not a plugin, so do not add it to `plugins.lst`.

## Build the dist package

Requires Windows, Visual Studio 2026 with the C++ workload and CMake 4.2 or newer.

```bash
git clone --recursive https://github.com/DrAbcOfficial/AbcCore.git
cmake --preset win32-release
cmake --build --preset win32-release
```

This produces `build/win32-release/Release/AbcCore.dll`. AbcCore has no `dist` target of its own: the ready-to-install `AbcCore-<version>.zip` is built by the [ABCEnchance](https://github.com/DrAbcOfficial/ABCEnchance) repository with `--target dist`.

## License

Licensed under the [BSD 3-Clause License](LICENSE).
