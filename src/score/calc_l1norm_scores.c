#include "../constants.h"
#define PT_MATH_PRECISE_POW
#include "../pt_math.h"

void calc_l1norm_scores(float *mfcc, float *profileMfcc, float *means,
                        float *standardDeviations, float *scores,
                        unsigned int mfccCount) {
  for(int phoneme = 0; phoneme < mfccCount; phoneme++) {
    scores[phoneme] = 0;
    for(int i = 0; i < MFCC_NUM; i++) {
      float x = (mfcc[i] - means[i]) / standardDeviations[i];
      float y = (profileMfcc[phoneme * MFCC_NUM + i] - means[i]) / standardDeviations[i];
      scores[phoneme] += PT_ABS(x - y);
    }
    scores[phoneme] /= MFCC_NUM;
    scores[phoneme] = PT_powf(10.f, -scores[phoneme]);
  }
}