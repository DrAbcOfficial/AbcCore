#pragma once

// Engine-backed random helpers.
//
// Kept out of abc/Math.h so the pure math header does not pull in metahook.h
// and the Windows headers. Include this header when calling
// CMathlib::RANDOM_FLOAT / CMathlib::RANDOM_LONG or the abc::math aliases.

#include <abc/Math.h>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <metahook.h>

namespace abc {
inline float CMathlib::RANDOM_FLOAT(float low, float high) {
  return gEngfuncs.pfnRandomFloat(low, high);
}
inline int CMathlib::RANDOM_LONG(int low, int high) {
  return gEngfuncs.pfnRandomLong(low, high);
}

namespace math {
inline float RANDOM_FLOAT(float low, float high) {
  return CMathlib::RANDOM_FLOAT(low, high);
}
inline int RANDOM_LONG(int low, int high) {
  return CMathlib::RANDOM_LONG(low, high);
}
} // namespace math
} // namespace abc
