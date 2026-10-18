// Ported from ABCEnchance src/utility/steamid.cpp (the non-Steam2, non-GameID
// parts; those are compiled out in the original too). The SDK compatibility
// methods live in src/detail/SteamIdCompat.cpp.
#include <abc/SteamId.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <charconv>
#include <string_view>
#include <limits>

namespace abc {

bool TryParseSteamID(const char *text, CSteamID &out) {
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
CSteamID ParseSteamID(const char *text) {
  CSteamID id;
  TryParseSteamID(text, id);
  return id;
}

const char *RenderSteamID(const CSteamID &id) {
  return id.Render();
}

} // namespace abc
