void copy_ring_buffer(float *outputBuffer, float *inputBuffer,
                      unsigned long startIndex, unsigned long inputBufferSize,
                      unsigned long outputBufferSize) {
  startIndex %= inputBufferSize;
  for(int i = 0; i < outputBufferSize; i++) {
    outputBuffer[i] = inputBuffer[(startIndex + i) % inputBufferSize];
  }
}