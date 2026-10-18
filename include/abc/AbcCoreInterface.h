#pragma once

// Valve-style interface export for AbcCore.dll.
//
// The factory name is stable (`ABC_CORE_INTERFACE_VERSION`) and the returned
// struct is a plain C function table: no C++ classes, STL types or virtual
// layout cross the DLL boundary. New entries are appended and guarded by
// `size`; existing offsets never change within an interface version.

#include <abc/Export.h>

#include <stdarg.h>

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

  // Console. The v-variants keep printf formatting in AbcCore.dll while the
  // C++ header only forwards va_list.
  void (*ConsolePrintV)(const char *fmt, va_list args);
  void (*ConsoleWarnV)(const char *fmt, va_list args);
  void (*ConsoleErrorV)(const char *fmt, va_list args);
  void (*ConsoleWriteline)(const char *text);

  // Cvar/command helpers. `cvar_t` and callbacks stay void* at the ABI edge.
  void *(*CvarGetPointer)(const char *name);
  float (*CvarGetFloat)(const char *name);
  const char *(*CvarGetString)(const char *name);
  void (*CvarSetString)(const char *name, const char *value);
  void (*CvarSetFloat)(const char *name, float value);
  void *(*CreateCvar)(const char *name, const char *value, int flags,
                      void (*callback)(void *));
  void (*AddCommand)(const char *name, void (*fn)(void));
  void (*ServerCmd)(const char *command);
  void (*EngineClientCmd)(const char *command);

  // Paths. ModPath returns the required length (excluding NUL) and copies at
  // most `capacity` bytes.
  const char *(*GameDir)(void);
  unsigned long long (*ModPath)(const char *relative, char *out,
                                unsigned long long capacity);
  int (*EnsureDir)(const char *relativeDir);
  int (*EnsureParentDir)(const char *relativeFilePath);
  int (*CheckAsset)(const char *relativePath);
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

#ifndef ABC_CORE_EXPORTS
inline const AbcCoreApi *AbcCoreApiTable() {
  static const AbcCoreApi *table = [] {
    int returnCode = 0;
    return AbcCore_CreateInterface(ABC_CORE_INTERFACE_VERSION, &returnCode);
  }();
  return table;
}
#endif
#endif
