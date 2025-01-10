#include "../pt_math.h"

void hamming_window(float *data, int len) {
  for(int i = 0; i < len; ++i) {
    float x = (float)i / (len - 1);
    data[i] *= 0.54f - 0.46f * PT_cosf(2.f * PT_PI * x);
  }
}