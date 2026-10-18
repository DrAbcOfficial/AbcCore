#pragma once

// Inline CMathlib implementation. Kept header-only so classic plugins do not
// import C++ methods from AbcCore.dll.

#include <algorithm>
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <numeric>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <metahook.h>
#ifdef min
#pragma push_macro("min")
#undef min
#define ABC_MATH_INL_RESTORE_MIN
#endif
#ifdef max
#pragma push_macro("max")
#undef max
#define ABC_MATH_INL_RESTORE_MAX
#endif

namespace abc {
inline bool CMathlib::Q_IS_NAN(float x) {
  // Strict NaN: exponent all ones AND a non-zero mantissa. The old
  // pointer-pun also reported +/-Inf as NaN and violated strict aliasing.
  const auto bits = std::bit_cast<std::uint32_t>(x);
  return (bits & 0x7F800000u) == 0x7F800000u && (bits & 0x007FFFFFu) != 0;
}
inline float CMathlib::Q_DEG2RAD(float a) {
  return (a * mathlib::F_PI) / 180.0f;
}
inline float CMathlib::Q_RAD2DEG(float r) {
  return (r * 180.0f) / mathlib::F_PI;
}
inline float CMathlib::RANDOM_FLOAT(float low, float high) {
  return gEngfuncs.pfnRandomFloat(low, high);
}
inline int CMathlib::RANDOM_LONG(int low, int high) {
  return gEngfuncs.pfnRandomLong(low, high);
}
inline float CMathlib::METER2INCH(float x) {
  return (float)(x * (1.0f / mathlib::METERS_PER_INCH));
}
inline float CMathlib::INCH2METER(float x) {
  return (float)(x * (mathlib::METERS_PER_INCH / 1.0f));
}
inline void CMathlib::VectorSubtract(vec3_t a, vec3_t b, vec3_t c) {
  (c)[0] = (a)[0] - (b)[0];
  (c)[1] = (a)[1] - (b)[1];
  (c)[2] = (a)[2] - (b)[2];
}
inline void CMathlib::VectorAdd(vec3_t a, vec3_t b, vec3_t c) {
  (c)[0] = (a)[0] + (b)[0];
  (c)[1] = (a)[1] + (b)[1];
  (c)[2] = (a)[2] + (b)[2];
}
inline void CMathlib::VectorAddFloat(vec3_t a, float f) {
  a[0] += f;
  a[1] += f;
  a[2] += f;
}
inline void CMathlib::VectorMultipiler(vec3_t a, float f) {
  a[0] *= f;
  a[1] *= f;
  a[2] *= f;
}
inline void CMathlib::VectorCopy(const vec3_t src, vec3_t dst) {
  (dst)[0] = (src)[0];
  (dst)[1] = (src)[1];
  (dst)[2] = (src)[2];
}
inline void CMathlib::VectorReverse(vec3_t a) {
  (a[0]) = -a[0];
  (a[1]) = -a[1];
  (a[2]) = -a[2];
}
inline void CMathlib::Q_Vector2Copy(vec2_t a, vec2_t b) {
  (b)[0] = (a)[0];
  (b)[1] = (a)[1];
}
inline void CMathlib::VectorClear(vec3_t a) {
  (a)[0] = (a)[1] = (a)[2] = 0.0;
}
inline float CMathlib::DotProduct(vec3_t x, vec3_t y) {
  return ((x)[0] * (y)[0] + (x)[1] * (y)[1] + (x)[2] * (y)[2]);
}
inline float CMathlib::DotProduct(const vec3_t x, const vec3_t y) {
  return ((x)[0] * (y)[0] + (x)[1] * (y)[1] + (x)[2] * (y)[2]);
}
inline void CMathlib::ColorCalcuAlpha(int &r, int &g, int &b, int a) {
  const float alpha = std::clamp(a, 0, 255) / 255.0f;
  r = static_cast<int>(r * alpha);
  g = static_cast<int>(g * alpha);
  b = static_cast<int>(b * alpha);
}
inline size_t CMathlib::GetScreenPixel(int length, double percent) {
  return (size_t)((float)length *
                  std::clamp(static_cast<float>(percent), 0.0f, 1.0f));
}
inline void CMathlib::Vector2Rotate(vec2_t out, float x, float y,
                                    float rotate) {
  out[0] = x * cos(rotate) - y * sin(rotate);
  out[1] = x * sin(rotate) + y * cos(rotate);
}
inline void CMathlib::ProjectPointOnPlane(vec3_t dst, const vec3_t p,
                                          const vec3_t normal) {
  const float denominator = DotProduct(normal, normal);
  const float d = denominator > 0 ? DotProduct(normal, p) / denominator : 0;
  vec3_t projected = {p[0] - d * normal[0], p[1] - d * normal[1],
                      p[2] - d * normal[2]};
  VectorCopy(projected, dst);
}
inline void CMathlib::PerpendicularVector(vec3_t dst, const vec3_t src) {
  int pos;
  int i;
  float minelem = 1.0F;
  vec3_t tempvec;

  for (pos = 0, i = 0; i < 3; i++) {
    if (fabs(src[i]) < minelem) {
      pos = i;
      minelem = fabs(src[i]);
    }
  }

  tempvec[0] = tempvec[1] = tempvec[2] = 0.0F;
  tempvec[pos] = 1.0F;

  ProjectPointOnPlane(dst, tempvec, src);

  CMathlib::VectorNormalize(dst);
}
inline bool CMathlib::PointInPolygen(vec2_t p1, vec2_t p2, vec2_t p3, vec2_t p4,
                                     int x, int y) {
  float z1 = (p2[0] - p1[0]) * (y - p1[1]) - (x - p1[0]) * (p2[1] - p1[1]);
  float z2 = (p3[0] - p2[0]) * (y - p2[1]) - (x - p2[0]) * (p3[1] - p2[1]);
  float z3 = (p4[0] - p3[0]) * (y - p3[1]) - (x - p3[0]) * (p4[1] - p3[1]);
  float z4 = (p1[0] - p4[0]) * (y - p4[1]) - (x - p4[0]) * (p1[1] - p4[1]);
  return z1 * z2 > 0 && z3 * z4 > 0 && z1 * z3 > 0;
}

#ifdef _WIN32
#pragma optimize("", off)
#endif

// Original author disabled optimizer for this function to work around a
// miscompilation; kept as-is.
inline void CMathlib::RotatePointAroundVector(vec3_t dst, const vec3_t dir,
                                              const vec3_t point,
                                              float degrees) {
  float m[3][3];
  float im[3][3];
  float zrot[3][3];
  float tmpmat[3][3];
  float rot[3][3];
  int i;
  vec3_t vr, vup, vf;

  vf[0] = dir[0];
  vf[1] = dir[1];
  vf[2] = dir[2];

  PerpendicularVector(vr, dir);
  CMathlib::CrossProduct(vr, vf, vup);

  m[0][0] = vr[0];
  m[1][0] = vr[1];
  m[2][0] = vr[2];

  m[0][1] = vup[0];
  m[1][1] = vup[1];
  m[2][1] = vup[2];

  m[0][2] = vf[0];
  m[1][2] = vf[1];
  m[2][2] = vf[2];

  memcpy(im, m, sizeof(im));

  im[0][1] = m[1][0];
  im[0][2] = m[2][0];
  im[1][0] = m[0][1];
  im[1][2] = m[2][1];
  im[2][0] = m[0][2];
  im[2][1] = m[1][2];

  memset(zrot, 0, sizeof(zrot));
  zrot[0][0] = zrot[1][1] = zrot[2][2] = 1.0F;

  zrot[0][0] = cos(CMathlib::Q_DEG2RAD(degrees));
  zrot[0][1] = sin(CMathlib::Q_DEG2RAD(degrees));
  zrot[1][0] = -sin(CMathlib::Q_DEG2RAD(degrees));
  zrot[1][1] = cos(CMathlib::Q_DEG2RAD(degrees));

  CMathlib::R_ConcatRotations(m, zrot, tmpmat);
  CMathlib::R_ConcatRotations(tmpmat, im, rot);

  for (i = 0; i < 3; i++)
    dst[i] = rot[i][0] * point[0] + rot[i][1] * point[1] + rot[i][2] * point[2];
}

#ifdef _WIN32
#pragma optimize("", on)
#endif

inline float CMathlib::anglemod(float a) {
  a = (360.0f / 65536) * ((int)(a * (65536 / 360.0f)) & 65535);
  return a;
}
inline void CMathlib::AngleVectors(const vec3_t angles, vec3_t forward,
                                   vec3_t right, vec3_t up) {
  float angle;
  vec2_t cs[3];
  for (int i = 0; i < 3; i++) {
    angle = angles[i] * (mathlib::F_PI / 180.0f);
    cs[i][0] = sin(angle);
    cs[i][1] = cos(angle);
  }
  if (forward) {
    forward[0] = cs[0][1] * cs[1][1];
    forward[1] = cs[0][1] * cs[1][0];
    forward[2] = -cs[0][0];
  }
  if (right) {
    right[0] =
        (-1 * cs[2][0] * cs[0][0] * cs[1][1] + -1 * cs[2][1] * -cs[1][0]);
    right[1] = (-1 * cs[2][0] * cs[0][0] * cs[1][0] + -1 * cs[2][1] * cs[1][1]);
    right[2] = -1 * cs[2][0] * cs[0][1];
  }
  if (up) {
    up[0] = (cs[2][1] * cs[0][0] * cs[1][1] + -cs[2][0] * -cs[1][0]);
    up[1] = (cs[2][1] * cs[0][0] * cs[1][0] + -cs[2][0] * cs[1][1]);
    up[2] = cs[2][1] * cs[0][1];
  }
}
inline void CMathlib::AngleVectorsTranspose(const vec3_t angles,
                                            vec3_t *forward, vec3_t *right,
                                            vec3_t *up) {
  float angle;
  vec2_t cs[3];
  for (int i = 0; i < 3; i++) {
    angle = angles[i] * (mathlib::F_PI / 180.0f);
    cs[i][0] = sin(angle);
    cs[i][1] = cos(angle);
  }
  if (forward) {
    (*forward)[0] = cs[0][1] * cs[1][1];
    (*forward)[1] = cs[0][1] * cs[1][0];
    (*forward)[2] = -cs[0][0];
  }
  if (right) {
    (*right)[0] =
        (-1 * cs[2][0] * cs[0][0] * cs[1][1] + -1 * cs[2][1] * -cs[1][0]);
    (*right)[1] =
        (-1 * cs[2][0] * cs[0][0] * cs[1][0] + -1 * cs[2][1] * cs[1][1]);
    (*right)[2] = -1 * cs[2][0] * cs[0][1];
  }
  if (up) {
    (*up)[0] = (cs[2][1] * cs[0][0] * cs[1][1] + -cs[2][0] * -cs[1][0]);
    (*up)[1] = (cs[2][1] * cs[0][0] * cs[1][0] + -cs[2][0] * cs[1][1]);
    (*up)[2] = cs[2][1] * cs[0][1];
  }
}
inline void CMathlib::VectorAngles(const vec3_t forward, vec3_t angles) {
  float yaw, pitch;

  if (forward[1] == 0 && forward[0] == 0) {
    yaw = 0;

    if (forward[2] > 0)
      pitch = 90;
    else
      pitch = 270;
  } else {
    yaw = static_cast<float>(
        (atan2((double)forward[1], (double)forward[0]) * 180 / mathlib::Q_PI));

    if (yaw < 0)
      yaw += 360;

    float tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
    pitch = static_cast<float>(atan2((double)forward[2], (double)tmp) * 180 /
                               mathlib::Q_PI);

    if (pitch < 0)
      pitch += 360;
  }

  angles[0] = pitch;
  angles[1] = yaw;
  angles[2] = 0;
}
inline float CMathlib::FVectorLength(vec3_t v) {
  float length = 0;
  for (int i = 0; i < 3; i++)
    length += static_cast<float>(pow(v[i], 2));
  return sqrt(length);
}
inline double CMathlib::VectorLength(vec3_t v) {
  double length = 0;
  for (int i = 0; i < 3; i++)
    length += pow(v[i], 2);
  return sqrt(length);
}

#ifndef FLT_EPSILON
#define FLT_EPSILON (1.192093e-007)
#endif

inline int CMathlib::VectorCompare(const vec3_t v1, const vec3_t v2) {
  for (int i = 0; i < 3; i++) {
    if (abs(v1[i] - v2[i]) > FLT_EPSILON)
      return 0;
  }
  return 1;
}
inline vec_t CMathlib::Q_rint(vec_t in) {
  return static_cast<float>(floor(in + 0.5f));
}
inline void CMathlib::VectorMA(const vec3_t va, float scale, const vec3_t vb,
                               vec3_t vc) {
  vc[0] = va[0] + scale * vb[0];
  vc[1] = va[1] + scale * vb[1];
  vc[2] = va[2] + scale * vb[2];
}
inline void CMathlib::CrossProduct(const vec3_t v1, const vec3_t v2,
                                   vec3_t cross) {
  cross[0] = v1[1] * v2[2] - v1[2] * v2[1];
  cross[1] = v1[2] * v2[0] - v1[0] * v2[2];
  cross[2] = v1[0] * v2[1] - v1[1] * v2[0];
}
inline vec_t CMathlib::_DotProduct(vec3_t v1, vec3_t v2) {
  return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}
inline void CMathlib::_VectorSubtract(vec3_t va, vec3_t vb, vec3_t out) {
  out[0] = va[0] - vb[0];
  out[1] = va[1] - vb[1];
  out[2] = va[2] - vb[2];
}
inline void CMathlib::_VectorAdd(vec3_t va, vec3_t vb, vec3_t out) {
  out[0] = va[0] + vb[0];
  out[1] = va[1] + vb[1];
  out[2] = va[2] + vb[2];
}
inline void CMathlib::_VectorCopy(vec3_t in, vec3_t out) {
  out[0] = in[0];
  out[1] = in[1];
  out[2] = in[2];
}
inline void CMathlib::_VectorScale(vec3_t v, vec_t scale, vec3_t out) {
  out[0] = v[0] * scale;
  out[1] = v[1] * scale;
  out[2] = v[2] * scale;
}
inline vec_t CMathlib::Length(const vec3_t v) {
  int i;
  float length;

  length = 0;

  for (i = 0; i < 3; i++)
    length += v[i] * v[i];

  length = sqrt(length);
  return length;
}
inline vec_t CMathlib::VectorNormalize(vec3_t v) {
  int i = 0;
  double length;

  length = 0;
  for (i = 0; i < 3; i++)
    length += pow(v[i], 2);

  length = sqrt(length);

  if (length == 0)
    return 0;

  for (i = 0; i < 3; i++)
    v[i] /= static_cast<float>(length);

  return static_cast<float>(length);
}
inline void CMathlib::VectorInverse(vec3_t v) {
  v[0] = -v[0];
  v[1] = -v[1];
  v[2] = -v[2];
}
inline void CMathlib::VectorScale(const vec3_t in, vec_t scale, vec3_t out) {
  out[0] = in[0] * scale;
  out[1] = in[1] * scale;
  out[2] = in[2] * scale;
}
inline int CMathlib::Q_log2(int val) {
  if (val <= 0)
    return 0;
  int answer = 0;

  while (val >>= 1)
    answer++;

  return answer;
}
inline void CMathlib::ClearBounds(vec3_t mins, vec3_t maxs) {
  mins[0] = mins[1] = mins[2] = 99999;
  maxs[0] = maxs[1] = maxs[2] = -99999;
}
inline void CMathlib::MatrixCopy(float in[4][3], float out[4][3]) {
  memcpy(out, in, sizeof(float) * 3 * 4);
}
inline void CMathlib::AddPointToBounds(vec3_t v, vec3_t mins, vec3_t maxs) {
  vec_t val;
  for (int i = 0; i < 3; i++) {
    val = v[i];

    if (val < mins[i])
      mins[i] = val;

    if (val > maxs[i])
      maxs[i] = val;
  }
}
inline void CMathlib::AngleMatrix(const vec3_t angles, float (*matrix)[4]) {
  float angle;
  float sr, sp, sy, cr, cp, cy;

  angle = angles[2] * (mathlib::F_PI * 2 / 360);
  sy = sin(angle);
  cy = cos(angle);
  angle = angles[1] * (mathlib::F_PI * 2 / 360);
  sp = sin(angle);
  cp = cos(angle);
  angle = angles[0] * (mathlib::F_PI * 2 / 360);
  sr = sin(angle);
  cr = cos(angle);

  matrix[0][0] = cp * cy;
  matrix[1][0] = cp * sy;
  matrix[2][0] = -sp;
  matrix[0][1] = sr * sp * cy + cr * -sy;
  matrix[1][1] = sr * sp * sy + cr * cy;
  matrix[2][1] = sr * cp;
  matrix[0][2] = (cr * sp * cy + -sr * -sy);
  matrix[1][2] = (cr * sp * sy + -sr * cy);
  matrix[2][2] = cr * cp;
  matrix[0][3] = 0.0;
  matrix[1][3] = 0.0;
  matrix[2][3] = 0.0;
}
inline void CMathlib::AngleIMatrix(const vec3_t angles, float matrix[3][4]) {
  float angle;
  float sr, sp, sy, cr, cp, cy;

  angle = angles[2] * (mathlib::F_PI * 2 / 360);
  sy = sin(angle);
  cy = cos(angle);
  angle = angles[1] * (mathlib::F_PI * 2 / 360);
  sp = sin(angle);
  cp = cos(angle);
  angle = angles[0] * (mathlib::F_PI * 2 / 360);
  sr = sin(angle);
  cr = cos(angle);

  matrix[0][0] = cp * cy;
  matrix[0][1] = cp * sy;
  matrix[0][2] = -sp;
  matrix[1][0] = sr * sp * cy + cr * -sy;
  matrix[1][1] = sr * sp * sy + cr * cy;
  matrix[1][2] = sr * cp;
  matrix[2][0] = (cr * sp * cy + -sr * -sy);
  matrix[2][1] = (cr * sp * sy + -sr * cy);
  matrix[2][2] = cr * cp;
  matrix[0][3] = 0.0;
  matrix[1][3] = 0.0;
  matrix[2][3] = 0.0;
}
inline void CMathlib::R_ConcatRotations(float in1[3][3], float in2[3][3],
                                        float out[3][3]) {
  out[0][0] =
      in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
  out[0][1] =
      in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
  out[0][2] =
      in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
  out[1][0] =
      in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
  out[1][1] =
      in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
  out[1][2] =
      in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
  out[2][0] =
      in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
  out[2][1] =
      in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
  out[2][2] =
      in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
}
inline void CMathlib::R_ConcatTransforms(float in1[3][4], float in2[3][4],
                                         float out[3][4]) {
  out[0][0] =
      in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
  out[0][1] =
      in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
  out[0][2] =
      in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
  out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
              in1[0][2] * in2[2][3] + in1[0][3];
  out[1][0] =
      in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
  out[1][1] =
      in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
  out[1][2] =
      in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
  out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
              in1[1][2] * in2[2][3] + in1[1][3];
  out[2][0] =
      in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
  out[2][1] =
      in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
  out[2][2] =
      in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
  out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
              in1[2][2] * in2[2][3] + in1[2][3];
}
inline void CMathlib::VectorRotate(const vec3_t in1, const float in2[3][4],
                                   vec3_t out) {
  out[0] = CMathlib::DotProduct(in1, in2[0]);
  out[1] = CMathlib::DotProduct(in1, in2[1]);
  out[2] = CMathlib::DotProduct(in1, in2[2]);
}
inline void CMathlib::VectorIRotate(const vec3_t in1, const float in2[3][4],
                                    vec3_t out) {
  out[0] = in1[0] * in2[0][0] + in1[1] * in2[1][0] + in1[2] * in2[2][0];
  out[1] = in1[0] * in2[0][1] + in1[1] * in2[1][1] + in1[2] * in2[2][1];
  out[2] = in1[0] * in2[0][2] + in1[1] * in2[1][2] + in1[2] * in2[2][2];
}
inline void CMathlib::VectorTransform(const vec3_t in1, float in2[3][4],
                                      vec3_t out) {
  out[0] = DotProduct(in1, in2[0]) + in2[0][3];
  out[1] = DotProduct(in1, in2[1]) + in2[1][3];
  out[2] = DotProduct(in1, in2[2]) + in2[2][3];
}
inline void CMathlib::AngleQuaternion(const vec3_t angles, vec4_t quaternion) {
  float angle;
  float sr, sp, sy, cr, cp, cy;

  angle = angles[2] * 0.5f;
  sy = sin(angle);
  cy = cos(angle);
  angle = angles[1] * 0.5f;
  sp = sin(angle);
  cp = cos(angle);
  angle = angles[0] * 0.5f;
  sr = sin(angle);
  cr = cos(angle);

  quaternion[0] = sr * cp * cy - cr * sp * sy;
  quaternion[1] = cr * sp * cy + sr * cp * sy;
  quaternion[2] = cr * cp * sy - sr * sp * cy;
  quaternion[3] = cr * cp * cy + sr * sp * sy;
}
inline void CMathlib::QuaternionMatrix(const vec4_t quaternion,
                                       float (*matrix)[4]) {
  matrix[0][0] = 1.0f - 2.0f * quaternion[1] * quaternion[1] -
                 2.0f * quaternion[2] * quaternion[2];
  matrix[1][0] = 2.0f * quaternion[0] * quaternion[1] +
                 2.0f * quaternion[3] * quaternion[2];
  matrix[2][0] = 2.0f * quaternion[0] * quaternion[2] -
                 2.0f * quaternion[3] * quaternion[1];

  matrix[0][1] = 2.0f * quaternion[0] * quaternion[1] -
                 2.0f * quaternion[3] * quaternion[2];
  matrix[1][1] = 1.0f - 2.0f * quaternion[0] * quaternion[0] -
                 2.0f * quaternion[2] * quaternion[2];
  matrix[2][1] = 2.0f * quaternion[1] * quaternion[2] +
                 2.0f * quaternion[3] * quaternion[0];

  matrix[0][2] = 2.0f * quaternion[0] * quaternion[2] +
                 2.0f * quaternion[3] * quaternion[1];
  matrix[1][2] = 2.0f * quaternion[1] * quaternion[2] -
                 2.0f * quaternion[3] * quaternion[0];
  matrix[2][2] = 1.0f - 2.0f * quaternion[0] * quaternion[0] -
                 2.0f * quaternion[1] * quaternion[1];
}
inline void CMathlib::QuaternionSlerp(const vec4_t p, const vec4_t q, float t,
                                      vec4_t qt) {
  int i;
  float sclp, sclq;
  vec4_t qq;
  std::copy_n(q, 4, qq);

  float a = 0;
  float b = 0;

  for (i = 0; i < 4; i++) {
    a += (p[i] - qq[i]) * (p[i] - qq[i]);
    b += (p[i] + qq[i]) * (p[i] + qq[i]);
  }

  if (a > b) {
    for (i = 0; i < 4; i++)
      qq[i] = -qq[i];
  }

  float cosom = p[0] * qq[0] + p[1] * qq[1] + p[2] * qq[2] + p[3] * qq[3];

  if ((1.0 + cosom) > 0.00000001) {
    if ((1.0 - cosom) > 0.00000001) {
      float omega = acos(cosom);
      float sinom = sin(omega);
      sclp = sin((1.0f - t) * omega) / sinom;
      sclq = sin(t * omega) / sinom;
    } else {
      sclp = 1.0f - t;
      sclq = t;
    }

    for (i = 0; i < 4; i++)
      qt[i] = sclp * p[i] + sclq * qq[i];
  } else {
    qt[0] = -p[1];
    qt[1] = p[0];
    qt[2] = -p[3];
    qt[3] = p[2];
    sclp = sin((1.0f - t) * 0.5f * mathlib::F_PI);
    sclq = sin(t * 0.5f * mathlib::F_PI);

    for (i = 0; i < 3; i++)
      qt[i] = sclp * p[i] + sclq * qt[i];
  }
}
inline void CMathlib::FloorDivMod(double numer, double denom, int *quotient,
                                  int *rem) {
  int q, r;
  double x;

  if (numer >= 0.0) {
    x = floor(numer / denom);
    q = (int)x;
    r = (int)floor(numer - (x * denom));
  } else {
    x = floor(-numer / denom);
    q = -(int)x;
    r = (int)floor(-numer - (x * denom));

    if (r != 0) {
      q--;
      r = (int)denom - r;
    }
  }

  *quotient = q;
  *rem = r;
}
inline int CMathlib::GreatestCommonDivisor(int i1, int i2) {
  // std::gcd also accepts the negative and zero combinations that made the
  // old recursive Quake implementation recurse forever.
  return std::gcd(i1, i2);
}
inline int CMathlib::InvertMatrix(const float *m, float *out) {
#define SWAP_ROWS(a, b)                                                        \
  {                                                                            \
    float *_tmp = a;                                                           \
    (a) = (b);                                                                 \
    (b) = _tmp;                                                                \
  }
#define MAT(m, r, c) (m)[(c) * 4 + (r)]

  float wtmp[4][8];
  float m0, m1, m2, m3, s;
  float *r0, *r1, *r2, *r3;

  r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

  r0[0] = MAT(m, 0, 0), r0[1] = MAT(m, 0, 1), r0[2] = MAT(m, 0, 2),
  r0[3] = MAT(m, 0, 3), r0[4] = 1.0f, r0[5] = r0[6] = r0[7] = 0.0f,

  r1[0] = MAT(m, 1, 0), r1[1] = MAT(m, 1, 1), r1[2] = MAT(m, 1, 2),
  r1[3] = MAT(m, 1, 3), r1[5] = 1.0f, r1[4] = r1[6] = r1[7] = 0.0f,

  r2[0] = MAT(m, 2, 0), r2[1] = MAT(m, 2, 1), r2[2] = MAT(m, 2, 2),
  r2[3] = MAT(m, 2, 3), r2[6] = 1.0f, r2[4] = r2[5] = r2[7] = 0.0f,

  r3[0] = MAT(m, 3, 0), r3[1] = MAT(m, 3, 1), r3[2] = MAT(m, 3, 2),
  r3[3] = MAT(m, 3, 3), r3[7] = 1.0f, r3[4] = r3[5] = r3[6] = 0.0f;

  if (fabs(r3[0]) > fabs(r2[0]))
    SWAP_ROWS(r3, r2);
  if (fabs(r2[0]) > fabs(r1[0]))
    SWAP_ROWS(r2, r1);
  if (fabs(r1[0]) > fabs(r0[0]))
    SWAP_ROWS(r1, r0);
  if (0.0f == r0[0])
    return false;

  m1 = r1[0] / r0[0];
  m2 = r2[0] / r0[0];
  m3 = r3[0] / r0[0];
  s = r0[1];
  r1[1] -= m1 * s;
  r2[1] -= m2 * s;
  r3[1] -= m3 * s;
  s = r0[2];
  r1[2] -= m1 * s;
  r2[2] -= m2 * s;
  r3[2] -= m3 * s;
  s = r0[3];
  r1[3] -= m1 * s;
  r2[3] -= m2 * s;
  r3[3] -= m3 * s;
  s = r0[4];

  if (s != 0.0f) {
    r1[4] -= m1 * s;
    r2[4] -= m2 * s;
    r3[4] -= m3 * s;
  }
  s = r0[5];
  if (s != 0.0f) {
    r1[5] -= m1 * s;
    r2[5] -= m2 * s;
    r3[5] -= m3 * s;
  }
  s = r0[6];
  if (s != 0.0f) {
    r1[6] -= m1 * s;
    r2[6] -= m2 * s;
    r3[6] -= m3 * s;
  }
  s = r0[7];
  if (s != 0.0f) {
    r1[7] -= m1 * s;
    r2[7] -= m2 * s;
    r3[7] -= m3 * s;
  }

  if (fabs(r3[1]) > fabs(r2[1]))
    SWAP_ROWS(r3, r2);
  if (fabs(r2[1]) > fabs(r1[1]))
    SWAP_ROWS(r2, r1);
  if (0.0f == r1[1])
    return false;

  m2 = r2[1] / r1[1];
  m3 = r3[1] / r1[1];
  r2[2] -= m2 * r1[2];
  r3[2] -= m3 * r1[2];
  r2[3] -= m2 * r1[3];
  r3[3] -= m3 * r1[3];
  s = r1[4];
  if (0.0f != s) {
    r2[4] -= m2 * s;
    r3[4] -= m3 * s;
  }
  s = r1[5];
  if (0.0f != s) {
    r2[5] -= m2 * s;
    r3[5] -= m3 * s;
  }
  s = r1[6];
  if (0.0f != s) {
    r2[6] -= m2 * s;
    r3[6] -= m3 * s;
  }
  s = r1[7];
  if (0.0f != s) {
    r2[7] -= m2 * s;
    r3[7] -= m3 * s;
  }

  if (fabs(r3[2]) > fabs(r2[2]))
    SWAP_ROWS(r3, r2);
  if (0.0f == r2[2])
    return false;

  m3 = r3[2] / r2[2];
  r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4], r3[5] -= m3 * r2[5],
      r3[6] -= m3 * r2[6], r3[7] -= m3 * r2[7];

