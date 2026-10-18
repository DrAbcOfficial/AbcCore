#pragma once

// Engine-backed random helpers.
//
// Kept out of abc/Math.h so the pure math header does not pull in metahook.h
// and the Windows headers. Include this header when calling
// abc::math::RANDOM_FLOAT / RANDOM_LONG.

#include <abc/Math.h>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <metahook.h>

namespace abc {
namespace math {
inline float RANDOM_FLOAT(float low, float high) {
  return gEngfuncs.pfnRandomFloat(low, high);
}
inline int RANDOM_LONG(int low, int high) {
  return gEngfuncs.pfnRandomLong(low, high);
}
} // namespace math
} // namespace abc
