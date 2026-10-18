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
#include <filesystem>
#include <string_view>
namespace abc::detail {
inline constexpr unsigned MaxConfigBytes = 4 * 1024 * 1024;
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
    if (GetFileAttributesW(target.c_str()) != INVALID_FILE_ATTRIBUTES)
      ok = ReplaceFileW(target.c_str(), temporary.c_str(), backup.c_str(), 0,
                        nullptr, nullptr) != FALSE;
    else
      ok = MoveFileExW(temporary.c_str(), target.c_str(),
                       MOVEFILE_WRITE_THROUGH) != FALSE;
  }
  if (!ok)
    DeleteFileW(temporary.c_str());
  return ok;
}

} // namespace abc::detail
