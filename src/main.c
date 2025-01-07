#include "wasm.h"
#include "algo/algo.h"
#include "pt_math.h"

unsigned long strlen(const char *s) {
  unsigned long rc = 0;
  while (s[rc]) {
    ++rc;
  }

  return rc;
}

JS_FUNC void js_print(char const *str, unsigned long len);
#define JS_PRINT(str) js_print(str, strlen(str))

int main() {
  JS_PRINT("Hello world");

  char buffer[100];
  float pi = 3.14159;
  JS_PRINT(buffer);
  return 0;
}

float execute() {
	int buffer_size = 3;
	float buffer[3] = {0.1f, 0.3f, 10.0f};

	int outputSampleRate = 48000; // Based on actual output
	int targetSampleRate = 100; // Depends on profile
	int cutoff = targetSampleRate / 2;
	int range = 500;

	// CopyRingBuffer(input, out var buffer, startIndex);

	// LowPassFilter(ref buffer, outputSampleRate, cutoff, range);
	low_pass_filter(buffer, buffer_size, outputSampleRate, cutoff, range);

	// DownSample(buffer, out var data, outputSampleRate, targetSampleRate);
	float data[buffer_size];
	int data_size = buffer_size;
	if(outputSampleRate <= targetSampleRate) {
		// FIXME: More efficient buffer copy?
		down_sample_exact(buffer, data, data_size, 1);
	} else if(outputSampleRate % targetSampleRate == 0) {
		int skip = outputSampleRate / targetSampleRate;
		data_size = buffer_size / skip;
		down_sample_exact(buffer, data, data_size, skip);
	} else {
		float df = (float)outputSampleRate / targetSampleRate;
		data_size = (int)PT_round(buffer_size / df);
		down_sample(buffer, buffer_size, data, data_size, df);
	}

	// PreEmphasis(ref data, 0.97f);
	// HammingWindow(ref data);
	// Normalize(ref data, 1f);
	// FFT(data, out var spectrum);
	// MelFilterBank(spectrum, out var melSpectrum, targetSampleRate, melFilterBankChannels);
	// PowerToDb(ref melSpectrum);
	// DCT(melSpectrum, out var melCepstrum);

	return 1;
}