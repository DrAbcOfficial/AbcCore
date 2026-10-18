#pragma once

// Valve-style interface export for AbcCore.dll.
//
// The factory name is stable (`ABC_CORE_INTERFACE_VERSION`) and the returned
// struct is a plain C function table: no C++ classes, STL types or virtual
// layout cross the DLL boundary. New entries are appended and guarded by
// `size`; existing offsets never change within an interface version.

#include <abc/Export.h>

#define ABC_CORE_INTERFACE_VERSION "AbcCore_API_001"

typedef struct AbcCoreApi {
  unsigned int size;
  unsigned int version;
  const char *(*CoreVersion)(void);
  void (*SetPluginName)(const char *name);
  // Host pointers are passed as void* so this header stays C-compatible.
  void (*SetContext)(void *metaApi, void *interfacePtr, void *save);
  void (*SetEngineFuncs)(void *engineFuncs);
  void (*SetClientExportFuncs)(void *exportFuncs);
} AbcCoreApi;

#ifdef __cplusplus
extern "C" {
#endif

// Returns nullptr for an unknown name. `returnCode` receives 0 on success and
// 1 on failure, matching the Valve CreateInterface convention.
// Named with an AbcCore prefix because MetaHook's global CreateInterface is
// already the VGUI/Valve factory symbol.
ABC_CORE_API const AbcCoreApi *AbcCore_CreateInterface(const char *name,
                                                       int *returnCode);

#ifdef __cplusplus
}
#endif
