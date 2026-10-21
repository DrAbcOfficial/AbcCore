#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <atomic>
#include <chrono>
#include <filesystem>
#include <string_view>
namespace abc::detail {
inline constexpr unsigned MaxConfigBytes = 4 * 1024 * 1024;
// rapidjson's default parser is recursive; a corrupt or hostile config.json
// with deeply nested arrays/objects would exhaust the 1 MB stack before the
// parse error is reported. Pre-scan the text (string-aware) and reject
// anything deeper than this before handing it to the parser.
inline constexpr unsigned MaxJsonNesting = 64;

inline bool JsonNestingExceeds(std::string_view text,
                               unsigned limit = MaxJsonNesting) {
  unsigned depth = 0;
  bool inString = false;
  bool escaped = false;
  for (const char c : text) {
    if (inString) {
      if (escaped)
        escaped = false;
      else if (c == '\\')
        escaped = true;
      else if (c == '"')
        inString = false;
      continue;
    }
    if (c == '"') {
      inString = true;
    } else if (c == '{' || c == '[') {
      if (++depth > limit)
        return true;
    } else if ((c == '}' || c == ']') && depth != 0) {
      --depth;
    }
  }
  return false;
}

// Crash leftovers from WriteConfigAtomic are named "<target>.tmp.<pid>.<seq>".
// Sweep the ones that can no longer belong to a live replace (older than an
// hour) on the next save so the config directory does not accumulate them.
inline void RemoveStaleConfigTemps(const std::filesystem::path &target) {
  if (target.parent_path().empty())
    return;
  std::error_code error;
  std::filesystem::directory_iterator entries(target.parent_path(), error);
  if (error)
    return;
  const std::wstring prefix = target.filename().wstring() + L".tmp.";
  const auto now = std::filesystem::file_time_type::clock::now();
  for (const auto &entry : entries) {
    const std::wstring name = entry.path().filename().wstring();
    if (name.rfind(prefix, 0) != 0)
      continue;
    std::error_code stampError;
    const auto stamp = entry.last_write_time(stampError);
    if (stampError || now - stamp < std::chrono::hours(1))
      continue;
    std::error_code removeError;
    std::filesystem::remove(entry.path(), removeError);
  }
}

// Native disk seam used by production persistence and offline fault tests.
bool WriteConfigAtomic(const std::filesystem::path &target,
                       std::string_view text);
inline bool WriteConfigAtomic(const std::filesystem::path &target,
                              std::string_view text) {
  if (text.size() > MaxConfigBytes || target.filename().empty())
    return false;
  std::error_code error;
  if (!target.parent_path().empty())
    std::filesystem::create_directories(target.parent_path(), error);
  if (error)
    return false;
  RemoveStaleConfigTemps(target);
  static std::atomic_ulong sequence{0};
  std::filesystem::path temporary;
  HANDLE file = INVALID_HANDLE_VALUE;
  for (int attempt = 0; attempt < 32 && file == INVALID_HANDLE_VALUE;
       ++attempt) {
    temporary = target;
    temporary += L".tmp." + std::to_wstring(GetCurrentProcessId()) + L"." +
                 std::to_wstring(sequence++);
    file = CreateFileW(temporary.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW,
                       FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE && GetLastError() != ERROR_FILE_EXISTS)
      return false;
  }
  if (file == INVALID_HANDLE_VALUE)
    return false;
  DWORD written = 0;
  bool ok = ::WriteFile(file, text.data(), static_cast<DWORD>(text.size()),
                        &written, nullptr) &&
            written == text.size() && FlushFileBuffers(file);
  if (!CloseHandle(file))
    ok = false;
  if (ok) {
    auto backup = target;
    backup += L".bak";
    // Do not remove the old target on failure, or fall back to copy+delete.
    // REPLACEFILE_WRITE_THROUGH keeps the same durability promise the
    // MoveFileExW branch makes for the first write.
    if (GetFileAttributesW(target.c_str()) != INVALID_FILE_ATTRIBUTES)
      ok = ReplaceFileW(target.c_str(), temporary.c_str(), backup.c_str(),
                        REPLACEFILE_WRITE_THROUGH, nullptr, nullptr) != FALSE;
    else
      ok = MoveFileExW(temporary.c_str(), target.c_str(),
                       MOVEFILE_WRITE_THROUGH) != FALSE;
  }
  if (!ok)
    DeleteFileW(temporary.c_str());
  return ok;
}

} // namespace abc::detail
