#include <stdio.h>
#include <stdlib.h>

#define release_assert(cond, msg) if (!(cond)) { fputs(msg "\n", stderr); abort(); }

#include "mylib.h"

using namespace std;

// Declare callback function that's going to be invoked from the library.
void hello_cb(const char* str);

int main(int argc, char const *argv[]) {
  // Call the library hello function
// ANCHOR: hello
  hello();
// ANCHOR_END: hello

// ANCHOR: add-context
  int array[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  // Call the library add function
  auto ret = add(3, 4);
  auto array_val = array[ret];
// ANCHOR_END: add-context
  printf("Got array value %d\n", ret);

  // Call the library echo function
  const char* helloStr = "hi hi!";
  echo(helloStr);

  // Call the library function call_cb, passing in the callback hello_cb
  call_cb(hello_cb);

  return 0;
}

// ANCHOR: callback
void hello_cb(const char* str)
{
  release_assert(str != nullptr, "Expected value for string");
  printf("hello_cb: %s\n", str);
}
// ANCHOR_END: callback
