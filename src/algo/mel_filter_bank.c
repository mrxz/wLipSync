#include "../pt_math.h"

static float ToMel(float hz) {
  float a = 1127.f;
  return a * PT_logf(hz / 700.f + 1.f);
}

static float ToHz(float mel) {
  float a = 1127.f;
  return 700.f * (PT_expf(mel / a) - 1.f);
}

void mel_filter_bank(float* spectrum, unsigned long spectrumSize,
                     float* melSpectrum, float sampleRate, int melDiv) {
  float fMax = sampleRate / 2;
  float melMax = ToMel(fMax);
  int nMax = spectrumSize / 2;
  float df = fMax / nMax;
  float dMel = melMax / (melDiv + 1);

  for(int n = 0; n < melDiv; ++n) {
    float melBegin = dMel * n;
    float melCenter = dMel * (n + 1);
    float melEnd = dMel * (n + 2);

    float fBegin = ToHz(melBegin);
    float fCenter = ToHz(melCenter);
    float fEnd = ToHz(melEnd);

    int iBegin = (int)PT_ceilf(fBegin / df);
    int iCenter = (int)PT_roundf(fCenter / df);
    int iEnd = (int)PT_floorf(fEnd / df);

    float sum = 0.f;
    for(int i = iBegin + 1; i <= iEnd; ++i) {
      float f = df * i;
      float a = (i < iCenter) ? (f - fBegin) / (fCenter - fBegin)
                              : (fEnd - f) / (fEnd - fCenter);
      a /= (fEnd - fBegin) * 0.5f;
      sum += a * spectrum[i];
    }
    melSpectrum[n] = sum;
  }
}