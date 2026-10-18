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

const AbcCoreApi kApi = {
    sizeof(AbcCoreApi),
    1u,
    ApiVersion,
    abc::SetPluginName,
    SetContextThunk,
    SetEngineFuncsThunk,
    SetClientExportFuncsThunk,
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
