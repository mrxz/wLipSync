void pre_emphasis(float* data, int len, float p) {
  for(int i = len; i >= 1; --i) {
    data[i] = data[i] - p * data[i - 1];
  }
}