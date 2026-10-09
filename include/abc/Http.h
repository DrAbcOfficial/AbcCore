#pragma once

// abc::Http - OPTIONAL HTTP client module (decision recorded in plan 03 §2).
//
// The implementation (src/Http/HttpClient.*) is excluded from the default
// abc_core sources. Build with:
//   -DABC_WITH_HTTP=ON -DABC_HTTPCLIENT_INCLUDE_DIR=<UtilHTTPClient PluginLib include>
// It requires IUtilHTTPClient.h at compile time and UtilHTTPClient_libcurl.dll
// at runtime. No current consumer (the NetEase integration was dropped); the
// interface must stay a soft dependency.

#ifndef ABC_HTTP_H
#define ABC_HTTP_H

#if defined(ABC_WITH_HTTP)
#include "../src/Http/HttpClient.h"
#endif

#endif //ABC_HTTP_H
