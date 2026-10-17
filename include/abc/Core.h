#pragma once

namespace abc {

// Returns the AbcCore library version string.
// Note: AbcCore is a shared DLL; abc:: state is process-wide.
const char *CoreVersion();

}
