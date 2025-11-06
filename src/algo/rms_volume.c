#include "../pt_math.h"

float rms_volume(float* array, unsigned long size) {
  float average = 0.f;
  for(int i = 0; i < size; ++i) {
    average += array[i] * array[i];
  }
  return PT_sqrtf(average / size);
}