#include <abc/Paths.h>
#include <abc/Console.h>
#include <abc/Plugin.h>

#include <filesystem>

namespace abc {

static std::string CachedGameDir;
static bool WarnedNoGameDir = false;

const char* GameDir() {
	if (CachedGameDir.empty()) {
		const char* dir = g_pMetaHookAPI ? g_pMetaHookAPI->GetGameDirectory() : nullptr;
		if (dir && *dir)
			CachedGameDir = dir;
		else if (!WarnedNoGameDir) {
			// Paths will silently degrade to process-CWD-relative until the
			// engine dir is available; usually means "called too early".
			WarnedNoGameDir = true;
			ConsoleWarn("GameDir not available yet, paths stay CWD-relative\n");
		}
	}
	return CachedGameDir.c_str();
}

std::string ModPath(const char* relativePath) {
	std::filesystem::path p = std::filesystem::path(GameDir()) / std::filesystem::u8path(relativePath);
	return p.string();
}

bool EnsureDir(const char* relativeDir) {
	std::error_code ec;
	std::filesystem::create_directories(std::filesystem::path(GameDir()) / std::filesystem::u8path(relativeDir), ec);
	if (ec) {
		ConsoleWarn("Couldn't create directory %s (%s)\n", relativeDir, ec.message().c_str());
		return false;
	}
	return true;
}

bool EnsureParentDir(const char* relativeFilePath) {
	std::filesystem::path p = std::filesystem::path(GameDir()) / std::filesystem::u8path(relativeFilePath);
	std::filesystem::path parent = p.parent_path();
	if (parent.empty())
		return true;
	std::error_code ec;
	std::filesystem::create_directories(parent, ec);
	if (ec) {
		ConsoleWarn("Couldn't create directory %s (%s)\n", parent.string().c_str(), ec.message().c_str());
		return false;
	}
	return true;
}

bool CheckAsset(const char* relativePath) {
	if (g_pFileSystem && g_pFileSystem->FileExists(relativePath))
		return true;
	ConsoleWarn("Missing resource files!\nPlease make sure the \"%s\" folder is placed correctly!\n", relativePath);
	return false;
}

}
