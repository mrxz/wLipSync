#include "../math.h"
#include "../pt_math.h"

float get_max_value(float *array, int len) {
  float max = 0.f;
  for(int i = 0; i < len; ++i) {
    max = maxf(max, PT_fabsf(array[i]));
  }
  return max;
}

void normalize(float *data, int len, float value) {
  float max = get_max_value(data, len);
  if(max < EPSILON) {
    return;
  }
  float r = value / max;
  for(int i = 0; i < len; ++i) {
    data[i] *= r;
  }
}