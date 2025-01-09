#include "../constants.h"
#include "../math.h"
#include "../pt_math.h"

void calc_cosine_similarity_scores(float *mfcc, float *profileMfcc,
                                   float *means, float *standardDeviations,
                                   float *scores, unsigned int mfccCount) {
  for(int phoneme = 0; phoneme < mfccCount; phoneme++) {
    float mfccNorm = 0.f;
    float phonemeNorm = 0.f;

    float prod = 0.f;

    scores[phoneme] = 0;
    for(int i = 0; i < MFCC_NUM; i++) {
      float x = (mfcc[i] - means[i]) / standardDeviations[i];
      float y = (profileMfcc[phoneme * MFCC_NUM + i]) / standardDeviations[i];
      mfccNorm += x * x;
      phonemeNorm += y * y;
      prod += x * y;
    }

    mfccNorm = PT_sqrtf(mfccNorm);
    phonemeNorm = PT_sqrtf(phonemeNorm);
    float similarity = prod / (mfccNorm * phonemeNorm);
    similarity = maxf(similarity, 0.f);

    scores[phoneme] = PT_powf(similarity, 100.f);
  }
}