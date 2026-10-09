#pragma once

// abc::Paths - single entry point for mod-dir path resolution.
// Collects the scattered gamedir concatenation logic from ABCEnchance
// src/plugins.cpp (CheckAsset) and config paths. All paths are mod-relative
// ("svencoop/" working dir): "abcenchance/config.json" etc.

#ifndef ABC_PATHS_H
#define ABC_PATHS_H

#include <string>

namespace abc {

// Engine mod directory (g_pMetaHookAPI->GetGameDirectory), cached.
const char* GameDir();
// "<mod dir>/<relativePath>" with native separators.
std::string ModPath(const char* relativePath);
// Creates "<mod dir>/<relativeDir>" recursively; true when it exists after.
bool EnsureDir(const char* relativeDir);
// Creates the parent directory of a file path ("<a>/<b>/file" -> "<a>/<b>").
bool EnsureParentDir(const char* relativeFilePath);
// True when the engine filesystem can see "<relativePath>"; warns when missing.
bool CheckAsset(const char* relativePath);

}

#endif //ABC_PATHS_H
