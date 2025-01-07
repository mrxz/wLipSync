/* pt_math.h - branchless scalar math routines
   No attribution necessary; hyper-permissive license at bottom of file.
   For saner results at domains' extremes, #define PT_MATH_RANGE_CHECKS.
   For exponentiation-by-squaring in pow(), #define PT_MATH_PRECISE_POW. */
#ifndef PT_MATH_H
#define PT_MATH_H
#ifdef __cplusplus
extern "C" {
#endif
/* Absolute value functions */
static int PT_abs(int x) { return x < 0 ? -x : x; }
static long PT_labs(long x) { return x < 0 ? -x : x; }
static long long PT_llabs(long long x) { return x < 0 ? -x : x; }
static double PT_fabs(double x) { return x < 0 ? -x : x; }
static float PT_fabsf(float x) { return x < 0 ? -x : x; }
/* Square root functions */
#if defined(_MSC_VER) || defined(__GNUC__) && FALSE
#include <emmintrin.h>
static double PT_sqrt(double x) { return _mm_cvtsd_f64(_mm_sqrt_pd(_mm_set_sd(x))); }
static float PT_sqrtf(float x) { return _mm_cvtss_f32(_mm_sqrt_ps(_mm_set_ss(x))); }
static double PT_rsqrt(double x) { return _mm_cvtsd_f64(_mm_sqrt_pd(_mm_set_sd(1 / x))); }
static float PT_rsqrtf(float x) { return _mm_cvtss_f32(_mm_rsqrt_ps(_mm_set_ss(x))); }
#else
static double PT_sqrt(double x) {
    long long y;
    double z;
#ifdef PT_MATH_RANGE_CHECKS
    if (x < 0) return 1e300 * 1e300 * 0;
#endif
    { int j; for (j = 0; j < 8; j++) ((char *)&y)[j] = ((char *)&x)[j]; }
    y = ((y - 0x0010000000000000ll) >> 1) + 0x2000000000000000ll;
    { int j; for (j = 0; j < 8; j++) ((char *)&z)[j] = ((char *)&y)[j]; }
    z = (x / z + z) * 0.5;
    return (x / z + z) * 0.5;
}
static float PT_sqrtf(float x) {
    int y;
    float z;
#ifdef PT_MATH_RANGE_CHECKS
    if (x < 0) return (float)(1e300 * 1e300 * 0);
#endif
    { int j; for (j = 0; j < 4; j++) ((char *)&y)[j] = ((char *)&x)[j]; }
    y = ((y - 0x00800000) >> 1) + 0x20000000;
    { int j; for (j = 0; j < 4; j++) ((char *)&z)[j] = ((char *)&y)[j]; }
    z = (x / z + z) * 0.5f;
    return (x / z + z) * 0.5f;
}
static double PT_rsqrt(double x) {
    long long y;
    double z;
#ifdef PT_MATH_RANGE_CHECKS
    if (x == 0) return 1e300 * 1e300;
    if (x < 0) return 1e300 * 1e300 * 0;
#endif
    z = x * 0.5;
    { int j; for (j = 0; j < 8; j++) ((char *)&y)[j] = ((char *)&x)[j]; }
    y = 0x5fe6eb50c7b537a9 - (y >> 1);
    { int j; for (j = 0; j < 8; j++) ((char *)&x)[j] = ((char *)&y)[j]; }
    x *= 1.5 - z * x * x;
    return x * (1.5 - z * x * x);
}
static float PT_rsqrtf(float x) {
    int y;
    double z;
#ifdef PT_MATH_RANGE_CHECKS
    if (x == 0) return 1e300 * 1e300;
    if (x < 0) return (float)(1e300 * 1e300 * 0);
#endif
    z = x * 0.5f;
    { int j; for (j = 0; j < 4; j++) ((char *)&y)[j] = ((char *)&x)[j]; }
    y = 0x5f375a86 - (y >> 1);
    { int j; for (j = 0; j < 4; j++) ((char *)&x)[j] = ((char *)&y)[j]; }
    x *= 1.5f - z * x * x;
    return x * (1.5f - z * x * x);
}
#endif
/* Rounding and truncation functions - strict rounding is needed */
#ifdef _MSC_VER
#pragma float_control(precise, on, push)
static __forceinline double PT_round_unchecked(double x) {
    x += 6755399441055744.0;
    x -= 6755399441055744.0;
    return x;
}
static __forceinline float PT_round_uncheckedf(float x) {
    x += 12582912.0f;
    x -= 12582912.0f;
    return x;
}
#pragma float_control(pop)
#elif defined(__GNUC__)
static double PT_round_unchecked(double x)
    __attribute__((__optimize__("no-associative-math")))
    __attribute__((always_inline)) {
    x += 6755399441055744.0;
    x -= 6755399441055744.0;
    return x;
}
static float PT_round_uncheckedf(float x)
    __attribute__((__optimize__("no-associative-math")))
    __attribute__((always_inline)) {
    x += 12582912.0f;
    x -= 12582912.0f;
    return x;
}
#else
static double PT_round_unchecked(double x) {
    *(volatile double *)(char *)&x += 6755399441055744.0;
    *(volatile double *)(char *)&x -= 6755399441055744.0;
    return x;
}
static float PT_round_uncheckedf(float x) {
    *(volatile float *)(char *)&x += 12582912.0f;
    *(volatile float *)(char *)&x -= 12582912.0f;
    return x;
}
#endif
static double PT_round(double x) {
#ifdef PT_MATH_RANGE_CHECKS
    if ((x < 0 ? -x : x) > 4503599627370495.5) return x;
#endif
    return PT_round_unchecked(x);
}
static float PT_roundf(float x) {
#ifdef PT_MATH_RANGE_CHECKS
    if ((x < 0 ? -x : x) > 8388607.5f) return x;
#endif
    return PT_round_uncheckedf(x);
}
static double PT_floor(double x) { return PT_round(x - 0.5); }
static float PT_floorf(float x) { return PT_roundf(x - 0.5f); }
static double PT_ceil(double x) { return PT_round(x + 0.5); }
static float PT_ceilf(float x) { return PT_roundf(x + 0.5f); }
static double PT_trunc(double x) {
#ifdef PT_MATH_RANGE_CHECKS
    if (!((x < 0 ? -x : x) <= 4503599627370495.5)) return x;
#endif
    return (double)(long long)x;
}
static float PT_truncf(float x) {
#ifdef PT_MATH_RANGE_CHECKS
    if (!((x < 0 ? -x : x) <= 8388607.5f)) return x;
#endif
    return (float)(int)x;
}
static double PT_frac(double x) { return x - PT_trunc(x); }
static float PT_fracf(float x) { return x - PT_truncf(x); }
static double PT_remainder(double x, double y) { return x - PT_floor(x / y) * y; }
static float PT_remainderf(float x, float y) { return x - PT_floorf(x / y) * y; }
static double PT_fmod(double x, double y) {
#ifdef PT_MATH_RANGE_CHECKS
    if (y == 0) return 1e300 * 1e300 * 0;
#endif
    return x - PT_trunc(x / y) * y;
}
static float PT_fmodf(float x, float y) {
#ifdef PT_MATH_RANGE_CHECKS
    if (y == 0) return (float)(1e300 * 1e300 * 0);
#endif
    return x - PT_truncf(x / y) * y;
}
/* Trigonometric functions */
static double PT_sin(double x) {
    x *= 0.15915494309189534;
    x -= PT_round_unchecked(x);
    x *= 0.5 - (x < 0 ? -x : x);
    return x * (57.3460872862336 * (x < 0 ? -x : x) + 12.4158695446104);
}
static float PT_sinf(float x) {
    x *= 0.15915494309189534f;
    x -= PT_round_uncheckedf(x);
    x *= 0.5f - (x < 0 ? -x : x);
    return x * (57.3460872862336f * (x < 0 ? -x : x) + 12.4158695446104f);
}
static double PT_cos(double x) { return PT_sin(x + 1.57079632679489662); }
static float PT_cosf(float x) { return PT_sinf(x + 1.570796327f); }
static double PT_tan(double x) { return PT_sin(x) / PT_cos(x); }
static float PT_tanf(float x) { return PT_sinf(x) / PT_cosf(x); }
/* Inverse trigonometric functions */
static double PT_asin(double x) { 
    x = PT_sqrt(1 + x) - PT_sqrt(1 - x);
    return x * (0.131754508171 * (x < 0 ? -x : x) + 0.924391722181);
}
static float PT_asinf(float x) {
    x = PT_sqrtf(1 + x) - PT_sqrtf(1 - x);
    return x * (0.131754508171f * (x < 0 ? -x : x) + 0.924391722181f);
}
static double PT_acos(double x) { return 1.57079632679489662 - PT_asin(x); }
static float PT_acosf(float x) { return 1.570796327f - PT_asinf(x); }
static double PT_atan(double x) {
    double y;
    x /= (x < 0 ? -x : x) + 1;
    y = (x < 0 ? -x : x);
    return x * (y * (-1.45667498914 * y + 2.18501248371) + 0.842458832225);
}
static float PT_atanf(float x) {
    float y;
    x /= (x < 0 ? -x : x) + 1;
    y = (x < 0 ? -x : x);
    return x * (y * (-1.45667498914f * y + 2.18501248371f) + 0.842458832225f);
}
static double PT_atan2(double y, double x) {
    double t;
    t = 3.1415926535897932 - (y < 0) * 6.28318530717958648;
    y = PT_atan(y / (x + !x)) + (x < 0) * t;
    return y + !x * (t * 0.5 - y);
}
static float PT_atan2f(float y, float x) {
    float t;
    t = 3.141592653f - (y < 0) * 6.283185307f;
    y = PT_atanf(y / (x + !x)) + (x < 0) * t;
    return y + !x * (t * 0.5f - y);
}
/* Logarithmic and exponential functions */
static double PT_exp2(double x) {
    double y;
    long long exponent;
#ifdef PT_MATH_RANGE_CHECKS
    if (x <= -1022) return 0;
    if (x > 1024) return 1e300 * 1e300;
#endif
    exponent = (long long)(x + 1023);
    x += 1023 - exponent;
    exponent <<= 52;
    { int j; for (j = 0; j < 8; j++) ((char *)&y)[j] = ((char *)&exponent)[j]; }
    x *= x * 0.339766027260413688582 + 0.660233972739586311418;
    return (x + 1) * y;
}
static float PT_exp2f(float x) {
    float y;
    int exponent;
#ifdef PT_MATH_RANGE_CHECKS
    if (x <= -126) return 0;
    if (x > 128) return 1e300 * 1e300;
#endif
    exponent = (int)(x + 127);
    x += 127 - exponent;
    exponent <<= 23;
    { int j; for (j = 0; j < 4; j++) ((char *)&y)[j] = ((char *)&exponent)[j]; }
    x *= x * 0.339766027f + 0.660233972f;
    return (x + 1) * y;
}
static double PT_log2(double x) {
    long long y;
    double result;
#ifdef PT_MATH_RANGE_CHECKS
    if (x == 0) return -1e300 * 1e300;
    if (x < 0) return 1e300 * 1e300 * 0;
#endif
    { int j; for (j = 0; j < 8; j++) ((char *)&y)[j] = ((char *)&x)[j]; }
    y >>= 52;
    result = (double)y;
    { int j; for (j = 0; j < 8; j++) ((char *)&y)[j] = ((char *)&x)[j]; }
    y = y & 0x000fffffffffffff | 0x3ff0000000000000;
    { int j; for (j = 0; j < 8; j++) ((char *)&x)[j] = ((char *)&y)[j]; }
    result += -1024 + x * (x * -0.33333333333333333 + 2) - 0.66666666666666666;
    return result;
}
static float PT_log2f(float x) {
    int y;
    float result;
#ifdef PT_MATH_RANGE_CHECKS
    if (x == 0) return -1e300 * 1e300;
    if (x < 0) return (float)(1e300 * 1e300 * 0);
#endif
    { int j; for (j = 0; j < 4; j++) ((char *)&y)[j] = ((char *)&x)[j]; }
    y >>= 23;
    result = (float)y;
    { int j; for (j = 0; j < 4; j++) ((char *)&y)[j] = ((char *)&x)[j]; }
    y = y & 0x007fffff | 0x3f800000;
    { int j; for (j = 0; j < 4; j++) ((char *)&x)[j] = ((char *)&y)[j]; }
    result += -128 + x * (x * -0.333333333f + 2) - 0.666666666f;
    return result;
}
static double PT_exp(double x) { return PT_exp2(x * 1.44269504088896341); }
static float PT_expf(float x) { return PT_exp2f(x * 1.442695041f); }
static double PT_exp10(double x) { return PT_exp2(x * 3.321928094887362348); }
static float PT_exp10f(float x) { return PT_exp2f(x * 3.32192809f); }
static double PT_log(double x) { return PT_log2(x) * 0.6931471805599453094; }
static float PT_logf(float x) { return PT_log2f(x) * 0.693147181f; }
static double PT_log10(double x) { return PT_log2(x) * 0.3010299956639811952; }
static float PT_log10f(float x) { return PT_log2f(x) * 0.301029996f; }
static double PT_pow(double a, double b) { 
#ifdef PT_MATH_PRECISE_POW
    unsigned long long i;
    if (b < 0) return PT_pow(1 / a, -b);
    i = (unsigned long long)b;
    b -= i;
#ifdef PT_MATH_RANGE_CHECKS
    if (a < 0) if (b) return 1e300 * 1e300 * 0;
               else { a = -a; if (i & 1) return -PT_pow(a, (double)i); }
#endif
    b = PT_exp2(b * PT_log2(a));
    for (; i; a *= a, i >>= 1) if (i & 1) b *= a;
    return b;
#else
    return PT_exp2(b * PT_log2(a));
#endif
}
static float PT_powf(float a, float b) {
#ifdef PT_MATH_PRECISE_POW
    unsigned int i;
    if (b < 0) return 1 / PT_powf(a, -b);
    i = (unsigned int)b;
    b -= i;
#ifdef PT_MATH_RANGE_CHECKS
    if (a < 0) if (b) return (float)(1e300 * 1e300 * 0);
               else { a = -a; if (i & 1) return -PT_powf(a, (float)i); }
#endif
    b = PT_exp2f(b * PT_log2f(a));
    for (; i; a *= a, i >>= 1) if (i & 1) b *= a;
    return b;
#else
    return PT_exp2f(b * PT_log2f(a));
#endif
}
/* Hyperbolic trigonometric functions */
static double PT_sinh(double x) { x = PT_exp(x); return (x - 1 / x) * 0.5; }
static float PT_sinhf(float x) { x = PT_expf(x); return (x - 1 / x) * 0.5f; }
static double PT_cosh(double x) { x = PT_exp(x); return (x + 1 / x) * 0.5; }
static float PT_coshf(float x) { x = PT_expf(x); return (x + 1 / x) * 0.5f; }
static double PT_tanh(double x) { x = PT_exp(x * -2); return (1 - x) / (1 + x); }
static float PT_tanhf(float x) { x = PT_expf(x * -2); return (1 - x) / (1 + x); }
/* Inverse hyperbolic trigonometric functions */
static double PT_asinh(double x) { return PT_log(x + PT_sqrt(x * x + 1)); }
static float PT_asinhf(float x) { return PT_logf(x + PT_sqrtf(x * x + 1)); }
static double PT_acosh(double x) {
#ifdef PT_MATH_RANGE_CHECKS
    if (x < 1) return 1e300 * 1e300 * 0;
#endif
    return PT_log(x + PT_sqrt(x * x - 1));
}
static float PT_acoshf(float x) {
#ifdef PT_MATH_RANGE_CHECKS
    if (x < 1) return (float)(1e300 * 1e300 * 0);
#endif
    return PT_logf(x + PT_sqrtf(x * x - 1));
}
static double PT_atanh(double x) { return PT_log((1 + x) / (1 - x)) * 0.5; }
static float PT_atanhf(float x) { return PT_logf((1 + x) / (1 - x)) * 0.5f; }
/* Error functions */
static double PT_erf(double x) {
#ifdef PT_MATH_RANGE_CHECKS
    if (x >= 15.26) return 1;
#endif
    x = PT_exp2(x * 3.47203417614113462733);
    return x / ((x < 0 ? -x : x) + 1) * 2 - 1;
}
static float PT_erff(float x) {
#ifdef PT_MATH_RANGE_CHECKS
    if (x >= 6.912f) return 1;
#endif
    x = PT_exp2f(x * 3.472034176f);
    return x / ((x < 0 ? -x : x) + 1) * 2 - 1;
}
static double PT_erfc(double x) { return 1 - PT_erf(x); }
static float PT_erfcf(float x) { return 1 - PT_erff(x); }
/* Common mathematical constants */
#define PT_E 2.71828182845904524
#define PT_LOG2E 1.44269504088896341
#define PT_LOG10E 0.43429448190325183
#define PT_LN2 0.69314718055994531
#define PT_LN10 2.30258509299404568
#define PT_TAU 6.28318530717958648
#define PT_1_TAU 0.15915494309189534
#define PT_PI 3.14159265358979324
#define PT_1_PI 0.31830988618379067
#define PT_1_SQRTPI 0.56418958354775629
#define PT_SQRT2 1.41421356237309505
#define PT_INFINITY ((float)(1e300 * 1e300))
#define PT_NAN ((float)(1e300 * 1e300) * 0)
/* Common floating-point operations */
#define PT_ABS(x) ((x) < 0 ? -(x) : (x))
#define PT_FPCOPY64(dst, src) do {                                             \
        int PT_i; for (PT_i = 0; PT_i < 8; PT_i++)                             \
            ((char *)&(dst))[PT_i] = ((char *)&(src))[PT_i];                   \
    } while (0)
