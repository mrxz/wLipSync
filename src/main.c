#include "wasm.h"

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