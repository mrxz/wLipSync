#include "../math.h"

void copy_ring_buffer(float *outputBuffer, float *inputBuffer,
                      unsigned long startIndex, unsigned long inputBufferSize,
                      unsigned long outputBufferSize) {
  startIndex %= inputBufferSize;

  // Copy from the tail end of the inputBuffer (startIndex);
  int tail = min(inputBufferSize - startIndex, outputBufferSize);
  __builtin_memcpy(outputBuffer, &inputBuffer[startIndex], sizeof(float)*tail);

  // Copy from the start of the inputBuffer (index 0)
  int left = outputBufferSize - tail;
  __builtin_memcpy(&outputBuffer[outputBufferSize - left], inputBuffer, sizeof(float)*left);
}