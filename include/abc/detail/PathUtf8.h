#pragma once

// std::filesystem::u8path is deprecated in C++20. This helper makes the
// intent explicit: the input is UTF-8 and the path is constructed from a
// u8string, which converts through the native wide representation.

#include <filesystem>
#include <string>
#include <string_view>

namespace abc::detail {

inline std::filesystem::path PathFromUtf8(std::string_view Text) {
  return std::filesystem::path(std::u8string(
      reinterpret_cast<const char8_t *>(Text.data()), Text.size()));
}

} // namespace abc::detail
