#pragma once

#include <abc/Plugin.h>

namespace abc {

static_assert(METAHOOK_API_VERSION >= 115,
              "ABC gamedata requires MetaHook API 115");

// Match the host gamedata required-symbol policy: never fall back to a byte
// scan.
inline void *ResolveGameSymbol(void *module, const char *moduleName,
                               const char *name, mh_gamesymbol_kind_t kind) {
  void *address = nullptr;
  const auto status =
      g_pMetaHookAPI->ResolveGameSymbol(module, name, kind, &address);
  if (status != MH_GAMESYMBOL_OK || !address)
    SysError("Could not resolve gamedata symbol: %s (module %s, %s)\nEngine "
             "buildnum: %d",
             name, moduleName,
             g_pMetaHookAPI->GetGameSymbolStatusString(status),
             g_dwEngineBuildnum);
  return address;
}

inline uint32_t GameScalar(void *module, const char *name) {
  uint32_t value = 0;
  const auto status =
      g_pMetaHookAPI->QueryGameSymbolScalar(module, name, &value);
  if (status != MH_GAMESYMBOL_OK)
    SysError("Could not query gamedata scalar: %s (%s)\nEngine buildnum: %d",
             name, g_pMetaHookAPI->GetGameSymbolStatusString(status),
             g_dwEngineBuildnum);
  return value;
}

inline uint32_t GameStructMember(void *module, const char *name) {
  uint32_t offset = 0;
  const auto status =
      g_pMetaHookAPI->QueryGameSymbolStructMember(module, name, &offset);
  if (status != MH_GAMESYMBOL_OK)
    SysError("Could not query gamedata member: %s (%s)\nEngine buildnum: %d",
             name, g_pMetaHookAPI->GetGameSymbolStatusString(status),
             g_dwEngineBuildnum);
  return offset;
}

} // namespace abc
