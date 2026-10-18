#pragma once

// abc::Paths - single entry point for mod-dir path resolution.
// Collects the scattered gamedir concatenation logic from ABCEnchance
// src/plugins.cpp (CheckAsset) and config paths. All paths are mod-relative
// ("svencoop/" working dir): "abcenchance/config.json" etc.
//
// NOTE: the engine returns the mod directory RELATIVE ("svencoop", see plan 08
// §4.11), so GameDir()/ModPath() resolve against the process CWD; the host
// game must be launched with the game root as CWD (the fixed launch in 08 §3).

#include <abc/AbcCoreInterface.h>

#include <string>

namespace abc {

#ifdef ABC_CORE_EXPORTS
// Engine mod directory (g_pMetaHookAPI->GetGameDirectory), cached.
const char *GameDir();
// Trusted local paths only (not server/user input): absolute paths/.. are
// not sandboxed by these general helpers. Config separately restricts paths.
// "<mod dir>/<relativePath>" with native separators; empty until root ready.
std::string ModPath(const char *relativePath);
// Creates "<mod dir>/<relativeDir>" recursively; true when it exists after.
bool EnsureDir(const char *relativeDir);
// Creates the parent directory of a file path ("<a>/<b>/file" -> "<a>/<b>").
bool EnsureParentDir(const char *relativeFilePath);
// True when the engine filesystem can see "<relativePath>"; warns when missing.
bool CheckAsset(const char *relativePath);
#else
inline const char *GameDir() {
  return AbcCoreApiTable()->GameDir();
}
inline std::string ModPath(const char *relativePath) {
  const AbcCoreApi *api = AbcCoreApiTable();
  const unsigned long long size = api->ModPath(relativePath, nullptr, 0);
  std::string out(static_cast<std::size_t>(size), '\0');
  if (size != 0)
    api->ModPath(relativePath, out.data(), size + 1);
  return out;
}
inline bool EnsureDir(const char *relativeDir) {
  return AbcCoreApiTable()->EnsureDir(relativeDir) != 0;
}
inline bool EnsureParentDir(const char *relativeFilePath) {
  return AbcCoreApiTable()->EnsureParentDir(relativeFilePath) != 0;
}
inline bool CheckAsset(const char *relativePath) {
  return AbcCoreApiTable()->CheckAsset(relativePath) != 0;
}
#endif

} // namespace abc
