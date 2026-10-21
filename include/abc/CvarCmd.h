#pragma once

// abc::CvarCmd - cvar/console-command helpers.
// Ported from ABCEnchance src/utility/util.h/.cpp.
// The old global gCVars mega-struct is intentionally NOT ported: each feature
// module owns its cvar_t* members and registers them via CREATE_CVAR (or the
// CCvarRegistrar below) in its own init path. Core never stores domain cvars.

// Platform header first (cvar_callback_t lives here) - the header must be
// self-sufficient, consumers must not rely on including metahook.h first.
#include <metahook.h>

#include <abc/AbcCoreInterface.h>

#include <cvardef.h>

namespace abc {

// Client-dll friendly flag combo used by most ABC cvars (was util.h
// FCVAR_VALUE).
constexpr int FCVAR_VALUE =
    (FCVAR_PRINTABLEONLY | FCVAR_CLIENTDLL | FCVAR_ARCHIVE);

#ifdef ABC_CORE_EXPORTS
void ADD_COMMAND(const char *cmd, void (*pfnEngSrc_function)(void));

cvar_t *CVAR_GET_POINTER(const char *x);
float CVAR_GET_FLOAT(const char *x);
const char *CVAR_GET_STRING(const char *x);
void CVAR_SET_STRING(const char *x, const char *v);
void CVAR_SET_FLOAT(const char *x, float v);
cvar_t *CREATE_CVAR(const char *name, const char *val, int flag,
                    cvar_callback_t callback);

void ServerCmd(const char *x);
void EngineClientCmd(const char *x);
#else
inline void ADD_COMMAND(const char *cmd, void (*fn)(void)) {
  if (const AbcCoreApi *api = AbcCoreApiTable())
    api->AddCommand(cmd, fn);
}
// Every wrapper degrades to a default when the table is absent (a stale
// AbcCore.dll whose interface name predates this build). CAbcPluginBase
// reports the mismatch once at Init; these accessors only avoid the
// unconditional null dereference the earlier mixed policy produced.
inline cvar_t *CVAR_GET_POINTER(const char *name) {
  const AbcCoreApi *api = AbcCoreApiTable();
  return api ? static_cast<cvar_t *>(api->CvarGetPointer(name)) : nullptr;
}
inline float CVAR_GET_FLOAT(const char *name) {
  const AbcCoreApi *api = AbcCoreApiTable();
  return api ? api->CvarGetFloat(name) : 0.0f;
}
inline const char *CVAR_GET_STRING(const char *name) {
  const AbcCoreApi *api = AbcCoreApiTable();
  return api ? api->CvarGetString(name) : "";
}
inline void CVAR_SET_STRING(const char *name, const char *value) {
  if (const AbcCoreApi *api = AbcCoreApiTable())
    api->CvarSetString(name, value);
}
inline void CVAR_SET_FLOAT(const char *name, float value) {
  if (const AbcCoreApi *api = AbcCoreApiTable())
    api->CvarSetFloat(name, value);
}
inline cvar_t *CREATE_CVAR(const char *name, const char *value, int flags,
                           cvar_callback_t callback) {
  const AbcCoreApi *api = AbcCoreApiTable();
  return api ? static_cast<cvar_t *>(api->CreateCvar(
                   name, value, flags,
                   reinterpret_cast<void (*)(void *)>(callback)))
             : nullptr;
}
inline void ServerCmd(const char *command) {
  if (const AbcCoreApi *api = AbcCoreApiTable())
    api->ServerCmd(command);
}
inline void EngineClientCmd(const char *command) {
  if (const AbcCoreApi *api = AbcCoreApiTable())
    api->EngineClientCmd(command);
}
#endif

// Thin wrappers over g_pMetaHookAPI->SearchPattern / ReverseSearchPattern.
// The sig-scan call sites themselves belong to the feature plugins (old
// exportfuncs.cpp FillAddress section), not to core.
void *SigScan(void *startSearch, unsigned long searchLen, const char *pattern,
              unsigned long patternLen);
void *SigScanReverse(void *startSearch, unsigned long searchLen,
                     const char *pattern, unsigned long patternLen);
void PlaySoundByName(const char *x, float volum);
float ClientTime();

// Central client-cvar registry for debugging/auditing. Registration is optional
// (plain CREATE_CVAR still works); everything created through Create() can be
// listed with ListAll().
class CCvarRegistrar {
public:
  // per-DLL instance (static lib)
  static CCvarRegistrar *Get();

  cvar_t *Create(const char *name, const char *def, int flags,
                 cvar_callback_t callback = nullptr);
  // Con_Prints every registered cvar: name = current value.
  void ListAll();

private:
  struct SEntry {
    cvar_t *CVar;
  };
  static const size_t kMaxCvars = 256;
  SEntry m_aryCvars[kMaxCvars];
  size_t m_nCount = 0;
};

} // namespace abc
