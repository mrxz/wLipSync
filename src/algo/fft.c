#include "../pt_math.h"

void fft_impl(float *spectrumRe, float *spectrumIm, unsigned long size) {
  if (size < 2) {
    return;
  }

  float evenRe[size / 2];
  float evenIm[size / 2];
  float oddRe[size / 2];
  float oddIm[size / 2];

  for(int i = 0; i < size / 2; ++i) {
    evenRe[i] = spectrumRe[i * 2];
    evenIm[i] = spectrumIm[i * 2];
    oddRe[i] = spectrumRe[i * 2 + 1];
    oddIm[i] = spectrumIm[i * 2 + 1];
  }

  fft_impl(evenRe, evenIm, size / 2);
  fft_impl(oddRe, oddIm, size / 2);

  for(int i = 0; i < size / 2; ++i) {
    float er = evenRe[i];
    float ei = evenIm[i];
    float or = oddRe[i];
    float oi = oddIm[i];
    float theta = -2.f * PT_PI * i / size;

    float cx = PT_cosf(theta);
    float cy = PT_sinf(theta);
    float cx2 = cx * or - cy * oi;
    float cy2 = cx * oi + cy * or;

    spectrumRe[i] = er + cx2;
    spectrumIm[i] = ei + cy2;
    spectrumRe[size / 2 + i] = er - cx2;
    spectrumIm[size / 2 + i] = ei - cy2;
  }
}

void fft(float *data, float *spectrum, unsigned long size) {
  float spectrumRe[size];
  float spectrumIm[size];

  for(int i = 0; i < size; ++i) {
    spectrumRe[i] = data[i];
    spectrumIm[i] = 0.f;
  }
  fft_impl(spectrumRe, spectrumIm, size);

  for(int i = 0; i < size; ++i) {
    float re = spectrumRe[i];
    float im = spectrumIm[i];
    spectrum[i] = PT_sqrtf(re * re + im * im); // length
  }
}