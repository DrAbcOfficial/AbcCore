#pragma once

// abc::Console - engine console output helpers.
// Replaces ABCEnchance src/utility/util.cpp ConsoleWriteline and the colored
// GameConsole prints (VGUI2/GameUI/GameConsole.cpp) that old core modules
// called via extern - core code must not depend on UI, so the colored variants
// degrade to plain text with a tag prefix here.
//
// Consumers call the inline wrappers, which forward va_list to AbcCore.dll;
// the printf formatting stays in the shared library.

#include <abc/AbcCoreInterface.h>

#include <cstdarg>

namespace abc {

#ifdef ABC_CORE_EXPORTS
void ConsolePrintV(const char *fmt, va_list args);
void ConsoleWarnV(const char *fmt, va_list args);
void ConsoleErrorV(const char *fmt, va_list args);
void ConsoleWriteline(const char *text);
inline void ConsolePrint(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  ConsolePrintV(fmt, args);
  va_end(args);
}
inline void ConsoleWarn(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  ConsoleWarnV(fmt, args);
  va_end(args);
}
inline void ConsoleError(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  ConsoleErrorV(fmt, args);
  va_end(args);
}
#else
inline void ConsolePrintV(const char *fmt, va_list args) {
  if (const AbcCoreApi *api = AbcCoreApiTable())
    api->ConsolePrintV(fmt, args);
}
inline void ConsoleWarnV(const char *fmt, va_list args) {
  if (const AbcCoreApi *api = AbcCoreApiTable())
    api->ConsoleWarnV(fmt, args);
}
inline void ConsoleErrorV(const char *fmt, va_list args) {
  if (const AbcCoreApi *api = AbcCoreApiTable())
    api->ConsoleErrorV(fmt, args);
}
inline void ConsoleWriteline(const char *text) {
  if (const AbcCoreApi *api = AbcCoreApiTable())
    api->ConsoleWriteline(text);
}

inline void ConsolePrint(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  ConsolePrintV(fmt, args);
  va_end(args);
}
inline void ConsoleWarn(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  ConsoleWarnV(fmt, args);
  va_end(args);
}
inline void ConsoleError(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  ConsoleErrorV(fmt, args);
  va_end(args);
}
#endif

} // namespace abc
