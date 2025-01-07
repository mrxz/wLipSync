#include "../pt_math.h"
#include "../math.h"

void down_sample_exact(float *input, float *output, unsigned long output_size,
                 int skip) {
    for(int i = 0; i < output_size; ++i) {
        output[i] = input[i * skip];
    }
}

void down_sample(float *input, unsigned long size, float *output,
                 unsigned long output_size, float df) {
  for (int j = 0; j < output_size; ++j) {
    float fIndex = df * j;
    int i0 = (int)PT_floor(fIndex);
    int i1 = min(i0, size - 1);
    float t = fIndex - i0;
    float x0 = input[i0];
    float x1 = input[i1];
    output[j] = lerp(x0, x1, t);
  }
}