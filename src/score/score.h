#define L1_NORM 0
#define L2_NORM 1
#define COSINE_SIMILARITY 2

void calc_l1norm_scores(float* mfcc, float* profileMfcc, float* means, float* standardDeviations, float* scores, unsigned int mfccCount);
void calc_l2norm_scores(float* mfcc, float* profileMfcc, float* means, float* standardDeviations, float* scores, unsigned int mfccCount);
void calc_cosine_similarity_scores(float* mfcc, float* profileMfcc, float* means, float* standardDeviations, float* scores, unsigned int mfccCount);

void normalize_scores(float* scores, unsigned int mfccCount);