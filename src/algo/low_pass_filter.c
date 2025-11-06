#include "../pt_math.h"

void low_pass_filter_impl(float* data, int len, float cutoff, float* tmp, int bLen, int skip) {
  for(int j = 0; j < bLen/2; ++j) {
    float x = j - (bLen - 1) / 2.f;
    float ang = 2.f * PT_PI * cutoff * x;
    float b = 2.f * cutoff * PT_sinf(ang) / ang;

    // Start i at j rounded up to the nearest multiple of skip
    for(int i = j + (skip - j%skip)%skip; i < len; i += skip) {
      data[i] += b * (tmp[bLen + i - j] + tmp[bLen + i - (bLen - 1 - j)]);
    }
  }
}

void low_pass_filter(float* data, unsigned long size, float sampleRate, float cutoff, float range, int skip) {
  cutoff = (cutoff - range) / sampleRate;
  range /= sampleRate;

  int n = (int)PT_roundf(3.1f/range);
  if(n%2) {
    n++;
  }

  // Pad temp copy with zeros to allow easy indexing
  float tmp[n + size];
  __builtin_memset(tmp, 0, sizeof(float)*n);
  __builtin_memcpy(&tmp[n], data, sizeof(float)*size);

  low_pass_filter_impl(data, size, cutoff, tmp, n, skip);
}
