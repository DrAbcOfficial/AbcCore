#pragma once

// abc::Math - vector/matrix math library.
// Ported from ABCEnchance src/core/library/mymathlib.h/.cpp (GBK -> UTF-8).
// Vector typedefs stay at global scope: engine/HLSDK headers (com_model.h,
// triangleapi.h, ...) expect unqualified vec2_t/vec3_t/mat4 and this library
// was the only provider of them in the original plugin.

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

using mat3x4 = float[3][4];
using mat4 = float[4][4];
using vec4 = float[4];
using ivec4 = int[4];
using uvec4 = unsigned int[4];

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
  static std::size_t ScalePixelLength(int contentLength, double fraction);
  // Legacy name.
  static std::size_t GetScreenPixel(int length, double percent);
  static void Vector2Rotate(vec2_t out, float x, float y, float radians);

  static void MatrixCopy(float in[4][3], float out[4][3]);
  static void VectorMA(const vec3_t veca, float scale, const vec3_t vecb,
                       vec3_t vecc);

  static int VectorCompare(const vec3_t v1, const vec3_t v2);
  static float FVectorLength(vec3_t v);
  static double VectorLength(vec3_t v);
  static void CrossProduct(const vec3_t v1, const vec3_t v2, vec3_t cross);
  static float VectorNormalize(vec3_t v);
  static void VectorInverse(vec3_t v);
  static void VectorScale(const vec3_t in, vec_t scale, vec3_t out);
  // floor(log2(val)); non-positive input returns 0.
  static int Log2Floor(int val);
  // Legacy Quake-prefixed alias.
  static int Q_log2(int val);
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
