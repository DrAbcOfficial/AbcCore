// Ported from ABCEnchance src/plugins.h/.cpp (skeleton half; feature hook
// installation stays with the feature plugins).
#include <metahook.h>

#include <abc/Plugin.h>

#include <cstring>
#include <cstdarg>

// per-DLL instance (static lib): the globals metahook.h declares extern.
// Defined here so exactly one copy exists per plugin DLL.
mh_interface_t* g_pInterface = nullptr;
cl_enginefunc_t gEngfuncs = {};
metahook_api_t* g_pMetaHookAPI = nullptr;
mh_enginesave_t* g_pMetaSave = nullptr;
cl_exportfuncs_t gExportfuncs = {};

// IPluginsV4 declares its virtuals without definitions (each plugin was
// expected to implement them directly in the old layout). Now that
// CAbcPluginBase derives and overrides them all, MSVC still emits references
// to the base entries while building construction/destruction vtables.
void IPluginsV4::Init(metahook_api_t*, mh_interface_t*, mh_enginesave_t*) {}
void IPluginsV4::Shutdown(void) {}
void IPluginsV4::LoadEngine(cl_enginefunc_t*) {}
void IPluginsV4::LoadClient(cl_exportfuncs_t*) {}
void IPluginsV4::ExitGame(int) {}
const char* IPluginsV4::GetVersion(void) { return ""; }

namespace abc {

IFileSystem* g_pFileSystem = nullptr;
DWORD g_dwEngineBuildnum = 0;
int g_iEngineType = 0;
PVOID g_dwEngineBase = nullptr;
DWORD g_dwEngineSize = 0;
PVOID g_dwEngineTextBase = nullptr;
DWORD g_dwEngineTextSize = 0;
PVOID g_dwEngineDataBase = nullptr;
DWORD g_dwEngineDataSize = 0;
PVOID g_dwEngineRdataBase = nullptr;
DWORD g_dwEngineRdataSize = 0;
PVOID g_dwClientBase = nullptr;
DWORD g_dwClientSize = 0;

static char s_szPluginName[64] = "abc";

void SetPluginName(const char* name) {
	_snprintf_s(s_szPluginName, sizeof(s_szPluginName), _TRUNCATE, "%s", name);
}

const char* PluginName() {
	return s_szPluginName;
}

void SysError(const char* fmt, ...) {
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	_vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, args);
	va_end(args);
	g_pMetaHookAPI->SysError("[%s]: %s", s_szPluginName, buf);
}

CAbcPluginBase::CAbcPluginBase(const char* pluginName, const char* version) {
	_snprintf_s(m_szName, sizeof(m_szName), _TRUNCATE, "%s", pluginName);
	_snprintf_s(m_szVersion, sizeof(m_szVersion), _TRUNCATE, "%s|Version:%s", pluginName, version);
	SetPluginName(pluginName);
}

void CAbcPluginBase::Init(metahook_api_t* pAPI, mh_interface_t* pInterface, mh_enginesave_t* pSave) {
	g_pInterface = pInterface;
	g_pMetaHookAPI = pAPI;
	g_pMetaSave = pSave;
	OnInit();
}

void CAbcPluginBase::LoadEngine(cl_enginefunc_t* pEngineFuncs) {
	g_pFileSystem = g_pInterface->FileSystem;
	g_iEngineType = g_pMetaHookAPI->GetEngineType();
	g_dwEngineBuildnum = g_pMetaHookAPI->GetEngineBuildnum();
	g_dwEngineBase = g_pMetaHookAPI->GetEngineBase();
	g_dwEngineSize = g_pMetaHookAPI->GetEngineSize();
	g_dwEngineTextBase = (PVOID)g_pMetaHookAPI->GetSectionByName(g_dwEngineBase, ".text\x0\x0\x0", &g_dwEngineTextSize);
	g_dwEngineDataBase = (PVOID)g_pMetaHookAPI->GetSectionByName(g_dwEngineBase, ".data\x0\x0\x0", &g_dwEngineDataSize);
	g_dwEngineRdataBase = (PVOID)g_pMetaHookAPI->GetSectionByName(g_dwEngineBase, ".rdata\x0\x0", &g_dwEngineRdataSize);

	memcpy(&gEngfuncs, pEngineFuncs, sizeof(gEngfuncs));

	if (g_iEngineType != ENGINE_SVENGINE) {
		SysError("%s can only run in SvEngine!\nEngine type: %d\nEngine buildnum: %d",
			m_szName, g_iEngineType, g_dwEngineBuildnum);
		return;
	}

	OnLoadEngine();
}

void CAbcPluginBase::LoadClient(cl_exportfuncs_t* pExportFuncs) {
	memcpy(&gExportfuncs, pExportFuncs, sizeof(gExportfuncs));

	g_dwClientBase = g_pMetaHookAPI->GetClientBase();
	g_dwClientSize = g_pMetaHookAPI->GetClientSize();

	OnLoadClient(pExportFuncs);
}

void CAbcPluginBase::ExitGame(int iResult) {
	OnExitGame(iResult);
}

void CAbcPluginBase::Shutdown() {
	OnShutdown();
}

const char* CAbcPluginBase::GetVersion() {
	return m_szVersion;
}

const char* CAbcPluginBase::Version() const {
	return m_szVersion;
}

}
