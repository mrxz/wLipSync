void copy_ring_buffer(float* outputBuffer, float* inputBuffer, unsigned long startIndex, unsigned long inputBufferSize, unsigned long outputBufferSize);
void low_pass_filter(float* data, unsigned long size, float sampleRate, float cutoff, float range);
void down_sample_exact(float* input, float* output, unsigned long output_size, int skip);
void down_sample(float* input, unsigned long size, float* output, unsigned long output_size, float df);
void pre_emphasis(float* data, int len, float p);
void hamming_window(float* data, int len);
void normalize(float* data, int len, float value);
void fft(float* data, float* spectrum, unsigned long size);
void mel_filter_bank(float* spectrum, unsigned long spectrumSize, float* melSpectrum, float sampleRate, int melDiv);
void power_to_db(float* array, unsigned long size);
void dct(float* spectrum, float* cepstrum, unsigned long size);

float rms_volume(float* array, unsigned long size);