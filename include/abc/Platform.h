#pragma once

// abc::Platform - Win32 misc helpers.
// Ported from ABCEnchance src/core/library/win32helper.h/.cpp
// (namespace-wrapped, proper <windows.h> include added - the old header relied
// on include order). HICON->RGBA conversion is kept for AbcUi's future use.
//
// DORMANT MODULE: no consumer inside the three repos yet. Re-audit the
// registry parsing (Win32GetFileIconByExtension) against real registry data
// before wiring the first call site.

#ifdef _WIN32
#include <windows.h>
#include <string>
#include <vector>

namespace abc {

const char *Win32GetAttributesAsString(DWORD dwAttributes);
std::string Win32GetFileDescriptionByExtension(const char *extension);
std::string Win32GetStringValueFromRegistry(HKEY hKey,
                                            const std::string &subKey,
                                            const std::string &valueName);
std::string Win32GetDefaultValueFromRegistry(HKEY hKey,
                                             const std::string &subKey);
HICON Win32GetFileIconByExtension(const std::string &extension);
BITMAP Win32GetBitmapInfo(HICON hIcon);
std::vector<unsigned char> Win32ConvertHICONToRGBA(HICON hIcon, size_t &ww,
                                                   size_t &hh);
void Win32GetRegKeyValue(char *pszSubKey, char *pszElement,
                         char *pszReturnString, int nReturnLength,
                         char *pszDefaultValue);

} // namespace abc

#endif //_WIN32
