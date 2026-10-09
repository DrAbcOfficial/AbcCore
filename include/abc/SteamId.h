#pragma once

// abc::SteamId - SteamID parsing/rendering.
// The out-of-line CSteamID methods ported from ABCEnchance
// src/utility/steamid.cpp are compiled in src/SteamId.cpp against the
// header-only SteamSDK submodule (thirdparty/SteamSDK, pinned to the
// MetaHookSv aggregate). No steam_api.dll dependency: only string/number
// parsing lives here; avatar/friends data goes through AbcUi's
// SteamAPIBridge (decision D-07).

#ifndef ABC_STEAMID_H
#define ABC_STEAMID_H

#include <steamclientpublic.h>

namespace abc {

// Parse any supported text form ("[U:1:123]", "STEAM_0:1:123", plain
// account id, 64-bit steamid) into a CSteamID.
CSteamID ParseSteamID(const char* text);
// Text form ("[U:1:123]"); thread-local static buffer, not for long-term storage.
const char* RenderSteamID(const CSteamID& id);

}

#endif //ABC_STEAMID_H
