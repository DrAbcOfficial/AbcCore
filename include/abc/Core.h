#pragma once

namespace abc {

// Returns the AbcCore library version string.
// AbcCore is a shared library (AbcCore.dll); abc:: state is process-wide and
// the stable plugin-facing surface is the C ABI CreateInterface table.
const char *CoreVersion();

} // namespace abc
