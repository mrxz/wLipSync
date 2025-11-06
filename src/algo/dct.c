#include "../pt_math.h"

void dct(float* spectrum, float* cepstrum, unsigned long size) {
  float a = PT_PI / size;
  for(int i = 0; i < size; ++i) {
    float sum = 0.f;
    for(int j = 0; j < size; ++j) {
      float ang = (j + 0.5f) * i * a;
      sum += spectrum[j] * PT_cosf(ang);
    }
    cepstrum[i] = sum;
  }
}