  if (0.0f == r3[3])
    return false;

  s = 1.0f / r3[3];
  r3[4] *= s;
  r3[5] *= s;
  r3[6] *= s;
  r3[7] *= s;

  m2 = r2[3];
  s = 1.0f / r2[2];
  r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
  r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
  m1 = r1[3];
  r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1, r1[6] -= r3[6] * m1,
      r1[7] -= r3[7] * m1;
  m0 = r0[3];
  r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0, r0[6] -= r3[6] * m0,
      r0[7] -= r3[7] * m0;

  m1 = r1[2];
  s = 1.0f / r1[1];
  r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
  r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
  m0 = r0[2];
  r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0, r0[6] -= r2[6] * m0,
      r0[7] -= r2[7] * m0;

  m0 = r0[1];
  s = 1.0f / r0[0];
  r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
  r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

  MAT(out, 0, 0) = r0[4];
  MAT(out, 0, 1) = r0[5], MAT(out, 0, 2) = r0[6];
  MAT(out, 0, 3) = r0[7], MAT(out, 1, 0) = r1[4];
  MAT(out, 1, 1) = r1[5], MAT(out, 1, 2) = r1[6];
  MAT(out, 1, 3) = r1[7], MAT(out, 2, 0) = r2[4];
  MAT(out, 2, 1) = r2[5], MAT(out, 2, 2) = r2[6];
  MAT(out, 2, 3) = r2[7], MAT(out, 3, 0) = r3[4];
  MAT(out, 3, 1) = r3[5], MAT(out, 3, 2) = r3[6];
  MAT(out, 3, 3) = r3[7];

