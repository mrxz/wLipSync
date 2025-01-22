#include "constants.h"
#include "algo/algo.h"
#include "score/score.h"
// NOTE: to ensure the computed stdDevs match with uLipSync, the precise pow implementation
//       is required. This is only used during profile pre-computation so no runtime impact.
#define PT_MATH_PRECISE_POW
#include "pt_math.h"

// Profile
struct Profile {
  int targetSampleRate;
  int sampleCount;
  int melFilterBankChannels;
  int compareMethod;
  int mfccCount;
  int mfccDataCount;

  int useStandardization;
} profile;

float profileMfccRaw[MAX_MFCC_SAMPLES * MFCC_NUM];
float profileMfcc[MAX_PHONEMES * MFCC_NUM];
float profileMeans[MFCC_NUM];
float profileStdDev[MFCC_NUM];
float* profilePtrs[3] = {profileMfcc, profileMeans, profileStdDev};

float mfccOut[MFCC_NUM];
float scores[MAX_PHONEMES];

float inputBuffer[INPUT_BUFFER_SIZE] = {0};

float volume = 0.f;

int outputSampleRate = 48000;
int inputSampleCount = 0;

float* load_profile(int targetSampleRate, int sampleCount, int melFilterBankChannels, int compareMethod, int mfccCount, int mfccDataCount, int useStandardization) {
  profile.targetSampleRate = targetSampleRate;
  profile.sampleCount = sampleCount;
  profile.melFilterBankChannels = melFilterBankChannels;
  profile.compareMethod = compareMethod;
  profile.mfccCount = mfccCount;
  profile.mfccDataCount = mfccDataCount;

  profile.useStandardization = useStandardization;

  return profileMfccRaw;
}

void precompute_profile() {
  // Zero all data
  for(int i = 0; i < MFCC_NUM; i++) {
    profileMeans[i] = 0;
    profileStdDev[i] = 0;

    for(int phoneme = 0; phoneme < profile.mfccCount; phoneme++) {
      profileMfcc[phoneme * MFCC_NUM + i] = 0;
    }
  }

  // Compute averages, means and standard deviations
  for(int phoneme = 0; phoneme < profile.mfccCount; phoneme++) {
    for(int sample = 0; sample < profile.mfccDataCount; sample++) {
      for(int i = 0; i < MFCC_NUM; i++) {
        profileMfcc[phoneme * MFCC_NUM + i] += profileMfccRaw[((phoneme * profile.mfccDataCount) + sample) * MFCC_NUM + i];
      }
    }

    for(int i = 0; i < MFCC_NUM; i++) {
      profileMfcc[phoneme * MFCC_NUM + i] /= profile.mfccDataCount;
      profileMeans[i] += profileMfcc[phoneme * MFCC_NUM + i];
    }
  }

  if(profile.useStandardization) {
    for(int i = 0; i < MFCC_NUM; i++) {
      profileMeans[i] /= profile.mfccCount;
    }

    for(int phoneme = 0; phoneme < profile.mfccCount; phoneme++) {
      for(int sample = 0; sample < profile.mfccDataCount; sample++) {
        for(int i = 0; i < MFCC_NUM; i++) {
          float rawSample = profileMfccRaw[((phoneme * profile.mfccDataCount) + sample) * MFCC_NUM + i];
          profileStdDev[i] += PT_powf(rawSample - profileMeans[i], 2.0f);
        }
      }
    }
    for(int i = 0; i < MFCC_NUM; i++) {
      profileStdDev[i] = PT_sqrtf(profileStdDev[i] / (profile.mfccCount * profile.mfccDataCount));
    }
  } else {
    // Clear means and std deviations
    for(int i = 0; i < MFCC_NUM; i++) {
      profileMeans[i] = 0;
      profileStdDev[i] = 1;
    }
  }
}

float** get_profile_ptrs() {
  // Return pointer to a list of pointers to the various computed arrays
  return profilePtrs;
}

