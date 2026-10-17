#pragma once

// Force-included by plugin targets so every translation unit sees the
// ABC_CORE_API declarations before metahook.h/windows.h are parsed.
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <abc/Plugin.h>
