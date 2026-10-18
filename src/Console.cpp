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

void ConsolePrintV(const char *fmt, va_list args) {
  ConsoleVPrint("", fmt, args);
}

void ConsoleWarnV(const char *fmt, va_list args) {
  ConsoleVPrint("[abc] WARNING: ", fmt, args);
}

void ConsoleErrorV(const char *fmt, va_list args) {
  ConsoleVPrint("[abc] ERROR: ", fmt, args);
}

void ConsoleWriteline(const char *text) {
  if (gEngfuncs.Con_Printf)
    gEngfuncs.Con_Printf("%s", text);
}

} // namespace abc
