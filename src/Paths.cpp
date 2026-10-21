#include <abc/Paths.h>
#include <abc/detail/PathUtf8.h>
#include <abc/Console.h>
#include <abc/Plugin.h>

#include <filesystem>
#include <mutex>

namespace abc {

namespace {

// Task.h allows worker threads to reach ModPath/EnsureDir/JsonSave, so the
// lazy cache is written under a lock. The returned pointer is stable once the
// root is known (the string is never reassigned after that); while the root
// is still unavailable the callers get a string literal instead of a pointer
// into the (still empty) cached string.
std::mutex GameDirMutex;
std::string CachedGameDir;
bool WarnedNoGameDir = false;

} // namespace

const char *GameDir() {
  std::lock_guard<std::mutex> lock(GameDirMutex);
  if (CachedGameDir.empty()) {
    const char *dir =
        g_pMetaHookAPI ? g_pMetaHookAPI->GetGameDirectory() : nullptr;
    if (dir && *dir)
      CachedGameDir = dir;
    else if (!WarnedNoGameDir) {
      // Do not invent a writable CWD-relative root before engine readiness.
      WarnedNoGameDir = true;
      ConsoleWarn(
          "GameDir not available yet, path operations are unavailable\n");
    }
  }
  return CachedGameDir.empty() ? "" : CachedGameDir.c_str();
}

std::string ModPath(const char *relativePath) {
  if (!relativePath || !*GameDir())
    return {};
  std::filesystem::path p =
      std::filesystem::path(GameDir()) / detail::PathFromUtf8(relativePath);
  return p.string();
}

bool EnsureDir(const char *relativeDir) {
  if (!relativeDir || !*GameDir())
    return false;
  std::error_code ec;
  std::filesystem::create_directories(
      std::filesystem::path(GameDir()) / detail::PathFromUtf8(relativeDir), ec);
  if (ec) {
    ConsoleWarn("Couldn't create directory %s (%s)\n", relativeDir,
                ec.message().c_str());
    return false;
  }
  return true;
}

bool EnsureParentDir(const char *relativeFilePath) {
  if (!relativeFilePath || !*GameDir())
    return false;
  std::filesystem::path p =
      std::filesystem::path(GameDir()) / detail::PathFromUtf8(relativeFilePath);
  std::filesystem::path parent = p.parent_path();
  if (parent.empty())
    return true;
  std::error_code ec;
  std::filesystem::create_directories(parent, ec);
  if (ec) {
    ConsoleWarn("Couldn't create directory %s (%s)\n", parent.string().c_str(),
                ec.message().c_str());
    return false;
  }
  return true;
}

bool CheckAsset(const char *relativePath) {
  if (g_pFileSystem && g_pFileSystem->FileExists(relativePath))
    return true;
  ConsoleWarn("Missing resource files!\nPlease make sure the \"%s\" folder is "
              "placed correctly!\n",
              relativePath);
  return false;
}

} // namespace abc
