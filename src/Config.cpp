// Ported from ABCEnchance src/core/module/config.cpp. Dependency replacements
// per plan 03 §2: vgui::filesystem() -> g_pInterface->FileSystem (engine
// IFileSystem), ConsoleWarnMessage -> abc::ConsoleWarn.
#include <abc/Config.h>
#include <abc/Console.h>
#include <abc/Paths.h>
#include <abc/Plugin.h>

namespace abc {

bool ReadJsonFile(const char* relativePath, std::string& jsonOut) {
	jsonOut.clear();
	if (!g_pFileSystem) {
		ConsoleWarn("Config filesystem not ready (%s)\n", relativePath);
		return false;
	}
	FileHandle_t file = g_pFileSystem->Open(relativePath, "rb");
	if (!file)
		return false;
	unsigned size = g_pFileSystem->Size(file);
	char* mem = new char[size + 1];
	int bytesRead = g_pFileSystem->Read(mem, (int)size, file);
	g_pFileSystem->Close(file);
	mem[bytesRead] = 0;
	jsonOut.assign(mem, (size_t)bytesRead);
	delete[] mem;
	return true;
}

bool WriteJsonFile(const char* relativePath, const std::string& json) {
	if (!g_pFileSystem) {
		ConsoleWarn("Config filesystem not ready (%s)\n", relativePath);
		return false;
	}
	EnsureParentDir(relativePath);
	FileHandle_t file = g_pFileSystem->Open(relativePath, "wb");
	if (!file) {
		ConsoleWarn("Couldn't write config file %s\n", relativePath);
		return false;
	}
	g_pFileSystem->Write(json.data(), (int)json.size(), file);
	g_pFileSystem->Close(file);
	return true;
}

}
