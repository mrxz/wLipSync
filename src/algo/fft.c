#include "../pt_math.h"

void fft(float* data, float* spectrum, unsigned long size) {
  float spectrumRe[size];
  float spectrumIm[size];

  // Bit reverse
  unsigned int bits = 31 - __builtin_clzl(size);
  for(int i = 0; i < size; ++i) {
    int reversed = 0;
    for(int j = 0; j < bits; ++j) {
      reversed = (reversed << 1) | (i >> j & 1);
    }

    spectrumRe[i] = data[reversed];
    spectrumIm[i] = 0.f;
  }

  // Compute twiddle factors
  float cx[size/2];
  float cy[size/2];
  for(int j = 0; j < size/2; ++j) {
    float theta = -2.f * PT_PI * j / size;
    cx[j] = PT_cosf(theta);
    cy[j] = PT_sinf(theta);
  }

  // Perform FFT
  for(int halfSize = 1; halfSize < size; halfSize <<= 1) {
    int stride = size / (halfSize * 2);
    for(int i = 0; i < size; i += 2 * halfSize) {
      for(int j = 0; j < halfSize; ++j) {
        float er = spectrumRe[i + j];
        float ei = spectrumIm[i + j];
        float or = spectrumRe[i + j + halfSize];
        float oi = spectrumIm[i + j + halfSize];

        int ti = j * stride;
        float cx2 = cx[ti] * or - cy[ti] * oi;
        float cy2 = cx[ti] * oi + cy[ti] * or;

        spectrumRe[i + j] = er + cx2;
        spectrumIm[i + j] = ei + cy2;
        spectrumRe[i + j + halfSize] = er - cx2;
        spectrumIm[i + j + halfSize] = ei - cy2;
      }
    }
  }

  for(int i = 0; i < size; ++i) {
    float re = spectrumRe[i];
    float im = spectrumIm[i];
    spectrum[i] = PT_sqrtf(re * re + im * im); // length
  }
}