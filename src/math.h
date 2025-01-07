inline int min(int a, int b) {
    return a < b ? a : b;
}

inline float lerp(float a, float b, float t) {
    return a + (b - a)*t;
}