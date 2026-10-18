#pragma once

// abc::Plugin - plugin base class + shared engine state.
// Ported from ABCEnchance src/plugins.h/.cpp.
//
// Each plugin DLL defines the metahook.h globals and its own abc::g_* copies
// in src/CoreGlobals.cpp. CAbcPluginBase refreshes those copies and forwards
// the host pointers to AbcCore.dll through the C ABI table, so the DLL's
// utilities observe the same state without exporting C++ data.

#include <abc/AbcCoreInterface.h>
#include <abc/Export.h>

#include <metahook.h>

#include <cstdio>
#include <cstring>

class IFileSystem;

namespace abc {

// Resolve the stable C ABI table once per plugin. The table keeps the
// lifecycle calls independent of the C++ symbol layout.
inline const AbcCoreApi *CoreApi() {
  static const AbcCoreApi *api = [] {
    int returnCode = 0;
    return AbcCore_CreateInterface(ABC_CORE_INTERFACE_VERSION, &returnCode);
  }();
  return api;
}

// Filled in CAbcPluginBase::LoadEngine.
extern IFileSystem *g_pFileSystem;
extern DWORD g_dwEngineBuildnum;
extern int g_iEngineType;
extern PVOID g_dwEngineBase;
extern DWORD g_dwEngineSize;
extern PVOID g_dwEngineTextBase;
extern DWORD g_dwEngineTextSize;
extern PVOID g_dwEngineDataBase;
extern DWORD g_dwEngineDataSize;
extern PVOID g_dwEngineRdataBase;
extern DWORD g_dwEngineRdataSize;
// Filled in CAbcPluginBase::LoadClient.
extern PVOID g_dwClientBase;
extern DWORD g_dwClientSize;

// "[<plugin>]: ..." fatal error via AbcCore.dll's host context.
void SysErrorV(const char *fmt, va_list args);
void SysError(const char *fmt, ...);
void SetPluginName(const char *name);
const char *PluginName();

// Copy the host state into AbcCore.dll. Internal to the DLL; plugins reach
// them through AbcCoreApi.
void SetCoreContext(metahook_api_t *pAPI, mh_interface_t *pInterface,
                    mh_enginesave_t *pSave);
void SetEngineFuncs(cl_enginefunc_t *pEngineFuncs);
void SetClientExportFuncs(cl_exportfuncs_t *pExportFuncs);

#ifndef ABC_CORE_EXPORTS
inline void SysErrorV(const char *fmt, va_list args) {
  if (const AbcCoreApi *api = CoreApi())
    api->SysErrorV(fmt, args);
}
inline void SysError(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  SysErrorV(fmt, args);
  va_end(args);
}
#endif

// Fill the plugin-local abc:: state from the host pointers. Core keeps its
// own copy in AbcCore.dll; these globals no longer cross the DLL boundary.
inline void RefreshSharedState() {
  if (!g_pInterface || !g_pMetaHookAPI)
    return;
  g_pFileSystem = g_pInterface->FileSystem;
  g_iEngineType = g_pMetaHookAPI->GetEngineType();
  g_dwEngineBuildnum = g_pMetaHookAPI->GetEngineBuildnum();
  g_dwEngineBase = g_pMetaHookAPI->GetEngineBase();
  g_dwEngineSize = g_pMetaHookAPI->GetEngineSize();
  g_dwEngineTextBase = (PVOID)g_pMetaHookAPI->GetSectionByName(
      g_dwEngineBase, ".text\x0\x0\x0", &g_dwEngineTextSize);
  g_dwEngineDataBase = (PVOID)g_pMetaHookAPI->GetSectionByName(
      g_dwEngineBase, ".data\x0\x0\x0", &g_dwEngineDataSize);
  g_dwEngineRdataBase = (PVOID)g_pMetaHookAPI->GetSectionByName(
      g_dwEngineBase, ".rdata\x0\x0", &g_dwEngineRdataSize);
  if (g_pMetaHookAPI->GetClientBase) {
    g_dwClientBase = g_pMetaHookAPI->GetClientBase();
    g_dwClientSize = g_pMetaHookAPI->GetClientSize();
  }
}

// Base class every Abc plugin DLL exposes. Derived classes override the
// OnXxx hooks only; the IPluginsV4 plumbing (globals, engine type check,
// section discovery) is done here. The plugin's plugins.cpp keeps the
// EXPOSE_SINGLE_INTERFACE line:
//   class CMyPlugin : public abc::CAbcPluginBase { ... };
//   EXPOSE_SINGLE_INTERFACE(CMyPlugin, IPluginsV4,
//   METAHOOK_PLUGIN_API_VERSION_V4);
class CAbcPluginBase : public IPluginsV4 {
public:
  CAbcPluginBase(const char *pluginName, const char *version) {
    _snprintf_s(m_szName, sizeof(m_szName), _TRUNCATE, "%s", pluginName);
    _snprintf_s(m_szVersion, sizeof(m_szVersion), _TRUNCATE, "%s|Version:%s",
                pluginName, version);
    if (const AbcCoreApi *api = CoreApi())
      api->SetPluginName(pluginName);
  }