  return true;
#undef MAT
#undef SWAP_ROWS
}
inline void CMathlib::SinCos(float radians, float *sine, float *cosine) {
  *sine = sinf(radians);
  *cosine = cosf(radians);
}
inline void CMathlib::Matrix4x4_CreateFromEntity(float out[4][4],
                                                 const vec3_t angles,
                                                 const vec3_t origin,
                                                 float scale) {
  out[3][0] = out[3][1] = out[3][2] = 0;
  out[3][3] = 1;
  float angle, sr, sp, sy, cr, cp, cy;
  if (angles[Q_ROLL]) {
    angle = angles[Q_YAW] * (mathlib::F_PI * 2 / 360.0f);
    SinCos(angle, &sy, &cy);
    angle = angles[Q_PITCH] * (mathlib::F_PI * 2 / 360.0f);
    SinCos(angle, &sp, &cp);
    angle = angles[Q_ROLL] * (mathlib::F_PI * 2 / 360.0f);
    SinCos(angle, &sr, &cr);

    out[0][0] = (cp * cy) * scale;
    out[0][1] = (sr * sp * cy + cr * -sy) * scale;
    out[0][2] = (cr * sp * cy + -sr * -sy) * scale;
    out[0][3] = origin[0];
    out[1][0] = (cp * sy) * scale;
    out[1][1] = (sr * sp * sy + cr * cy) * scale;
    out[1][2] = (cr * sp * sy + -sr * cy) * scale;
    out[1][3] = origin[1];
    out[2][0] = (-sp) * scale;
    out[2][1] = (sr * cp) * scale;
    out[2][2] = (cr * cp) * scale;
    out[2][3] = origin[2];
  } else if (angles[Q_PITCH]) {
    angle = angles[Q_YAW] * (mathlib::F_PI * 2 / 360.0f);
    SinCos(angle, &sy, &cy);
    angle = angles[Q_PITCH] * (mathlib::F_PI * 2 / 360.0f);
    SinCos(angle, &sp, &cp);

    out[0][0] = (cp * cy) * scale;
    out[0][1] = (-sy) * scale;
    out[0][2] = (sp * cy) * scale;
    out[0][3] = origin[0];
    out[1][0] = (cp * sy) * scale;
    out[1][1] = (cy)*scale;
    out[1][2] = (sp * sy) * scale;
    out[1][3] = origin[1];
    out[2][0] = (-sp) * scale;
    out[2][1] = 0.0f;
    out[2][2] = (cp)*scale;
    out[2][3] = origin[2];
  } else if (angles[Q_YAW]) {
    angle = angles[Q_YAW] * (mathlib::F_PI * 2 / 360.0f);
    SinCos(angle, &sy, &cy);

    out[0][0] = (cy)*scale;
    out[0][1] = (-sy) * scale;
    out[0][2] = 0.0f;
    out[0][3] = origin[0];
    out[1][0] = (sy)*scale;
    out[1][1] = (cy)*scale;
    out[1][2] = 0.0f;
    out[1][3] = origin[1];
    out[2][0] = 0.0f;
    out[2][1] = 0.0f;
    out[2][2] = scale;
    out[2][3] = origin[2];
  } else {
    out[0][0] = scale;
    out[0][1] = 0.0f;
    out[0][2] = 0.0f;
    out[0][3] = origin[0];
    out[1][0] = 0.0f;
    out[1][1] = scale;
    out[1][2] = 0.0f;
    out[1][3] = origin[1];
    out[2][0] = 0.0f;
    out[2][1] = 0.0f;
    out[2][2] = scale;
    out[2][3] = origin[2];
  }
}
inline void CMathlib::Matrix4x4_ConcatTransforms(float out[4][4],
                                                 float in1[4][4],
                                                 float in2[4][4]) {
  float result[4][4] = {};
  for (int row = 0; row < 3; ++row) {
    for (int col = 0; col < 4; ++col) {
      for (int k = 0; k < 3; ++k)
        result[row][col] += in1[row][k] * in2[k][col];
      if (col == 3)
        result[row][col] += in1[row][3];
    }
  }
  result[3][3] = 1;
  std::memcpy(out, result, sizeof(result));
}
inline void CMathlib::ConcatTransforms(float in1[3][4], float in2[3][4],
                                       float out[3][4]) {
  out[0][0] =
      in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
  out[0][1] =
      in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
  out[0][2] =
      in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
  out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
              in1[0][2] * in2[2][3] + in1[0][3];
  out[1][0] =
      in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
  out[1][1] =
      in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
  out[1][2] =
      in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
  out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
              in1[1][2] * in2[2][3] + in1[1][3];
  out[2][0] =
      in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
  out[2][1] =
      in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
  out[2][2] =
      in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
  out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
              in1[2][2] * in2[2][3] + in1[2][3];
}
inline void CMathlib::RGBToHSV(int r, int g, int b, float &h, float &s,
                               float &v) {
  float fr = r / 255.0f, fg = g / 255.0f, fb = b / 255.0f;
  float max = max3(fr, fg, fb);
  float min = min3(fr, fg, fb);
  float range = max - min;
  // H
  if (range <= 0)
    h = 0;
  else if (max == fr)
    h = 60 * (fg - fb) / range + (g >= b ? 0 : 360);
  else if (max == fg)
    h = 60 * (fb - fr) / range + 120;
  else
    h = 60 * (fr - fg) / range + 240;
  if (abs(h) >= 360)
    h = static_cast<float>(fmod(h, 360));
  // S
  s = max <= 0 ? 0 : range / max;
  // V
  v = max <= 0 ? 0 : max;
}
inline void CMathlib::HSVToRGB(float h, float s, float v, int &r, int &g,
                               int &b) {
  // 0<=h<360
  // 0<=s<=1
  // 0<=v<=1
  h = static_cast<float>(fmod(h, 360));
  s = std::clamp(s, 0.0f, 1.0f);
  v = std::clamp(v, 0.0f, 1.0f);
  float section = h / 60;
  float c = v * s;
  float x = static_cast<float>(c * (1 - abs(fmod(section, 2) - 1)));
  float hr = 0, hg = 0, hb = 0;
  switch ((int)section) {
  case 0:
    hr = c, hg = x, hb = 0;
    break;
  case 1:
    hr = x;
    hg = c;
    hb = 0;
    break;
  case 2:
    hr = 0;
    hg = c;
    hb = x;
    break;
  case 3:
    hr = 0;
    hg = x;
    hb = c;
    break;
  case 4:
    hr = x;
    hg = 0;
    hb = c;
    break;
  case 5:
    hr = c;
    hg = 0;
    hb = x;
    break;
  }
  float m = v - c;
  r = static_cast<int>((hr + m) * 255);
  g = static_cast<int>((hg + m) * 255);
  b = static_cast<int>((hb + m) * 255);
}

} // namespace abc

#ifdef ABC_MATH_INL_RESTORE_MAX
#pragma pop_macro("max")
#undef ABC_MATH_INL_RESTORE_MAX
#endif
#ifdef ABC_MATH_INL_RESTORE_MIN
#pragma pop_macro("min")
#undef ABC_MATH_INL_RESTORE_MIN
#endif
