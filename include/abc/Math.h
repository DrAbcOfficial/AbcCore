#pragma once

// abc::Math - vector/matrix math library.
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

namespace mathlib {
inline constexpr float kPi = std::numbers::pi_v<float>;
inline constexpr double kPiDouble = std::numbers::pi;
inline constexpr float kDegToRad = std::numbers::pi_v<float> / 180.0f;
inline constexpr float kRadToDeg = 180.0f / std::numbers::pi_v<float>;
inline constexpr float kMetersPerInch = 0.0254f;

// Legacy spellings retained for source compatibility; new code should use the
// k-prefixed constants above.
[[deprecated("use kPi")]] inline constexpr float F_PI = kPi;
[[deprecated("use kPiDouble")]] inline constexpr double Q_PI = kPiDouble;
[[deprecated("use kMetersPerInch")]] inline constexpr float METERS_PER_INCH =
    kMetersPerInch;
} // namespace mathlib

class CMathlib {
public:
  static bool IsNaN(float x);
  static float DegToRad(float degrees);
  static float RadToDeg(float radians);

  // Legacy Quake-prefixed aliases.
  static bool Q_IS_NAN(float x);
  static float Q_DEG2RAD(float a);
  static float Q_RAD2DEG(float r);

  static float RANDOM_FLOAT(float low, float high);
  static int RANDOM_LONG(int low, int high);

  enum {
    Q_PITCH = 0, // up / down
    Q_YAW,       // left / right
    Q_ROLL       // fall over
  };
  static constexpr int kPitch = Q_PITCH;
  static constexpr int kYaw = Q_YAW;
  static constexpr int kRoll = Q_ROLL;
  static float METER2INCH(float x);
  static float INCH2METER(float x);
  static void VectorSubtract(vec3_t a, vec3_t b, vec3_t c);
  static void VectorAdd(vec3_t a, vec3_t b, vec3_t c);
  static void VectorAddFloat(vec3_t a, float f);
  static void VectorMultiply(vec3_t a, float f);
  // Legacy misspelling.
  static void VectorMultipiler(vec3_t a, float f);
  static void VectorCopy(const vec3_t src, vec3_t dst);
  // Legacy alias for VectorInverse.
  static void VectorReverse(vec3_t a);
  static void Vector2Copy(vec2_t src, vec2_t dst);
  // Legacy spelling.
  static void Q_Vector2Copy(vec2_t a, vec2_t b);
  static void VectorClear(vec3_t a);
  static float DotProduct(const vec3_t x, const vec3_t y);
  static void ColorApplyAlpha(int &r, int &g, int &b, int a);
  // Legacy misspelling.
  static void ColorCalcuAlpha(int &r, int &g, int &b, int a);
  // contentLength * clamp(fraction, 0, 1), truncated.
  static std::size_t ScalePixelLength(int contentLength, double fraction);
  // Legacy name.
  static std::size_t GetScreenPixel(int length, double percent);
  // Rotates (x, y) by radians.
  static void Vector2Rotate(vec2_t out, float x, float y, float radians);

  static void MatrixCopy(float in[4][3], float out[4][3]);
  static void VectorMA(const vec3_t veca, float scale, const vec3_t vecb,
                       vec3_t vecc);

  // Element-wise absolute comparison against float epsilon (not HLSDK 0.001).
  static int VectorCompare(const vec3_t v1, const vec3_t v2);
  // Single-precision length.
  static float FVectorLength(const vec3_t v);
  // Double-precision length.
  static double VectorLength(const vec3_t v);
  static void CrossProduct(const vec3_t v1, const vec3_t v2, vec3_t cross);
  static float VectorNormalize(vec3_t v);
  static void VectorInverse(vec3_t v);
  static void VectorScale(const vec3_t in, vec_t scale, vec3_t out);
  // floor(log2(val)); non-positive input returns 0.
  static int Log2Floor(int val);
  // Legacy Quake-prefixed alias.
  static int Q_log2(int val);
  // Initializes mins to 99999 and maxs to -99999.
  static void ResetBounds(vec3_t mins, vec3_t maxs);
  // Legacy name; resets to sentinel bounds, it does not zero them.
  static void ClearBounds(vec3_t mins, vec3_t maxs);
  // Any finite normal; zero normal leaves p unchanged. Aliasing is supported.
  static void ProjectPointOnPlane(vec3_t dst, const vec3_t p,
                                  const vec3_t normal);
  static void PerpendicularVector(vec3_t dst, const vec3_t src);
  static vec_t RoundHalfUp(vec_t in);
  // Legacy Quake-prefixed alias.
  static vec_t Q_rint(vec_t in);
  // Legacy alias for FVectorLength.
  static vec_t Length(const vec3_t v);
  static void AddPointToBounds(vec3_t v, vec3_t mins, vec3_t maxs);
  // Legacy GoldSrc angle order: angles[0] is roll, angles[1] is pitch and
  // angles[2] is yaw. Prefer Matrix4x4_CreateFromEntity for engine
  // [pitch, yaw, roll] angles.
  static void AngleIMatrix(const vec3_t angles, float matrix[3][4]);
  // Angles are pitch/yaw/roll degrees, matching AngleVectors.
  static void AngleQuaternion(const vec3_t angles, vec4_t quaternion);
  static void QuaternionMatrix(const vec4_t quaternion, float (*matrix)[4]);
  // Does not modify p or q.
  static void QuaternionSlerp(const vec4_t p, const vec4_t q, float t,
                              vec4_t qt);
  static void SinCos(float radians, float *sine, float *cosine);

