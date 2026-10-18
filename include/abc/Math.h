#pragma once

// abc::Math - vector/matrix math library.
// Ported from ABCEnchance src/core/library/mymathlib.h/.cpp (GBK -> UTF-8).
// Vector typedefs stay at global scope: engine/HLSDK headers (com_model.h,
// triangleapi.h, ...) expect unqualified vec2_t/vec3_t/mat4 and this library
// was the only provider of them in the original plugin.

#include <cstddef>

#undef clamp

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

using fixed4_t = int;
using fixed8_t = int;
using fixed16_t = int;

namespace abc {

namespace mathlib {
const float F_PI = 3.14159265358979323846f;
const double Q_PI = 3.14159265358979323846;
const double DOUBLE_PI = 2 * 3.14159265358979323846;
const float COLINEAR_EPSILON = 0.001f;
const int MAX_PLAYER_NAME_LENGTH = 128;
const double RADIAN_PER_DEGREE = 57.29577951308232087684;
const double DEGREE_PER_RADIAN = 0.017453292519943295769;
const float PERIGON_ANGLE = 360.0f;
const double SQRT_TWO = 1.414213562373095048801;
const double SQRT_THREE = 1.732050807568877293527;
const float FLAT_ANGLE = 180.0f;
const vec3_t vecZero = {0, 0, 0};
const float METERS_PER_INCH = 0.0254f;
const int nanmask = 255 << 23;
} // namespace mathlib

class CMathlib {
public:
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
  static float METER2INCH(float x);
  static float INCH2METER(float x);
  static void VectorSubtract(vec3_t a, vec3_t b, vec3_t c);
  static void VectorAdd(vec3_t a, vec3_t b, vec3_t c);
  static void VectorAddFloat(vec3_t a, float f);
  static void VectorMultipiler(vec3_t a, float f);
  static void VectorCopy(const vec3_t src, vec3_t dst);
  static void VectorReverse(vec3_t a);
  static void Q_Vector2Copy(vec2_t a, vec2_t b);
  static void VectorClear(vec3_t a);
  static float DotProduct(vec3_t x, vec3_t y);
  static float DotProduct(const vec3_t x, const vec3_t y);
  static void ColorCalcuAlpha(int &r, int &g, int &b, int a);
  template <typename T> static T max3(T a, T b, T c) {
    return (((a) > ((((b) > (c)) ? (b) : (c)))) ? (a)
                                                : ((((b) > (c)) ? (b) : (c))));
  }
  template <typename T> static T min3(T a, T b, T c) {
    return (((a) < ((((b) < (c)) ? (b) : (c)))) ? (a)
                                                : ((((b) < (c)) ? (b) : (c))));
  }
  static size_t GetScreenPixel(int length, double percent);
  static void Vector2Rotate(vec2_t out, float x, float y, float rotate);

  static void MatrixCopy(float in[4][3], float out[4][3]);
  static void VectorMA(const vec3_t veca, float scale, const vec3_t vecb,
                       vec3_t vecc);

  static vec_t _DotProduct(vec3_t v1, vec3_t v2);
  static void _VectorSubtract(vec3_t veca, vec3_t vecb, vec3_t out);
  static void _VectorAdd(vec3_t veca, vec3_t vecb, vec3_t out);
  static void _VectorCopy(vec3_t in, vec3_t out);

  static int VectorCompare(const vec3_t v1, const vec3_t v2);
  static float FVectorLength(vec3_t v);
  static double VectorLength(vec3_t v);
  static void CrossProduct(const vec3_t v1, const vec3_t v2, vec3_t cross);
  static float VectorNormalize(vec3_t v);
  static void VectorInverse(vec3_t v);
  static void VectorScale(const vec3_t in, vec_t scale, vec3_t out);
  // floor(log2(val)); non-positive input returns 0.
  static int Q_log2(int val);
  static void ClearBounds(vec3_t mins, vec3_t maxs);
  // Any finite normal; zero normal leaves p unchanged. Aliasing is supported.
  static void ProjectPointOnPlane(vec3_t dst, const vec3_t p,
                                  const vec3_t normal);
  static void PerpendicularVector(vec3_t dst, const vec3_t src);
  static vec_t Q_rint(vec_t in);
  static void _VectorScale(vec3_t v, vec_t scale, vec3_t out);
  static vec_t Length(const vec3_t v);
  static void AddPointToBounds(vec3_t v, vec3_t mins, vec3_t maxs);
  static void AngleIMatrix(const vec3_t angles, float matrix[3][4]);
  static void AngleQuaternion(const vec3_t angles, vec4_t quaternion);
  static void QuaternionMatrix(const vec4_t quaternion, float (*matrix)[4]);
  static void QuaternionSlerp(const vec4_t p, vec4_t q, float t, vec4_t qt);
  static void SinCos(float radians, float *sine, float *cosine);

  static void R_ConcatRotations(float in1[3][3], float in2[3][3],
                                float out[3][3]);
  static void R_ConcatTransforms(float in1[3][4], float in2[3][4],
                                 float out[3][4]);
  // Affine row-major matrices; angles are pitch/yaw/roll degrees. Writes
  // (0,0,0,1) last row.
  static void Matrix4x4_CreateFromEntity(float out[4][4], const vec3_t angles,
                                         const vec3_t origin, float scale);

  static void FloorDivMod(double numer, double denom, int *quotient, int *rem);
  static int GreatestCommonDivisor(int i1, int i2);
  static void RotatePointAroundVector(vec3_t dst, const vec3_t dir,
                                      const vec3_t point, float degrees);

  static void AngleVectors(const vec3_t angles, vec3_t forward, vec3_t right,
                           vec3_t up);
  // Legacy compatibility name: currently identical orientation to AngleVectors,
  // NOT a mathematical transpose. Dormant; do not use as an inverse matrix.
  static void AngleVectorsTranspose(const vec3_t angles, vec3_t *forward,
                                    vec3_t *right, vec3_t *up);
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

  static bool PointInPolygen(vec2_t p1, vec2_t p2, vec2_t p3, vec2_t p4, int x,
                             int y);
  static float anglemod(float a);

  static void ConcatTransforms(float in1[3][4], float in2[3][4],
                               float out[3][4]);
  // RGB channels in [0,255]; HSV hue degrees, saturation/value in [0,1].
  static void RGBToHSV(int r, int g, int b, float &h, float &s, float &v);
  static void HSVToRGB(float h, float s, float v, int &r, int &g, int &b);
};

} // namespace abc

#include "detail/MathInl.h"
