#pragma once

// abc::Format - C#-style safe string formatting ({0} {1} ... placeholders).
//
// Rationale (D-17 round, 2026-10-11): the migrated code paths format
// server-derived or translation-table text through the printf family, where
// the *text itself* is the format string (the HLSDK TextMsg contract hands
// the client a format plus arguments; localize templates carried %s/%s1
// placeholders). A stray %n, a specifier/argument mismatch or an overlong
// expansion then crashes or garbles the HUD. abc::Format never interprets
// the text as a printf format: arguments are pre-converted to strings and
// substituted positionally, so hostile text is inert by construction.
//
// Contract:
// - "{0}".."{N}" substitute the Nth argument (any value std::to_string
//   accepts, plus const char*/std::string/std::string_view).
// - "{{" renders a literal '{'. There is no '}' escape (a lone '}' is
//   literal, matching the minimal needs of HUD templates).
// - A "{...}" run that is not a well-formed placeholder renders verbatim;
//   an index past the argument count renders verbatim too (the visible
//   "{2}" is the bug report).
// - String conversion/output may allocate and throw. No printf interpretation.
//   This is not RML escaping or console-command argument escaping.

#include <cstddef>
#include <limits>
#include <type_traits>
#include <string>
#include <string_view>
#include <utility>

namespace abc {

namespace FormatDetail {

inline std::string FormatValue(std::string value) {
  return value;
}
inline std::string FormatValue(const char *value) {
  return value ? std::string(value) : std::string();
}
inline std::string FormatValue(std::string_view value) {
  return std::string(value);
}
template <typename T>
inline std::string FormatValue(const T &value)
  requires std::is_arithmetic_v<T>
{
  return std::to_string(value);
}

} // namespace FormatDetail

template <typename... Args>
inline std::string Format(std::string_view fmt, Args &&...args) {
  // Common case (a literal line with no placeholder): do not materialize a
  // string for every argument. Only a '{' can start a substitution.
  if (fmt.find('{') == std::string_view::npos)
    return std::string(fmt);
  // The trailing sentinel keeps the array legal for the zero-argument call;
  // the index guard below never reaches it.
  const std::string values[] = {
      FormatDetail::FormatValue(std::forward<Args>(args))..., std::string()};
  const size_t kArgCount = sizeof...(Args);

  std::string out;
  out.reserve(fmt.size());
  for (size_t i = 0; i < fmt.size(); i++) {
    const char c = fmt[i];
    if (c != '{') {
      out += c;
      continue;
    }
    if (i + 1 < fmt.size() && fmt[i + 1] == '{') {
      out += '{';
      i++;
      continue;
    }
    size_t j = i + 1;
    size_t index = 0;
    bool digits = false;
    bool overflow = false;
    while (j < fmt.size() && fmt[j] >= '0' && fmt[j] <= '9') {
      const size_t digit = static_cast<size_t>(fmt[j] - '0');
      if (index > ((std::numeric_limits<size_t>::max)() - digit) / 10)
        overflow = true;
      else if (!overflow)
        index = index * 10 + digit;
      digits = true;
      j++;
    }
    if (digits && !overflow && j < fmt.size() && fmt[j] == '}' &&
        index < kArgCount) {
      out += values[index];
      i = j;
    } else {
      // Malformed or out-of-range placeholder: verbatim (debuggable).
      out += '{';
    }
  }
  return out;
}

} // namespace abc
