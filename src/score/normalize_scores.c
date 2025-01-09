void normalize_scores(float *scores, unsigned int mfccCount) {
  float sum = 0.f;

  for(int i = 0; i < mfccCount; i++) {
    sum += scores[i];
  }

  for(int i = 0; i < mfccCount; i++) {
    scores[i] = sum > 0 ? scores[i] / sum : 0.f;
  }
}