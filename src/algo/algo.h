void low_pass_filter(float* data, unsigned long size, float sampleRate, float cutoff, float range);
void down_sample_exact(float* input, float* output, unsigned long output_size, int skip);
void down_sample(float* input, unsigned long size, float* output, unsigned long output_size, float df);
