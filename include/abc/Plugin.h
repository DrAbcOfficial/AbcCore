#pragma once

// abc::Plugin - plugin base class + shared engine state.
// Ported from ABCEnchance src/plugins.h/.cpp.
//
// Ownership of the globals declared extern in metahook.h (g_pInterface,
// gEngfuncs, g_pMetaHookAPI, g_pMetaSave): they are DEFINED once per plugin
// DLL by src/Plugin.cpp of this static library. A plugin built on
// CAbcPluginBase must NOT define them again (duplicate symbol).
//
// All abc:: state below is per-DLL (static lib) - never assume process-wide
// uniqueness, both AbcEssential and AbcUi carry their own copy.

#ifndef ABC_PLUGIN_H
#define ABC_PLUGIN_H

#include <metahook.h>

class IFileSystem;

namespace abc {

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

// "[<plugin>]: ..." fatal error via g_pMetaHookAPI->SysError.
// Plugin name comes from the CAbcPluginBase ctor (default "abc").
void SysError(const char *fmt, ...);
void SetPluginName(const char *name);
const char *PluginName();

// Base class every Abc plugin DLL exposes. Derived classes override the
// OnXxx hooks only; the IPluginsV4 plumbing (globals, engine type check,
// section discovery) is done here. The plugin's plugins.cpp keeps the
// EXPOSE_SINGLE_INTERFACE line:
//   class CMyPlugin : public abc::CAbcPluginBase { ... };
//   EXPOSE_SINGLE_INTERFACE(CMyPlugin, IPluginsV4,
//   METAHOOK_PLUGIN_API_VERSION_V4);
class CAbcPluginBase : public IPluginsV4 {
public:
  CAbcPluginBase(const char *pluginName, const char *version);

  void Init(metahook_api_t *pAPI, mh_interface_t *pInterface,
            mh_enginesave_t *pSave) final;
  void Shutdown() final;
  void LoadEngine(cl_enginefunc_t *pEngineFuncs) final;
  void LoadClient(cl_exportfuncs_t *pExportFuncs) final;
  void ExitGame(int iResult) final;
  const char *GetVersion() final;

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

  const char *Version() const;

private:
  char m_szName[64];
  char m_szVersion[96];
};

} // namespace abc

#endif // ABC_PLUGIN_H
