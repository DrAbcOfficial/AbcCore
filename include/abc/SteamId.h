#pragma once

// abc::SteamId - SteamID parsing/rendering.
// Parsing lives in src/SteamId.cpp against the header-only SteamSDK
// submodule (thirdparty/SteamSDK, pinned to the MetaHookSv aggregate). The
// pinned SDK declares CSteamID's out-of-line methods but does not define
// them; src/detail/SteamIdCompat.cpp provides those compatibility symbols
// explicitly. No steam_api.dll dependency: only string/number parsing lives
// here; avatar/friends data goes through AbcUi's SteamAPIBridge (D-07).

#include <steamclientpublic.h>

namespace abc {

// Parse any supported text form ("[U:1:123]", "STEAM_0:1:123", plain
// account id, 64-bit steamid) into a CSteamID.
// Invalid text returns an invalid default ID. Strict API leaves out unchanged
// on failure.
bool TryParseSteamID(const char *text, CSteamID &out);
CSteamID ParseSteamID(const char *text);
// Text form ("[U:1:123]"); thread-local static buffer, not for long-term
// storage.
const char *RenderSteamID(const CSteamID &id);

} // namespace abc