void set_input(int sampleRate) {
	outputSampleRate = sampleRate;
	float r = (float)outputSampleRate / profile.targetSampleRate;
	inputSampleCount = PT_ceilf(profile.sampleCount * r);
}

float* get_input_buffer() {
	return inputBuffer;
}

int get_input_buffer_size() {
	return INPUT_BUFFER_SIZE;
}

float* get_volume_ptr() {
  return &volume;
}

int execute(int inputBufferIndex) {
  int buffer_size = inputSampleCount;
  float buffer[buffer_size];

  int targetSampleRate = profile.targetSampleRate;
  int cutoff = targetSampleRate / 2;
  int range = 500;
  int melFilterBankChannels = profile.melFilterBankChannels;

  // CopyRingBuffer(input, out var buffer, startIndex);
  copy_ring_buffer(buffer, inputBuffer, (inputBufferIndex + INPUT_BUFFER_SIZE - buffer_size), INPUT_BUFFER_SIZE, buffer_size);

  volume = rms_volume(buffer, buffer_size);

  // LowPassFilter(ref buffer, outputSampleRate, cutoff, range);
  // DownSample(buffer, out var data, outputSampleRate, targetSampleRate);
  float data[buffer_size];
  int data_size = buffer_size;
  if(outputSampleRate <= targetSampleRate) {
    low_pass_filter(buffer, buffer_size, outputSampleRate, cutoff, range, 1);
    down_sample_exact(buffer, data, data_size, 1);
  } else if(outputSampleRate % targetSampleRate == 0) {
    int skip = outputSampleRate / targetSampleRate;
    data_size = buffer_size / skip;
    low_pass_filter(buffer, buffer_size, outputSampleRate, cutoff, range, skip);
    down_sample_exact(buffer, data, data_size, skip);
  } else {
    float df = (float)outputSampleRate / targetSampleRate;
    data_size = (int)PT_round(buffer_size / df);
    low_pass_filter(buffer, buffer_size, outputSampleRate, cutoff, range, 1);
    down_sample(buffer, buffer_size, data, data_size, df);
  }

  // PreEmphasis(ref data, 0.97f);
  pre_emphasis(data, data_size, 0.97f);

  // HammingWindow(ref data);
  hamming_window(data, data_size);

  // Normalize(ref data, 1f);
  normalize(data, data_size, 1.f);

  // FFT(data, out var spectrum);
  float spectrum[data_size];
  fft(data, spectrum, data_size);

  // MelFilterBank(spectrum, out var melSpectrum, targetSampleRate, melFilterBankChannels);
  float melSpectrum[melFilterBankChannels];
  mel_filter_bank(spectrum, data_size, melSpectrum, targetSampleRate, melFilterBankChannels);

  // PowerToDb(ref melSpectrum);
  power_to_db(melSpectrum, melFilterBankChannels);

  // DCT(melSpectrum, out var melCepstrum);
  float melCepstrum[melFilterBankChannels];
  dct(melSpectrum, melCepstrum, melFilterBankChannels);

  for(int i = 1; i <= profile.mfccCount; ++i) {
    mfccOut[i - 1] = melCepstrum[i];
  }

  // Calculate scores
  if(profile.compareMethod == L1_NORM) {
    calc_l1norm_scores(mfccOut, profileMfcc, profileMeans, profileStdDev, scores, profile.mfccCount);
  } else if(profile.compareMethod == L2_NORM) {
    calc_l2norm_scores(mfccOut, profileMfcc, profileMeans, profileStdDev, scores, profile.mfccCount);
  } else { // COSINE_SIMILARITY
    calc_cosine_similarity_scores(mfccOut, profileMfcc, profileMeans, profileStdDev, scores, profile.mfccCount);
  }

  // Normalize scores so they sum up to 1.
  normalize_scores(scores, profile.mfccCount);

  // Find index of phoneme with highest score
  int index = -1;
  float maxScore = -1.f;
  for(int i = 0; i < profile.mfccCount; i++) {
    if(scores[i] > maxScore) {
      index = i;
      maxScore = scores[i];
    }
  }

  return index;
}