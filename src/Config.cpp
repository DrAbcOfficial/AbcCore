#include <abc/Config.h>
#include <abc/Paths.h>
#include <abc/Plugin.h>
#include <abc/detail/ConfigFile.h>

namespace abc {
JsonLoadStatus ReadJsonFileStatus(const char *relativePath,
                                  std::string &jsonOut) {
  if (!relativePath || !g_pFileSystem)
    return JsonLoadStatus::IoError;
  FileHandle_t file = g_pFileSystem->Open(relativePath, "rb");
  if (!file)
    return g_pFileSystem->FileExists(relativePath) ? JsonLoadStatus::IoError
                                                   : JsonLoadStatus::Missing;
  struct CloseFile {
    FileHandle_t File;
    ~CloseFile() {
      g_pFileSystem->Close(File);
    }
  } close{file};
  const unsigned size = g_pFileSystem->Size(file);
  if (size > detail::MaxConfigBytes)
    return JsonLoadStatus::Invalid;
  std::string text(size, '\0');
  const int read =
      size ? g_pFileSystem->Read(text.data(), static_cast<int>(size), file) : 0;
  if (read != static_cast<int>(size) || !g_pFileSystem->IsOk(file))
    return JsonLoadStatus::IoError;
  jsonOut = std::move(text);
  return JsonLoadStatus::Success;
}
bool ReadJsonFile(const char *relativePath, std::string &jsonOut) {
  return ReadJsonFileStatus(relativePath, jsonOut) == JsonLoadStatus::Success;
}
bool WriteJsonFile(const char *relativePath, const std::string &json) {
  // VFileSystem009 has void Flush/Close and no rename/replace operation.
  // Persist explicitly in the writable mod directory, never through a pack
  // or alternate read search path. Native sibling replacement preserves old
  // data.
  if (!relativePath || !*GameDir())
    return false;
  const auto relative = std::filesystem::u8path(relativePath);
  if (relative.empty() || relative.has_root_path())
    return false;
  for (const auto &part : relative)
    if (part == "..")
      return false;
  return detail::WriteConfigAtomic(std::filesystem::path(GameDir()) / relative,
                                   json);
}
} // namespace abc