  static void R_ConcatRotations(float in1[3][3], float in2[3][3],
                                float out[3][3]);
  // Legacy Renderer-style alias for ConcatTransforms.
  static void R_ConcatTransforms(float in1[3][4], float in2[3][4],
                                 float out[3][4]);
  // Affine row-major matrices; angles are pitch/yaw/roll degrees. Writes
  // (0,0,0,1) last row.
  static void Matrix4x4_CreateFromEntity(float out[4][4], const vec3_t angles,
                                         const vec3_t origin, float scale);

  static void FloorDivMod(double numer, double denom, int *quotient, int *rem);
  // Euclidean gcd; non-negative result, negative inputs accepted.
  static int GreatestCommonDivisor(int i1, int i2);
  static void RotatePointAroundVector(vec3_t dst, const vec3_t dir,
                                      const vec3_t point, float degrees);

  static void AngleVectors(const vec3_t angles, vec3_t forward, vec3_t right,
                           vec3_t up);
  // GoldSrc transpose: forward/right/up are the rows of the rotation matrix
  // produced by Matrix4x4_CreateFromEntity for the same angles.
  static void AngleVectorsTranspose(const vec3_t angles, vec3_t *forward,
                                    vec3_t *right, vec3_t *up);
  // See AngleIMatrix for the legacy GoldSrc angle order.
  static void AngleMatrix(const vec3_t angles, float (*matrix)[4]);
  static void VectorTransform(const vec3_t in1, float in2[3][4], vec3_t out);

  static void VectorAngles(const vec3_t forward, vec3_t angles);

  // 4x4 inverse; returns 0 for a singular matrix.
  static int InvertMatrix(const float *m, float *out);

  // Affine inputs only; output may alias either input.
  static void Matrix4x4_ConcatTransforms(float out[4][4], float in1[4][4],
                                         float in2[4][4]);

  static void VectorRotate(const vec3_t in1, const float in2[3][4], vec3_t out);
  static void VectorIRotate(const vec3_t in1, const float in2[3][4],
                            vec3_t out);

  static bool PointInQuad(vec2_t p1, vec2_t p2, vec2_t p3, vec2_t p4, int x,
                          int y);
  // Legacy misspelling.
  static bool PointInPolygen(vec2_t p1, vec2_t p2, vec2_t p3, vec2_t p4, int x,
                             int y);
  static float AngleMod(float a);
  // Legacy spelling.
  static float anglemod(float a);

  // Affine inputs only; out must not alias either input.
  static void ConcatTransforms(float in1[3][4], float in2[3][4],
                               float out[3][4]);
  // RGB channels in [0,255]; HSV hue degrees, saturation/value in [0,1].
  static void RGBToHSV(int r, int g, int b, float &h, float &s, float &v);
  static void HSVToRGB(float h, float s, float v, int &r, int &g, int &b);
};

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

