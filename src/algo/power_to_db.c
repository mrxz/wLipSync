#include "../pt_math.h"

void power_to_db(float *array, unsigned long size) {
  for (int i = 0; i < size; ++i) {
    array[i] = 10.f * PT_log10f(array[i]);
  }
}
