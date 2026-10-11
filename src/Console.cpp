#include <metahook.h>

#include <abc/Console.h>

#include <cstdarg>
#include <cstdio>

namespace abc {

void ConsolePrint(const char* fmt, ...) {
	char buf[2048];
	va_list args;
	va_start(args, fmt);
	_vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, args);
	va_end(args);
	if (gEngfuncs.Con_Printf) gEngfuncs.Con_Printf("%s", buf);
}

void ConsoleWarn(const char* fmt, ...) {
	char buf[2048];
	va_list args;
	va_start(args, fmt);
	_vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, args);
	va_end(args);
	if (gEngfuncs.Con_Printf) gEngfuncs.Con_Printf("[abc] WARNING: %s", buf);
}

void ConsoleError(const char* fmt, ...) {
	char buf[2048];
	va_list args;
	va_start(args, fmt);
	_vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, args);
	va_end(args);
	if (gEngfuncs.Con_Printf) gEngfuncs.Con_Printf("[abc] ERROR: %s", buf);
}

void ConsoleWriteline(const char* text) {
	if (gEngfuncs.Con_Printf) gEngfuncs.Con_Printf("%s", text);
}

}
