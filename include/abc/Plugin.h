#pragma once

// abc::Plugin - plugin base class + shared engine state.
// Ported from ABCEnchance src/plugins.h/.cpp.
//
// Each plugin DLL defines the metahook.h globals once (src/CoreGlobals.cpp).
// CAbcPluginBase copies the host pointers into AbcCore.dll through the
// exported setters, so shared utilities and plugin code observe the same state.
//
// The abc:: state below is process-wide because it lives in AbcCore.dll.

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
extern ABC_CORE_API IFileSystem *g_pFileSystem;
extern ABC_CORE_API DWORD g_dwEngineBuildnum;
extern ABC_CORE_API int g_iEngineType;
extern ABC_CORE_API PVOID g_dwEngineBase;
extern ABC_CORE_API DWORD g_dwEngineSize;
extern ABC_CORE_API PVOID g_dwEngineTextBase;
extern ABC_CORE_API DWORD g_dwEngineTextSize;
extern ABC_CORE_API PVOID g_dwEngineDataBase;
extern ABC_CORE_API DWORD g_dwEngineDataSize;
extern ABC_CORE_API PVOID g_dwEngineRdataBase;
extern ABC_CORE_API DWORD g_dwEngineRdataSize;
// Filled in CAbcPluginBase::LoadClient.
extern ABC_CORE_API PVOID g_dwClientBase;
extern ABC_CORE_API DWORD g_dwClientSize;

// "[<plugin>]: ..." fatal error via AbcCore.dll's host context.
ABC_CORE_API void SysError(const char *fmt, ...);
ABC_CORE_API void SetPluginName(const char *name);
ABC_CORE_API const char *PluginName();

// Copy the host state into AbcCore.dll. Called by the inline CAbcPluginBase.
ABC_CORE_API void SetCoreContext(metahook_api_t *pAPI,
                                 mh_interface_t *pInterface,
                                 mh_enginesave_t *pSave);
ABC_CORE_API void SetEngineFuncs(cl_enginefunc_t *pEngineFuncs);
ABC_CORE_API void SetClientExportFuncs(cl_exportfuncs_t *pExportFuncs);

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
    else
      SetPluginName(pluginName);
  }

  void Init(metahook_api_t *pAPI, mh_interface_t *pInterface,
            mh_enginesave_t *pSave) final {
    g_pInterface = pInterface;
    g_pMetaHookAPI = pAPI;
    g_pMetaSave = pSave;
    if (const AbcCoreApi *api = CoreApi())
      api->SetContext(pAPI, pInterface, pSave);
    else
      SetCoreContext(pAPI, pInterface, pSave);
    OnInit();
  }

  void Shutdown() final {
    OnShutdown();
  }

  void LoadEngine(cl_enginefunc_t *pEngineFuncs) final {
    memcpy(&gEngfuncs, pEngineFuncs, sizeof(gEngfuncs));
    if (const AbcCoreApi *api = CoreApi())
      api->SetEngineFuncs(pEngineFuncs);
    else
      SetEngineFuncs(pEngineFuncs);

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
    if (const AbcCoreApi *api = CoreApi())
      api->SetClientExportFuncs(pExportFuncs);
    else
      SetClientExportFuncs(pExportFuncs);
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
