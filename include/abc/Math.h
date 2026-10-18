#pragma once

// abc::math - vector/matrix math library.
// Ported from ABCEnchance src/core/library/mymathlib.h/.cpp (GBK -> UTF-8).
// Vector typedefs stay at global scope: engine/HLSDK headers (com_model.h,
// triangleapi.h, ...) expect unqualified vec2_t/vec3_t/vec4_t and this library
// was the only provider of them in the original plugin. Renderer-side
// mat3x4/mat4/vec4/ivec4/uvec4 aliases live in AbcUi's RenderCommon.h.

// Engine/SDK headers expose these as macros; hide them while this header
// includes the standard library, then restore the caller's definitions.
#ifdef clamp
#pragma push_macro("clamp")
#undef clamp
#define ABC_MATH_RESTORE_CLAMP
#endif
#ifdef min
#pragma push_macro("min")
#undef min
#define ABC_MATH_RESTORE_MIN
#endif
#ifdef max
#pragma push_macro("max")
#undef max
#define ABC_MATH_RESTORE_MAX
#endif

#include <cstddef>
#include <numbers>

using vec_t = float;
using vec2_t = vec_t[2];
using vec3_t = vec_t[3];
using vec4_t = vec_t[4];
using vec5_t = vec_t[5];

namespace abc {
namespace math {

struct Rgb {
  int r;
  int g;
  int b;
};
struct Hsv {
  float h;
  float s;
  float v;
};
struct DivMod {
  int quotient;
  int remainder;
};

inline constexpr float kPi = std::numbers::pi_v<float>;
inline constexpr double kPiDouble = std::numbers::pi;
inline constexpr float kDegToRad = std::numbers::pi_v<float> / 180.0f;
inline constexpr float kRadToDeg = 180.0f / std::numbers::pi_v<float>;
inline constexpr float kMetersPerInch = 0.0254f;
inline constexpr float kEqualEpsilon = 0.001f;

inline constexpr int kPitch = 0; // up / down
inline constexpr int kYaw = 1;   // left / right
inline constexpr int kRoll = 2;  // fall over

bool IsNaN(float x);
float DegToRad(float degrees);
float RadToDeg(float radians);

float METER2INCH(float x);
float INCH2METER(float x);

void VectorSubtract(vec3_t a, vec3_t b, vec3_t c);
void VectorAdd(vec3_t a, vec3_t b, vec3_t c);
void VectorAddFloat(vec3_t a, float f);
void VectorMultiply(vec3_t a, float f);
void VectorCopy(const vec3_t src, vec3_t dst);
void Vector2Copy(vec2_t src, vec2_t dst);
void VectorClear(vec3_t a);
float DotProduct(const vec3_t x, const vec3_t y);

void ColorApplyAlpha(int &r, int &g, int &b, int a);
// contentLength * clamp(fraction, 0, 1), truncated.
std::size_t ScalePixelLength(int contentLength, double fraction);
// Rotates (x, y) by radians.
void Vector2Rotate(vec2_t out, float x, float y, float radians);

void MatrixCopy(float in[4][3], float out[4][3]);
void VectorMA(const vec3_t veca, float scale, const vec3_t vecb,
              vec3_t vecc);

// Element-wise absolute comparison against HLSDK EQUAL_EPSILON (0.001).
int VectorCompare(const vec3_t v1, const vec3_t v2);
// Single-precision length.
float FVectorLength(const vec3_t v);
// Double-precision length.
double VectorLength(const vec3_t v);
void CrossProduct(const vec3_t v1, const vec3_t v2, vec3_t cross);
float VectorNormalize(vec3_t v);
void VectorInverse(vec3_t v);
void VectorScale(const vec3_t in, vec_t scale, vec3_t out);
// floor(log2(val)); non-positive input returns 0.
int Log2Floor(int val);
// Initializes mins to 99999 and maxs to -99999.
void ResetBounds(vec3_t mins, vec3_t maxs);
// Any finite normal; zero normal leaves p unchanged. Aliasing is supported.
void ProjectPointOnPlane(vec3_t dst, const vec3_t p, const vec3_t normal);
void PerpendicularVector(vec3_t dst, const vec3_t src);
vec_t RoundHalfUp(vec_t in);
void AddPointToBounds(vec3_t v, vec3_t mins, vec3_t maxs);

// Legacy GoldSrc angle order: angles[0] is roll, angles[1] is pitch and
// angles[2] is yaw. Prefer Matrix4x4_CreateFromEntity for engine
// [pitch, yaw, roll] angles.
void AngleIMatrix(const vec3_t angles, float matrix[3][4]);
// Angles are pitch/yaw/roll degrees, matching AngleVectors.
void AngleQuaternion(const vec3_t angles, vec4_t quaternion);
void QuaternionMatrix(const vec4_t quaternion, float (*matrix)[4]);
// Does not modify p or q.
void QuaternionSlerp(const vec4_t p, const vec4_t q, float t, vec4_t qt);
void SinCos(float radians, float *sine, float *cosine);

void ConcatRotations(float in1[3][3], float in2[3][3], float out[3][3]);
// Affine row-major matrices; angles are pitch/yaw/roll degrees. Writes
// (0,0,0,1) last row.
void Matrix4x4_CreateFromEntity(float out[4][4], const vec3_t angles,
                                const vec3_t origin, float scale);

void FloorDivMod(double numer, double denom, int *quotient, int *rem);
DivMod FloorDivMod(double numer, double denom);
// Euclidean gcd; non-negative result, negative inputs accepted.
int GreatestCommonDivisor(int i1, int i2);
void RotatePointAroundVector(vec3_t dst, const vec3_t dir, const vec3_t point,
                             float degrees);

void AngleVectors(const vec3_t angles, vec3_t forward, vec3_t right,
                  vec3_t up);
// GoldSrc transpose: forward/right/up are the rows of the rotation matrix
// produced by Matrix4x4_CreateFromEntity for the same angles.
void AngleVectorsTranspose(const vec3_t angles, vec3_t *forward, vec3_t *right,
                           vec3_t *up);
// See AngleIMatrix for the legacy GoldSrc angle order.
void AngleMatrix(const vec3_t angles, float (*matrix)[4]);
void VectorTransform(const vec3_t in1, float in2[3][4], vec3_t out);
void VectorAngles(const vec3_t forward, vec3_t angles);

// 4x4 inverse; returns 0 for a singular matrix.
int InvertMatrix(const float *m, float *out);

// Affine inputs only; output may alias either input.
void Matrix4x4_ConcatTransforms(float out[4][4], float in1[4][4],
                                float in2[4][4]);

void VectorRotate(const vec3_t in1, const float in2[3][4], vec3_t out);
void VectorIRotate(const vec3_t in1, const float in2[3][4], vec3_t out);

bool PointInQuad(vec2_t p1, vec2_t p2, vec2_t p3, vec2_t p4, int x, int y);
float AngleMod(float a);

// Affine inputs only; out must not alias either input.
void ConcatTransforms(float in1[3][4], float in2[3][4], float out[3][4]);
// RGB channels in [0,255]; HSV hue degrees, saturation/value in [0,1].
void RGBToHSV(int r, int g, int b, float &h, float &s, float &v);
void HSVToRGB(float h, float s, float v, int &r, int &g, int &b);
Hsv RGBToHSV(Rgb rgb);
Rgb HSVToRGB(Hsv hsv);

} // namespace math
} // namespace abc

#include "detail/MathInl.h"

#ifdef ABC_MATH_RESTORE_MAX
#pragma pop_macro("max")
#undef ABC_MATH_RESTORE_MAX
#endif
#ifdef ABC_MATH_RESTORE_MIN
#pragma pop_macro("min")
#undef ABC_MATH_RESTORE_MIN
#endif
#ifdef ABC_MATH_RESTORE_CLAMP
#pragma pop_macro("clamp")
#undef ABC_MATH_RESTORE_CLAMP
#endif
