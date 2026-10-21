#pragma once

// abc::SteamId - SteamID parsing/rendering.
// Header-only against the header-only SteamSDK submodule (thirdparty/
// SteamSDK, pinned to the MetaHookSv aggregate). AbcCore.dll exports only the
// versioned C ABI table, so these helpers cannot live in a DLL translation
// unit: a downstream include would otherwise fail at link time with an
// unresolved external. The pinned SDK declares CSteamID's out-of-line methods
// but does not define them; src/detail/SteamIdCompat.cpp provides those
// compatibility symbols for AbcCore.dll itself. Consumers do not call
// CSteamID::Render() (out-of-line); RenderSteamID below is self-contained.
// No steam_api.dll dependency: only string/number parsing lives here;
// avatar/friends data goes through AbcUi's SteamAPIBridge (D-07).

#include <steamclientpublic.h>

#include <charconv>
#include <cstdint>
#include <cstdio>
#include <string_view>

namespace abc {

// Parse any supported text form ("[U:1:123]", "STEAM_0:1:123", plain
// account id, 64-bit steamid) into a CSteamID.
// Invalid text returns an invalid default ID. Strict API leaves out unchanged
// on failure.
inline bool TryParseSteamID(const char *text, CSteamID &out) {
  if (!text || !*text)
    return false;
  std::string_view input(text);
  auto number = [](std::string_view part, uint64 &value) {
    if (part.empty())
      return false;
    auto result =
        std::from_chars(part.data(), part.data() + part.size(), value);
    return result.ec == std::errc{} && result.ptr == part.data() + part.size();
  };
  uint64 universe = 1, account = 0, parity = 0;
  CSteamID candidate;
  if (input.starts_with("STEAM_")) {
    input.remove_prefix(6);
    auto first = input.find(':'), last = input.rfind(':');
    if (first == input.npos || first == last ||
        !number(input.substr(0, first), universe) ||
        !number(input.substr(first + 1, last - first - 1), parity) ||
        parity > 1 || !number(input.substr(last + 1), account) ||
        account > (UINT32_MAX - parity) / 2)
      return false;
    account = account * 2 + parity;
    if (universe == 0)
      universe = 1;
  } else if (input.starts_with("[U:") && input.ends_with("]")) {
    input.remove_prefix(3);
    input.remove_suffix(1);
    auto colon = input.find(':');
    if (colon == input.npos || !number(input.substr(0, colon), universe) ||
        !number(input.substr(colon + 1), account))
      return false;
  } else {
    if (!number(input, account))
      return false;
    if (account > UINT32_MAX) {
      candidate.SetFromUint64(account);
      if (!candidate.IsValid())
        return false;
      out = candidate;
      return true;
    }
  }
  if (!universe || universe >= k_EUniverseMax || account > UINT32_MAX)
    return false;
  candidate.InstancedSet(static_cast<uint32>(account), 1,
                         static_cast<EUniverse>(universe),
                         k_EAccountTypeIndividual);
  if (!candidate.IsValid())
    return false;
  out = candidate;
  return true;
}

inline CSteamID ParseSteamID(const char *text) {
  CSteamID id;
  TryParseSteamID(text, id);
  return id;
}

// Text form ("[U:1:123]"); thread-local static buffer (four-slot ring, the
// same contract as SteamSDK's CSteamID::Render), not for long-term storage.
// The account-type layout mirrors the SDK renderer, so callers that switch
// from CSteamID::Render() see the same text.
inline const char *RenderSteamID(const CSteamID &id) {
  constexpr int kBufferLength = 37;
  static thread_local char buffers[4][kBufferLength];
  static thread_local int next = 0;
  char *buffer = buffers[next];
  next = (next + 1) % 4;

  const unsigned int universe = static_cast<unsigned int>(id.GetEUniverse());
  const unsigned int account = static_cast<unsigned int>(id.GetAccountID());
  const unsigned int instance =
      static_cast<unsigned int>(id.GetUnAccountInstance());
  switch (id.GetEAccountType()) {
  case k_EAccountTypeAnonGameServer:
    std::snprintf(buffer, kBufferLength, "[A:%u:%u:%u]", universe, account,
                  instance);
    break;
  case k_EAccountTypeGameServer:
    std::snprintf(buffer, kBufferLength, "[G:%u:%u]", universe, account);
    break;
  case k_EAccountTypeMultiseat:
    std::snprintf(buffer, kBufferLength, "[M:%u:%u:%u]", universe, account,
                  instance);
    break;
  case k_EAccountTypePending:
    std::snprintf(buffer, kBufferLength, "[P:%u:%u]", universe, account);
    break;
  case k_EAccountTypeContentServer:
    std::snprintf(buffer, kBufferLength, "[C:%u:%u]", universe, account);
    break;
  case k_EAccountTypeClan:
    std::snprintf(buffer, kBufferLength, "[g:%u:%u]", universe, account);
    break;
  case k_EAccountTypeChat:
    if (instance & k_EChatInstanceFlagClan)
      std::snprintf(buffer, kBufferLength, "[c:%u:%u]", universe, account);
    else if (instance & k_EChatInstanceFlagLobby)
      std::snprintf(buffer, kBufferLength, "[L:%u:%u]", universe, account);
    else
      std::snprintf(buffer, kBufferLength, "[T:%u:%u]", universe, account);
    break;
  case k_EAccountTypeInvalid:
    std::snprintf(buffer, kBufferLength, "[I:%u:%u]", universe, account);
    break;
  case k_EAccountTypeIndividual:
    std::snprintf(buffer, kBufferLength, "[U:%u:%u]", universe, account);
    break;
  case k_EAccountTypeAnonUser:
    std::snprintf(buffer, kBufferLength, "[a:%u:%u]", universe, account);
    break;
  default:
    std::snprintf(buffer, kBufferLength, "[i:%u:%u]", universe, account);
    break;
  }
  return buffer;
}

} // namespace abc
