#pragma once

// Force-included by plugin targets (AbcUi: /FIabc/ForceImport.h) so every
// translation unit sees NOMINMAX and the abc plugin declarations before
// metahook.h / windows.h are parsed.
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <abc/Plugin.h>