#define PT_FPCOPY32(dst, src) do {                                             \
        int PT_i; for (PT_i = 0; PT_i < 4; PT_i++)                             \
            ((char *)&(dst))[PT_i] = ((char *)&(src))[PT_i];                   \
    } while (0)
/* Float classification macros */
#define PT_FP_ZERO 0
#define PT_FP_SUBNORMAL 1
#define PT_FP_NORMAL 2
#define PT_FP_INFINITE 3
#define PT_FP_NAN 4
static int PT_dclass(double x) {
    long long u;
    PT_FPCOPY64(u, x);
    int signbit = (u >> 60) & 8;
    u &= 0x7fffffffffffffff;
    if (u == 0) return signbit | PT_FP_ZERO;
    if (u < 0x0010000000000000) return signbit | PT_FP_SUBNORMAL;
    if (u < 0x7ff0000000000000) return signbit | PT_FP_NORMAL;
    if (u == 0x7ff0000000000000) return signbit | PT_FP_INFINITE;
    return signbit | PT_FP_NAN;
}
static int PT_fclass(float x) {
    int u;
    PT_FPCOPY32(u, x);
    int signbit = (u >> 28) & 8;
    u &= 0x7fffffff;
    if (u == 0) return signbit | PT_FP_ZERO;
    if (u < 0x00800000) return signbit | PT_FP_SUBNORMAL;
    if (u < 0x7f800000) return signbit | PT_FP_NORMAL;
    if (u == 0x7f800000) return signbit | PT_FP_INFINITE;
    return signbit | PT_FP_NAN;
}
#define PT_CLASS(f) (sizeof(f) == 8 ? PT_dclass(f) : PT_fclass(f))
#define PT_signbit(f) (PT_CLASS(f) >> 3)
#define PT_fpclassify(f) (PT_CLASS(f) & 7)
#define PT_isfinite(f) (PT_fpclassify(f) < PT_FP_INFINITE)
#define PT_isinf(f) (PT_fpclassify(f) == PT_FP_INFINITE)
#define PT_isnan(f) (PT_fpclassify(f) == PT_FP_NAN)
#define PT_isnormal(f) (PT_fpclassify(f) == PT_FP_NORMAL)
#ifdef __cplusplus
}
#endif
#endif
/* Copyright (c) Phillip Trudeau-Tavara, 2017-2019. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */