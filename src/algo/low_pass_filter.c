#include "../pt_math.h"

void low_pass_filter_impl(float* data, int len, float cutoff, float* tmp, float* b, int bLen, int skip) {
    for (int i = 0; i < bLen; ++i)
    {
        float x = i - (bLen - 1) / 2.f;
        float ang = 2.f * PT_PI * cutoff * x;
        b[i] = 2.f * cutoff * PT_sinf(ang) / ang;
    }

    for (int j = 0; j < bLen; ++j)
    {
        // Start i at j rounded up to the nearest multiple of skip
        for (int i = j + (skip - j%skip)%skip; i < len; i += skip)
        {
            data[i] += b[j] * tmp[i - j];
        }
    }
}

void low_pass_filter(float* data, unsigned long size, float sampleRate, float cutoff, float range, int skip) {
    cutoff = (cutoff - range) / sampleRate;
    range /= sampleRate;

    float tmp[size];
    for(int i = 0; i < size; i++) {
        tmp[i] = data[i];
    }

    int n = (int)PT_roundf(3.1f/range);
    if(n%2) {
        n++;
    }
    float b[n];
    for(int i = 0; i < n; i++) {
        b[i] = 0;
    }

    low_pass_filter_impl(data, size, cutoff, tmp, b, n, skip);
}
