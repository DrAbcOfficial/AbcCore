#pragma once

// abc::Console - engine console output helpers.
// Replaces ABCEnchance src/utility/util.cpp ConsoleWriteline and the colored
// GameConsole prints (VGUI2/GameUI/GameConsole.cpp) that old core modules
// called via extern - core code must not depend on UI, so the colored variants
// degrade to plain text with a tag prefix here.

namespace abc {

// Raw Con_Printf passthrough.
void ConsolePrint(const char *fmt, ...);
// Plain-text warning ("[abc] WARNING: " prefix). Replaces ConsoleWarnMessage /
// GameConsole warn prints.
void ConsoleWarn(const char *fmt, ...);
// Plain-text error ("[abc] ERROR: " prefix). Replaces GameConsole error prints.
void ConsoleError(const char *fmt, ...);
// Raw-string print: text is emitted as-is (no printf interpretation, no
// newline added) - the safe entry for dynamic strings; ConsolePrint is
// printf-style and must never take a runtime string as its format.
void ConsoleWriteline(const char *text);

} // namespace abc
