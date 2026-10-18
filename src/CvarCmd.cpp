// Ported from ABCEnchance src/utility/util.cpp (cvar/command half).
#include <metahook.h>

#include <abc/CvarCmd.h>
#include <abc/Console.h>

#include <cstdio>
#include <cstring>

namespace abc {

void ADD_COMMAND(const char *cmd, void (*pfnEngSrc_function)(void)) {
  gEngfuncs.pfnAddCommand(cmd, pfnEngSrc_function);
}

cvar_t *CVAR_GET_POINTER(const char *x) {
  return gEngfuncs.pfnGetCvarPointer(x);
}
float CVAR_GET_FLOAT(const char *x) {
  return gEngfuncs.pfnGetCvarFloat(x);
}
const char *CVAR_GET_STRING(const char *x) {
  return gEngfuncs.pfnGetCvarString(x);
}
void CVAR_SET_STRING(const char *x, const char *v) {
  gEngfuncs.Cvar_Set(x, v);
}
void CVAR_SET_FLOAT(const char *x, float v) {
  gEngfuncs.Cvar_SetValue(x, v);
}
cvar_t *CREATE_CVAR(const char *name, const char *val, int flag,
                    cvar_callback_t callback) {
  cvar_t *cvar = gEngfuncs.pfnRegisterVariable(const_cast<char *>(name),
                                               const_cast<char *>(val), flag);
  if (callback)
    g_pMetaHookAPI->RegisterCvarCallback(name, callback, nullptr);
  return cvar;
}

void ServerCmd(const char *x) {
  (*gEngfuncs.pfnServerCmd)(const_cast<char *>(x));
}
void EngineClientCmd(const char *x) {
  (*gEngfuncs.pfnClientCmd)(const_cast<char *>(x));
}
void PlaySoundByName(const char *x, float volum) {
  (*gEngfuncs.pfnPlaySoundByName)(const_cast<char *>(x), volum);
}
float ClientTime() {
  return gEngfuncs.GetClientTime();
}

void *SigScan(void *startSearch, unsigned long searchLen, const char *pattern,
              unsigned long patternLen) {
  return g_pMetaHookAPI->SearchPattern(startSearch, searchLen, pattern,
                                       patternLen);
}
void *SigScanReverse(void *startSearch, unsigned long searchLen,
                     const char *pattern, unsigned long patternLen) {
  return g_pMetaHookAPI->ReverseSearchPattern(startSearch, searchLen, pattern,
                                              patternLen);
}

CCvarRegistrar *CCvarRegistrar::Get() {
  // per-DLL instance (static lib)
  static CCvarRegistrar s_Registrar;
  return &s_Registrar;
}

cvar_t *CCvarRegistrar::Create(const char *name, const char *def, int flags,
                               cvar_callback_t callback) {
  cvar_t *cvar = CREATE_CVAR(name, def, flags, callback);
  if (cvar) {
    if (m_nCount < kMaxCvars) {
      m_aryCvars[m_nCount].CVar = cvar;
      m_nCount++;
    } else {
      // Registration still happened; only the debug listing misses it.
      ConsoleWarn("CCvarRegistrar: table full (%zu), \"%s\" not tracked\n",
                  kMaxCvars, name);
    }
  }
  return cvar;
}

void CCvarRegistrar::ListAll() {
  for (size_t i = 0; i < m_nCount; i++) {
    cvar_t *cvar = m_aryCvars[i].CVar;
    ConsolePrint("  %s = \"%s\"\n", cvar->name,
                 cvar->string ? cvar->string : "");
  }
  ConsolePrint("[abc] %zu cvar(s) registered\n", m_nCount);
}

} // namespace abc
