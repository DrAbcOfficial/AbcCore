#pragma once
#include <filesystem>
#include <string_view>
namespace abc::detail {
inline constexpr unsigned MaxConfigBytes = 4 * 1024 * 1024;
// Native disk seam used by production persistence and offline fault tests.
bool WriteConfigAtomic(const std::filesystem::path &target,
                       std::string_view text);
} // namespace abc::detail
