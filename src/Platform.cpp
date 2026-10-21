// Ported from ABCEnchance src/core/library/win32helper.cpp (namespace-wrapped,
// snprintf-family uses updated; behavior preserved).
#include <abc/Platform.h>

#ifdef _WIN32

#include <cstring>
#include <cstdlib>
#include <cstdio>

namespace abc {

const char *Win32GetAttributesAsString(DWORD dwAttributes) {
  static char out[256];
  out[0] = 0;
  if (dwAttributes & FILE_ATTRIBUTE_ARCHIVE)
    std::strncat(out, "A", 1);
  if (dwAttributes & FILE_ATTRIBUTE_COMPRESSED)
    std::strncat(out, "C", 1);
  if (dwAttributes & FILE_ATTRIBUTE_DIRECTORY)
    std::strncat(out, "D", 1);
  if (dwAttributes & FILE_ATTRIBUTE_HIDDEN)
    std::strncat(out, "H", 1);
  if (dwAttributes & FILE_ATTRIBUTE_READONLY)
    std::strncat(out, "R", 1);
  if (dwAttributes & FILE_ATTRIBUTE_SYSTEM)
    std::strncat(out, "S", 1);
  if (dwAttributes & FILE_ATTRIBUTE_TEMPORARY)
    std::strncat(out, "T", 1);
  return out;
}

std::string Win32GetFileDescriptionByExtension(const char *extension) {
  HKEY hKey;
  if (RegOpenKeyExA(HKEY_CLASSES_ROOT, nullptr, 0, KEY_READ, &hKey) ==
      ERROR_SUCCESS) {
    std::string subKey = Win32GetDefaultValueFromRegistry(hKey, extension);
    std::string description = Win32GetDefaultValueFromRegistry(hKey, subKey);
    RegCloseKey(hKey);
    return description;
  }
  return "";
}
std::string Win32GetStringValueFromRegistry(HKEY hKey,
                                            const std::string &subKey,
                                            const std::string &valueName) {
  std::string result;
  DWORD size = 0;
  if (RegGetValueA(hKey, subKey.c_str(), valueName.c_str(), RRF_RT_REG_SZ,
                   nullptr, nullptr, &size) == ERROR_SUCCESS &&
      size > 0) {
    // RRF_RT_REG_SZ reports the byte count including the terminator; a zero
    // size (or an unset value) must not reach "size - 1" (huge size_t).
    char *buffer = new char[size];
    if (RegGetValueA(hKey, subKey.c_str(), valueName.c_str(), RRF_RT_REG_SZ,
                     nullptr, buffer, &size) == ERROR_SUCCESS &&
        size > 0) {
      const size_t length = buffer[size - 1] == '\0' ? size - 1 : size;
      result.assign(buffer, length);
    }
    delete[] buffer;
  }
  return result;
}
std::string Win32GetDefaultValueFromRegistry(HKEY hKey,
                                             const std::string &subKey) {
  return Win32GetStringValueFromRegistry(hKey, subKey, "");
}
HICON Win32GetFileIconByExtension(const std::string &extension) {
  HKEY hKey;
  if (RegOpenKeyExA(HKEY_CLASSES_ROOT, nullptr, 0, KEY_READ, &hKey) ==
      ERROR_SUCCESS) {
    std::string subKey = Win32GetDefaultValueFromRegistry(hKey, extension);
    std::string iconPath =
        Win32GetDefaultValueFromRegistry(hKey, subKey + "\\DefaultIcon");
    RegCloseKey(hKey);
    if (iconPath.size() == 0)
      return nullptr;
    // DefaultIcon value is "path[,index]" - a missing index means icon 0.
    // Parsed without exceptions: registry strings are untrusted input.
    std::string path = iconPath;
    int index = 0;
    const size_t pos = iconPath.find_last_of(',');
    if (pos != std::string::npos) {
      path = iconPath.substr(0, pos);
      index = static_cast<int>(
          std::strtol(iconPath.c_str() + pos + 1, nullptr, 10));
    }
    if (path.size() >= 2 && path.front() == '"' && path.back() == '"')
      path = path.substr(1, path.size() - 2);
    char expandedPath[MAX_PATH];
    const DWORD expanded =
        ExpandEnvironmentStringsA(path.c_str(), expandedPath, MAX_PATH);
    if (!expanded || expanded > MAX_PATH)
      return nullptr;
    wchar_t widePath[MAX_PATH];
    if (!MultiByteToWideChar(CP_ACP, 0, expandedPath, -1, widePath, MAX_PATH))
      return nullptr;
    HICON smallIcon = nullptr;
    if (ExtractIconExW(widePath, index, nullptr, &smallIcon, 1) != 1)
      return nullptr;
    return smallIcon;
  }
  return nullptr;
}
BITMAP Win32GetBitmapInfo(HICON hIcon) {
  BITMAP bmp = {};
  ICONINFO iconInfo = {};
  if (!GetIconInfo(hIcon, &iconInfo))
    return bmp;
  GetObject(iconInfo.hbmColor ? iconInfo.hbmColor : iconInfo.hbmMask,
            sizeof(BITMAP), &bmp);
  if (!iconInfo.hbmColor)
    bmp.bmHeight /= 2;
  if (iconInfo.hbmColor)
    DeleteObject(iconInfo.hbmColor);
  if (iconInfo.hbmMask)
    DeleteObject(iconInfo.hbmMask);
  return bmp;
}
std::vector<unsigned char> Win32ConvertHICONToRGBA(HICON hIcon, size_t &ww,
                                                   size_t &hh) {
  ww = hh = 0;
  BITMAP bmp = Win32GetBitmapInfo(hIcon);
  const int width = bmp.bmWidth, height = bmp.bmHeight;
  if (width <= 0 || height <= 0 || width > 4096 || height > 4096)
    return {};
  // Allocate before acquiring handles so bad_alloc cannot leak GDI resources.
  std::vector<unsigned char> rgba(static_cast<size_t>(width) * height * 4);
  HDC dc = CreateCompatibleDC(nullptr);
  if (!dc)
    return {};
  BITMAPINFO bmi{};
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = width;
  bmi.bmiHeader.biHeight = -height;
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  unsigned char *bits = nullptr;
  HBITMAP bitmap =
      CreateDIBSection(dc, &bmi, DIB_RGB_COLORS, (void **)&bits, nullptr, 0);
  if (!bitmap || !bits) {
    if (bitmap)
      DeleteObject(bitmap);
    DeleteDC(dc);
    return {};
  }
  HGDIOBJ previous = SelectObject(dc, bitmap);
  if (!previous || previous == HGDI_ERROR) {
    DeleteObject(bitmap);
    DeleteDC(dc);
    return {};
  }
  std::memset(bits, 0, rgba.size());
  const bool ok = DrawIconEx(dc, 0, 0, hIcon, width, height, 0, nullptr,
                             DI_NORMAL) != FALSE;
  if (ok)
    for (size_t i = 0; i < rgba.size(); i += 4) {
      rgba[i] = bits[i + 2];
      rgba[i + 1] = bits[i + 1];
      rgba[i + 2] = bits[i];
      rgba[i + 3] = bits[i + 3];
    }
  SelectObject(dc, previous); // Never delete an object while selected in a DC.
  DeleteObject(bitmap);
  DeleteDC(dc);
  if (!ok)
    return {};
  ww = width;
  hh = height;
  return rgba;
}
void Sys_GetRegKeyValueUnderRoot(HKEY rootKey, const char *pszSubKey,
                                 const char *pszElement, char *pszReturnString,
                                 int nReturnLength,
                                 const char *pszDefaultValue) {
  LONG lResult;
  HKEY hKey;
  char szBuff[128];
  DWORD dwDisposition;
  DWORD dwType;
  DWORD dwSize;
  // This helper is dormant but wired into the public header: reject the null
  // and negative-length calls the old body passed straight to snprintf
  // (a negative int becomes a huge size_t).
  if (!pszSubKey || !pszElement || !pszReturnString || nReturnLength <= 0)
    return;
  if (!pszDefaultValue)
    pszDefaultValue = "";
  snprintf(pszReturnString, nReturnLength, "%s", pszDefaultValue);
  if (nReturnLength > (int)sizeof(szBuff))
    nReturnLength = (int)sizeof(szBuff); // szBuff below can't hold more
  lResult = RegCreateKeyExA(rootKey, pszSubKey, 0, (LPSTR) "String",
                            REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr,
                            &hKey, &dwDisposition);
  if (lResult != ERROR_SUCCESS)
    return;
  if (dwDisposition == REG_CREATED_NEW_KEY)
    lResult = RegSetValueExA(hKey, pszElement, 0, REG_SZ,
                             (CONST BYTE *)pszDefaultValue,
                             (DWORD)strlen(pszDefaultValue) + 1);
  else {
    dwSize = nReturnLength;
    lResult = RegQueryValueExA(hKey, pszElement, 0, &dwType,
                               (unsigned char *)szBuff, &dwSize);
    if (lResult == ERROR_SUCCESS) {
      if (dwType == REG_SZ) {
        strncpy(pszReturnString, szBuff, nReturnLength);
        pszReturnString[nReturnLength - 1] = '\0';
      }
    } else
      lResult = RegSetValueExA(hKey, pszElement, 0, REG_SZ,
                               (CONST BYTE *)pszDefaultValue,
                               (DWORD)strlen(pszDefaultValue) + 1);
  };
  RegCloseKey(hKey);
}
void Win32GetRegKeyValue(char *pszSubKey, char *pszElement,
                         char *pszReturnString, int nReturnLength,
                         char *pszDefaultValue) {
  Sys_GetRegKeyValueUnderRoot(HKEY_CURRENT_USER, pszSubKey, pszElement,
                              pszReturnString, nReturnLength, pszDefaultValue);
}

} // namespace abc

#endif //_WIN32
