#pragma once

namespace abc {

// Returns the AbcCore library version string.
// Note: AbcCore is linked as a static library into each plugin DLL,
// so every plugin carries its own per-DLL instance of everything in abc::.
const char *CoreVersion();

} // namespace abc
