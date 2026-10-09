#pragma once

// abc::Config - JSON config persistence framework.
// Framework half of ABCEnchance src/core/module/config.*: load/save any
// AIGC-json-annotated struct through the engine IFileSystem under
// "<mod dir>/<plugin>/config.json". The concrete schema (favmodels,
// concurrent_cmd, events_cmd, ...) is domain data and lives with AbcEssential.

#ifndef ABC_CONFIG_H
#define ABC_CONFIG_H

#include <AIGCJson.hpp>

#include <string>

#include <abc/Console.h>

namespace abc {

// Raw JSON text under "<mod dir>/<relativePath>".
bool ReadJsonFile(const char* relativePath, std::string& jsonOut);
bool WriteJsonFile(const char* relativePath, const std::string& json);

// Typed helpers for AIGC_JSON_HELPER structs. Missing file is not an error
// (first run); parse failures warn through abc::ConsoleWarn and return false.
// JsonSave takes a non-const reference because AIGCJson's serializer API
// (ObjectToJson) is non-const.
template <typename T>
bool JsonLoad(T& cfg, const char* relativePath) {
	std::string json;
	if (!ReadJsonFile(relativePath, json))
		return false;
	if (!aigc::JsonHelper::JsonToObject(cfg, json)) {
		ConsoleWarn("Couldn't parse config file %s\n", relativePath);
		return false;
	}
	return true;
}

template <typename T>
bool JsonSave(T& cfg, const char* relativePath) {
	std::string json;
	if (!aigc::JsonHelper::ObjectToJson(cfg, json)) {
		ConsoleWarn("Couldn't serialize config for %s\n", relativePath);
		return false;
	}
	return WriteJsonFile(relativePath, json);
}

}

#endif //ABC_CONFIG_H
