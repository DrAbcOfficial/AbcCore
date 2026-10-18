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

} // namespace abc
