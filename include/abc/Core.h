#pragma once

namespace abc {

// Returns the AbcCore library version string.
// AbcCore is a shared library (AbcCore.dll); its internal state is shared, and
// the plugin-facing surface is the C ABI CreateInterface table.
const char *CoreVersion();

} // namespace abc
