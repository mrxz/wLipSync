void copy_ring_buffer(float *buffer, float *inputBuffer,
                      unsigned long startIndex, unsigned long inputBufferSize,
                      unsigned long outputBufferSize) {
  startIndex = (startIndex + inputBufferSize) % inputBufferSize;
  for(int i = 0; i < outputBufferSize; i++) {
    buffer[i] = inputBuffer[(startIndex + i) % inputBufferSize];
  }
}