#include "../pt_math.h"

void low_pass_filter_impl(float* data, int len, float cutoff, float* tmp, float* b, int bLen) {
    for (int i = 0; i < bLen; ++i)
    {
        float x = i - (bLen - 1) / 2.f;
        float ang = 2.f * PT_PI * cutoff * x;
        b[i] = 2.f * cutoff * PT_sin(ang) / ang;
    }

    for (int i = 0; i < len; ++i)
    {
        for (int j = 0; j < bLen; ++j)
        {
            if (i - j >= 0)
            {
                data[i] += b[j] * tmp[i - j];
            }
        }
    }
}

void low_pass_filter(float* data, unsigned long size, float sampleRate, float cutoff, float range) {
    cutoff = (cutoff - range) / sampleRate;
    range /= sampleRate;

    float tmp[size];

    int n = (int)PT_roundf(3.1f/range);
    if(n%2) {
        n++;
    }
    float b[n];

    low_pass_filter_impl(data, size, cutoff, tmp, b, n);
}