// Namespace facade for new code. CMathlib remains the compatibility surface;
// these forwards let callers use abc::math::VectorCopy(...) and abc::math::kPi.
namespace abc {
namespace math {
using mathlib::kDegToRad;
using mathlib::kMetersPerInch;
using mathlib::kPi;
using mathlib::kPiDouble;
using mathlib::kRadToDeg;
inline constexpr int kPitch = CMathlib::kPitch;
inline constexpr int kYaw = CMathlib::kYaw;
inline constexpr int kRoll = CMathlib::kRoll;
inline constexpr int Q_PITCH = CMathlib::Q_PITCH;
inline constexpr int Q_YAW = CMathlib::Q_YAW;
inline constexpr int Q_ROLL = CMathlib::Q_ROLL;

#define ABC_MATH_FORWARD(ret, name, params, args)                              \
  inline ret name params { return CMathlib::name args; }
ABC_MATH_FORWARD(bool, IsNaN, (float x), (x))
ABC_MATH_FORWARD(float, DegToRad, (float degrees), (degrees))
ABC_MATH_FORWARD(float, RadToDeg, (float radians), (radians))
ABC_MATH_FORWARD(bool, Q_IS_NAN, (float x), (x))
ABC_MATH_FORWARD(float, Q_DEG2RAD, (float a), (a))
ABC_MATH_FORWARD(float, Q_RAD2DEG, (float r), (r))
ABC_MATH_FORWARD(float, RANDOM_FLOAT, (float low, float high), (low, high))
ABC_MATH_FORWARD(int, RANDOM_LONG, (int low, int high), (low, high))
ABC_MATH_FORWARD(float, METER2INCH, (float x), (x))
ABC_MATH_FORWARD(float, INCH2METER, (float x), (x))
ABC_MATH_FORWARD(void, VectorSubtract, (vec3_t a, vec3_t b, vec3_t c), (a, b, c))
ABC_MATH_FORWARD(void, VectorAdd, (vec3_t a, vec3_t b, vec3_t c), (a, b, c))
ABC_MATH_FORWARD(void, VectorAddFloat, (vec3_t a, float f), (a, f))
ABC_MATH_FORWARD(void, VectorMultiply, (vec3_t a, float f), (a, f))
ABC_MATH_FORWARD(void, VectorMultipiler, (vec3_t a, float f), (a, f))
ABC_MATH_FORWARD(void, VectorCopy, (const vec3_t src, vec3_t dst), (src, dst))
ABC_MATH_FORWARD(void, VectorReverse, (vec3_t a), (a))
ABC_MATH_FORWARD(void, Vector2Copy, (vec2_t src, vec2_t dst), (src, dst))
ABC_MATH_FORWARD(void, Q_Vector2Copy, (vec2_t a, vec2_t b), (a, b))
ABC_MATH_FORWARD(void, VectorClear, (vec3_t a), (a))
ABC_MATH_FORWARD(float, DotProduct, (const vec3_t x, const vec3_t y), (x, y))
ABC_MATH_FORWARD(void, ColorApplyAlpha, (int &r, int &g, int &b, int a), (r, g, b, a))
ABC_MATH_FORWARD(void, ColorCalcuAlpha, (int &r, int &g, int &b, int a), (r, g, b, a))
ABC_MATH_FORWARD(std::size_t, ScalePixelLength,
                 (int contentLength, double fraction), (contentLength, fraction))
ABC_MATH_FORWARD(std::size_t, GetScreenPixel,
                 (int length, double percent), (length, percent))
ABC_MATH_FORWARD(void, Vector2Rotate,
                 (vec2_t out, float x, float y, float radians), (out, x, y, radians))
ABC_MATH_FORWARD(void, MatrixCopy, (float in[4][3], float out[4][3]), (in, out))
ABC_MATH_FORWARD(void, VectorMA,
                 (const vec3_t veca, float scale, const vec3_t vecb, vec3_t vecc), (veca, scale, vecb, vecc))
ABC_MATH_FORWARD(int, VectorCompare, (const vec3_t v1, const vec3_t v2), (v1, v2))
ABC_MATH_FORWARD(float, FVectorLength, (const vec3_t v), (v))
ABC_MATH_FORWARD(double, VectorLength, (const vec3_t v), (v))
ABC_MATH_FORWARD(void, CrossProduct,
                 (const vec3_t v1, const vec3_t v2, vec3_t cross), (v1, v2, cross))
ABC_MATH_FORWARD(float, VectorNormalize, (vec3_t v), (v))
ABC_MATH_FORWARD(void, VectorInverse, (vec3_t v), (v))
ABC_MATH_FORWARD(void, VectorScale,
                 (const vec3_t in, vec_t scale, vec3_t out), (in, scale, out))
ABC_MATH_FORWARD(int, Log2Floor, (int val), (val))
ABC_MATH_FORWARD(int, Q_log2, (int val), (val))
ABC_MATH_FORWARD(void, ResetBounds, (vec3_t mins, vec3_t maxs), (mins, maxs))
ABC_MATH_FORWARD(void, ClearBounds, (vec3_t mins, vec3_t maxs), (mins, maxs))
ABC_MATH_FORWARD(void, ProjectPointOnPlane,
                 (vec3_t dst, const vec3_t p, const vec3_t normal), (dst, p, normal))
ABC_MATH_FORWARD(void, PerpendicularVector, (vec3_t dst, const vec3_t src), (dst, src))
ABC_MATH_FORWARD(vec_t, RoundHalfUp, (vec_t in), (in))
ABC_MATH_FORWARD(vec_t, Q_rint, (vec_t in), (in))
ABC_MATH_FORWARD(vec_t, Length, (const vec3_t v), (v))
ABC_MATH_FORWARD(void, AddPointToBounds,
                 (vec3_t v, vec3_t mins, vec3_t maxs), (v, mins, maxs))
ABC_MATH_FORWARD(void, AngleIMatrix,
                 (const vec3_t angles, float matrix[3][4]), (angles, matrix))
ABC_MATH_FORWARD(void, AngleQuaternion,
                 (const vec3_t angles, vec4_t quaternion), (angles, quaternion))
ABC_MATH_FORWARD(void, QuaternionMatrix,
                 (const vec4_t quaternion, float (*matrix)[4]), (quaternion, matrix))
ABC_MATH_FORWARD(void, QuaternionSlerp,
                 (const vec4_t p, const vec4_t q, float t, vec4_t qt), (p, q, t, qt))
ABC_MATH_FORWARD(void, SinCos,
                 (float radians, float *sine, float *cosine), (radians, sine, cosine))
ABC_MATH_FORWARD(void, R_ConcatRotations,
                 (float in1[3][3], float in2[3][3], float out[3][3]), (in1, in2, out))
ABC_MATH_FORWARD(void, R_ConcatTransforms,
                 (float in1[3][4], float in2[3][4], float out[3][4]), (in1, in2, out))
ABC_MATH_FORWARD(void, Matrix4x4_CreateFromEntity,
                 (float out[4][4], const vec3_t angles, const vec3_t origin, float scale), (out, angles, origin, scale))
ABC_MATH_FORWARD(void, FloorDivMod,
                 (double numer, double denom, int *quotient, int *rem), (numer, denom, quotient, rem))
ABC_MATH_FORWARD(int, GreatestCommonDivisor, (int i1, int i2), (i1, i2))
ABC_MATH_FORWARD(void, RotatePointAroundVector,
                 (vec3_t dst, const vec3_t dir, const vec3_t point, float degrees), (dst, dir, point, degrees))
ABC_MATH_FORWARD(void, AngleVectors,
                 (const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up), (angles, forward, right, up))
ABC_MATH_FORWARD(void, AngleVectorsTranspose,
                 (const vec3_t angles, vec3_t *forward, vec3_t *right, vec3_t *up), (angles, forward, right, up))
ABC_MATH_FORWARD(void, AngleMatrix,
                 (const vec3_t angles, float (*matrix)[4]), (angles, matrix))
ABC_MATH_FORWARD(void, VectorTransform,
                 (const vec3_t in1, float in2[3][4], vec3_t out), (in1, in2, out))
ABC_MATH_FORWARD(void, VectorAngles,
                 (const vec3_t forward, vec3_t angles), (forward, angles))
ABC_MATH_FORWARD(int, InvertMatrix, (const float *m, float *out), (m, out))
ABC_MATH_FORWARD(void, Matrix4x4_ConcatTransforms,
                 (float out[4][4], float in1[4][4], float in2[4][4]), (out, in1, in2))
ABC_MATH_FORWARD(void, VectorRotate,
                 (const vec3_t in1, const float in2[3][4], vec3_t out), (in1, in2, out))
ABC_MATH_FORWARD(void, VectorIRotate,
                 (const vec3_t in1, const float in2[3][4], vec3_t out), (in1, in2, out))
ABC_MATH_FORWARD(bool, PointInQuad,
                 (vec2_t p1, vec2_t p2, vec2_t p3, vec2_t p4, int x, int y), (p1, p2, p3, p4, x, y))
ABC_MATH_FORWARD(bool, PointInPolygen,
                 (vec2_t p1, vec2_t p2, vec2_t p3, vec2_t p4, int x, int y), (p1, p2, p3, p4, x, y))
ABC_MATH_FORWARD(float, AngleMod, (float a), (a))
ABC_MATH_FORWARD(float, anglemod, (float a), (a))
ABC_MATH_FORWARD(void, ConcatTransforms,
                 (float in1[3][4], float in2[3][4], float out[3][4]), (in1, in2, out))
ABC_MATH_FORWARD(void, RGBToHSV,
                 (int r, int g, int b, float &h, float &s, float &v), (r, g, b, h, s, v))
ABC_MATH_FORWARD(void, HSVToRGB,
                 (float h, float s, float v, int &r, int &g, int &b), (h, s, v, r, g, b))
#undef ABC_MATH_FORWARD
} // namespace math
} // namespace abc
