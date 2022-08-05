#include <stdio.h>
#include "mylib.h"

void hello() {
  printf("Hello from mylib\n");
}

unsigned add(unsigned a, unsigned b) {
  return a + b;
}

void echo(const char* str) {
  printf("echo: %s\n", str);
}

void call_cb(void (*cb) (const char* str)) {
  cb("hi again!");
}
