#pragma once

// abc::ParseNumber - strict, locale-independent number parsing.
//
// The three repos previously carried seven different parsers (strtod,
// strtof, std::stof + catch, hand-written digit loops, std::from_chars).
// They disagreed on trailing garbage, locale and exceptions. This helper is
// the single rule: trim ASCII whitespace, consume the whole view, never
// throw, return std::optional.

#include <charconv>
#include <optional>
#include <string_view>
#include <system_error>

namespace abc {

namespace detail {

inline std::string_view TrimAsciiSpace(std::string_view Text) {
  const auto IsSpace = [](char Ch) {
    return Ch == ' ' || Ch == '\t' || Ch == '\r' || Ch == '\n';
  };
  while (!Text.empty() && IsSpace(Text.front()))
    Text.remove_prefix(1);
  while (!Text.empty() && IsSpace(Text.back()))
    Text.remove_suffix(1);
  return Text;
}

} // namespace detail

// Parses the full (trimmed) view as T. Returns nullopt on empty input,
// trailing garbage, overflow, or a malformed value. Supports integral and
// floating-point T (std::from_chars constraints apply).
template <typename T> std::optional<T> ParseNumber(std::string_view Text) {
  Text = detail::TrimAsciiSpace(Text);
  if (Text.empty())
    return std::nullopt;

  T Value{};
  const char *First = Text.data();
  const char *Last = Text.data() + Text.size();
  const auto Result = std::from_chars(First, Last, Value);
  if (Result.ec != std::errc{} || Result.ptr != Last)
    return std::nullopt;
  return Value;
}

inline std::optional<int> ParseInt(std::string_view Text) {
  return ParseNumber<int>(Text);
}

inline std::optional<float> ParseFloat(std::string_view Text) {
  return ParseNumber<float>(Text);
}

} // namespace abc