  void Init(metahook_api_t *pAPI, mh_interface_t *pInterface,
            mh_enginesave_t *pSave) final {
    g_pInterface = pInterface;
    g_pMetaHookAPI = pAPI;
    g_pMetaSave = pSave;
    if (const AbcCoreApi *api = CoreApi())
      api->SetContext(pAPI, pInterface, pSave);
    OnInit();
  }

  void Shutdown() final {
    OnShutdown();
  }

  void LoadEngine(cl_enginefunc_t *pEngineFuncs) final {
    memcpy(&gEngfuncs, pEngineFuncs, sizeof(gEngfuncs));
    RefreshSharedState();
    if (const AbcCoreApi *api = CoreApi())
      api->SetEngineFuncs(pEngineFuncs);

    if (g_iEngineType != ENGINE_SVENGINE) {
      SysError(
          "%s can only run in SvEngine!\nEngine type: %d\nEngine buildnum: %d",
          m_szName, g_iEngineType, g_dwEngineBuildnum);
      return;
    }

    OnLoadEngine();
  }

  void LoadClient(cl_exportfuncs_t *pExportFuncs) final {
    memcpy(&gExportfuncs, pExportFuncs, sizeof(gExportfuncs));
    RefreshSharedState();
    if (const AbcCoreApi *api = CoreApi())
      api->SetClientExportFuncs(pExportFuncs);
    OnLoadClient(pExportFuncs);
  }

  void ExitGame(int iResult) final {
    OnExitGame(iResult);
  }

  const char *GetVersion() final {
    return m_szVersion;
  }

protected:
  virtual void OnInit() {}
  virtual void OnShutdown() {}
  // Engine DLL loaded, gEngfuncs + engine section info ready.
  virtual void OnLoadEngine() {}
  // Client DLL loaded; hook pExportFuncs->HUD_XXX here as needed.
  virtual void OnLoadClient(cl_exportfuncs_t *pExportFuncs) {
    (void)pExportFuncs;
  }
  virtual void OnExitGame(int iResult) {
    (void)iResult;
  }

  const char *Version() const {
    return m_szVersion;
  }

private:
  char m_szName[64];
  char m_szVersion[96];
};

} // namespace abc

// IPluginsV4 base entries. Defined inline so a plugin does not import C++
// virtuals from AbcCore.dll; CAbcPluginBase overrides all of them.
inline void IPluginsV4::Init(metahook_api_t *, mh_interface_t *,
                             mh_enginesave_t *) {}
inline void IPluginsV4::Shutdown(void) {}
inline void IPluginsV4::LoadEngine(cl_enginefunc_t *) {}
inline void IPluginsV4::LoadClient(cl_exportfuncs_t *) {}
inline void IPluginsV4::ExitGame(int) {}
inline const char *IPluginsV4::GetVersion(void) {
  return "";
}
