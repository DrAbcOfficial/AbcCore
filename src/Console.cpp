#include <metahook.h>

#include <abc/Console.h>

#include <cstdarg>
#include <cstdio>

namespace abc {

namespace {

void ConsoleVPrint(const char *Prefix, const char *Fmt, va_list Args) {
  char Buffer[2048];
  _vsnprintf_s(Buffer, sizeof(Buffer), _TRUNCATE, Fmt, Args);
  if (gEngfuncs.Con_Printf)
    gEngfuncs.Con_Printf("%s%s", Prefix, Buffer);
}

} // namespace

void ConsolePrint(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  ConsoleVPrint("", fmt, args);
  va_end(args);
}

void ConsoleWarn(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  ConsoleVPrint("[abc] WARNING: ", fmt, args);
  va_end(args);
}

void ConsoleError(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  ConsoleVPrint("[abc] ERROR: ", fmt, args);
  va_end(args);
}

void ConsoleWriteline(const char *text) {
  if (gEngfuncs.Con_Printf)
    gEngfuncs.Con_Printf("%s", text);
}

} // namespace abc
