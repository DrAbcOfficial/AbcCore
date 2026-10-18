#pragma once

// abc::Config - JSON config persistence framework.
// Framework half of ABCEnchance src/core/module/config.*: load/save any
// AIGC-json-annotated struct through the engine IFileSystem under
// "<mod dir>/<plugin>/config.json". The concrete schema (favmodels,
// concurrent_cmd, events_cmd, ...) is domain data and lives with AbcEssential.

#include <AIGCJson.hpp>

#include <string>
#include <utility>

#include <abc/Console.h>
#include <abc/Paths.h>
#include <abc/Plugin.h>
#include <abc/detail/ConfigFile.h>
#include <abc/detail/PathUtf8.h>

namespace abc {

// Reads are capped at 4 MiB and preserve jsonOut on failure. Saves use a
// flushed sibling file and native Win32 atomic replacement, retaining .bak
// for existing files. Return false on any failure; never truncate the target.
// Relative paths only; call after GameDir is ready.
// Raw JSON text under "<mod dir>/<relativePath>".
enum class JsonLoadStatus { Success, Missing, Invalid, IoError };
JsonLoadStatus ReadJsonFileStatus(const char *relativePath,
                                  std::string &jsonOut);
bool ReadJsonFile(const char *relativePath, std::string &jsonOut);
bool WriteJsonFile(const char *relativePath, const std::string &json);

// Typed helpers for AIGC_JSON_HELPER structs. Missing file is not an error
// (first run); parse failures warn through abc::ConsoleWarn and return false.
// JsonSave takes a non-const reference because AIGCJson's serializer API
// (ObjectToJson) is non-const.
template <typename T>
JsonLoadStatus JsonLoadWithStatus(T &cfg, const char *relativePath) {
  std::string json;
  const auto status = ReadJsonFileStatus(relativePath, json);
  if (status != JsonLoadStatus::Success)
    return status;
  T candidate = cfg;
  if (!aigc::JsonHelper::JsonToObject(candidate, json)) {
    ConsoleWarn("Couldn't parse config file %s\n", relativePath);
    return JsonLoadStatus::Invalid;
  }
  cfg = std::move(candidate);
  return JsonLoadStatus::Success;
}
template <typename T> bool JsonLoad(T &cfg, const char *relativePath) {
  return JsonLoadWithStatus(cfg, relativePath) == JsonLoadStatus::Success;
}

template <typename T> bool JsonSave(T &cfg, const char *relativePath) {
  std::string json;
  if (!aigc::JsonHelper::ObjectToJson(cfg, json)) {
    ConsoleWarn("Couldn't serialize config for %s\n", relativePath);
    return false;
  }
  return WriteJsonFile(relativePath, json);
}

inline JsonLoadStatus ReadJsonFileStatus(const char *relativePath,
                                         std::string &jsonOut) {
  if (!relativePath || !g_pFileSystem)
    return JsonLoadStatus::IoError;
  FileHandle_t file = g_pFileSystem->Open(relativePath, "rb");
  if (!file)
    return g_pFileSystem->FileExists(relativePath) ? JsonLoadStatus::IoError
                                                   : JsonLoadStatus::Missing;
  struct CloseFile {
    FileHandle_t File;
    ~CloseFile() {
      g_pFileSystem->Close(File);
    }
  } close{file};
  const unsigned size = g_pFileSystem->Size(file);
  if (size > detail::MaxConfigBytes)
    return JsonLoadStatus::Invalid;
  std::string text(size, '\0');
  const int read =
      size ? g_pFileSystem->Read(text.data(), static_cast<int>(size), file) : 0;
  if (read != static_cast<int>(size) || !g_pFileSystem->IsOk(file))
    return JsonLoadStatus::IoError;
  jsonOut = std::move(text);
  return JsonLoadStatus::Success;
}
inline bool ReadJsonFile(const char *relativePath, std::string &jsonOut) {
  return ReadJsonFileStatus(relativePath, jsonOut) == JsonLoadStatus::Success;
}
inline bool WriteJsonFile(const char *relativePath, const std::string &json) {
  // VFileSystem009 has void Flush/Close and no rename/replace operation.
  // Persist explicitly in the writable mod directory, never through a pack
  // or alternate read search path. Native sibling replacement preserves old
  // data.
  if (!relativePath || !*GameDir())
    return false;
  const auto relative = detail::PathFromUtf8(relativePath);
  if (relative.empty() || relative.has_root_path())
    return false;
  for (const auto &part : relative)
    if (part == "..")
      return false;
  return detail::WriteConfigAtomic(std::filesystem::path(GameDir()) / relative,
                                   json);
}

} // namespace abc
