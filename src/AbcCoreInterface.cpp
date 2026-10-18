// Valve-style C ABI entry for AbcCore.dll - see AbcCoreInterface.h.
#include <abc/AbcCoreInterface.h>

#include <abc/Console.h>
#include <abc/Core.h>
#include <abc/CvarCmd.h>
#include <abc/Paths.h>
#include <abc/Plugin.h>

#include <cstring>

namespace {

const char *ApiVersion() {
  return abc::CoreVersion();
}

void SetContextThunk(void *metaApi, void *interfacePtr, void *save) {
  abc::SetCoreContext(static_cast<metahook_api_t *>(metaApi),
                      static_cast<mh_interface_t *>(interfacePtr),
                      static_cast<mh_enginesave_t *>(save));
}

void SetEngineFuncsThunk(void *engineFuncs) {
  abc::SetEngineFuncs(static_cast<cl_enginefunc_t *>(engineFuncs));
}

void SetClientExportFuncsThunk(void *exportFuncs) {
  abc::SetClientExportFuncs(static_cast<cl_exportfuncs_t *>(exportFuncs));
}

void SysErrorVThunk(const char *fmt, va_list args) {
  abc::SysErrorV(fmt, args);
}

void ConsolePrintVThunk(const char *fmt, va_list args) {
  abc::ConsolePrintV(fmt, args);
}

void ConsoleWarnVThunk(const char *fmt, va_list args) {
  abc::ConsoleWarnV(fmt, args);
}

void ConsoleErrorVThunk(const char *fmt, va_list args) {
  abc::ConsoleErrorV(fmt, args);
}

void ConsoleWritelineThunk(const char *text) {
  abc::ConsoleWriteline(text);
}

void *CvarGetPointerThunk(const char *name) {
  return abc::CVAR_GET_POINTER(name);
}
float CvarGetFloatThunk(const char *name) {
  return abc::CVAR_GET_FLOAT(name);
}
const char *CvarGetStringThunk(const char *name) {
  return abc::CVAR_GET_STRING(name);
}
void CvarSetStringThunk(const char *name, const char *value) {
  abc::CVAR_SET_STRING(name, value);
}
void CvarSetFloatThunk(const char *name, float value) {
  abc::CVAR_SET_FLOAT(name, value);
}
void *CreateCvarThunk(const char *name, const char *value, int flags,
                      void (*callback)(void *)) {
  return abc::CREATE_CVAR(name, value, flags,
                          reinterpret_cast<cvar_callback_t>(callback));
}
void AddCommandThunk(const char *name, void (*fn)(void)) {
  abc::ADD_COMMAND(name, fn);
}
void ServerCmdThunk(const char *command) {
  abc::ServerCmd(command);
}
void EngineClientCmdThunk(const char *command) {
  abc::EngineClientCmd(command);
}

const char *GameDirThunk() {
  return abc::GameDir();
}
unsigned long long ModPathThunk(const char *relative, char *out,
                                unsigned long long capacity) {
  const std::string path = abc::ModPath(relative);
  if (out && capacity != 0) {
    const unsigned long long copy =
        path.size() < capacity ? path.size() : capacity;
    std::memcpy(out, path.data(), static_cast<std::size_t>(copy));
    if (copy < capacity)
      out[copy] = 0;
  }
  return path.size();
}
int EnsureDirThunk(const char *relativeDir) {
  return abc::EnsureDir(relativeDir) ? 1 : 0;
}
int EnsureParentDirThunk(const char *relativeFilePath) {
  return abc::EnsureParentDir(relativeFilePath) ? 1 : 0;
}
int CheckAssetThunk(const char *relativePath) {
  return abc::CheckAsset(relativePath) ? 1 : 0;
}

const AbcCoreApi kApi = {
    sizeof(AbcCoreApi),
    2u,
    ApiVersion,
    abc::SetPluginName,
    SetContextThunk,
    SetEngineFuncsThunk,
    SetClientExportFuncsThunk,
    SysErrorVThunk,
    ConsolePrintVThunk,
    ConsoleWarnVThunk,
    ConsoleErrorVThunk,
    ConsoleWritelineThunk,
    CvarGetPointerThunk,
    CvarGetFloatThunk,
    CvarGetStringThunk,
    CvarSetStringThunk,
    CvarSetFloatThunk,
    CreateCvarThunk,
    AddCommandThunk,
    ServerCmdThunk,
    EngineClientCmdThunk,
    GameDirThunk,
    ModPathThunk,
    EnsureDirThunk,
    EnsureParentDirThunk,
    CheckAssetThunk,
};

} // namespace

extern "C" ABC_CORE_API const AbcCoreApi *
AbcCore_CreateInterface(const char *name, int *returnCode) {
  if (returnCode)
    *returnCode = 1;
  if (!name || std::strcmp(name, ABC_CORE_INTERFACE_VERSION) != 0)
    return nullptr;
  if (returnCode)
    *returnCode = 0;
  return &kApi;
}
