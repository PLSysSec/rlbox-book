#include <stdio.h>
#include <stdlib.h>

#define release_assert(cond, msg) if (!(cond)) { fputs(msg "\n", stderr); abort(); }

#include "mylib.h"

using namespace std;

// ANCHOR: imports
// We're going to use RLBox in a single-threaded environment.
#define RLBOX_SINGLE_THREADED_INVOCATIONS

#include "rlbox.hpp"
#include "rlbox_lfi_sandbox.hpp"

using namespace rlbox;

extern "C" {
extern uint8_t mylib_start[];
extern uint8_t mylib_end[];
};

// Define base type for mylib using the noop sandbox
RLBOX_DEFINE_BASE_TYPES_FOR(mylib, lfi);
// ANCHOR_END: imports

// Declare callback function that's going to be invoked from the library.
void hello_cb(rlbox_sandbox_mylib& _, tainted_mylib<const char*> str);

int main(int argc, char const *argv[]) {
// ANCHOR: create
  // Declare and create a new sandbox
  rlbox_sandbox_mylib sandbox;
  sandbox.create_sandbox(mylib_start, mylib_end);
// ANCHOR_END: create

  // Call the library hello function
// ANCHOR: hello
  sandbox.invoke_sandbox_function(hello);
// ANCHOR_END: hello

int array[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
// Call the library add function
// ANCHOR: add
  auto ret = sandbox.invoke_sandbox_function(add, 3, 4)
// ANCHOR_END: add
// ANCHOR: add-copy-and-verify
                   .copy_and_verify([](unsigned val){
// ANCHOR_END: add-copy-and-verify
// ANCHOR: add-verifier
    release_assert(val < 10, "Unexpected result");
// ANCHOR_END: add-verifier
// ANCHOR: add-copy-and-verify-close
    return val;
  });
// ANCHOR_END: add-copy-and-verify-close
// ANCHOR: add-tainted-use
  auto array_val = array[ret];
// ANCHOR_END: add-tainted-use
  printf("Got array value %d\n", ret);

// ANCHOR: echo-pre
  // Call the library echo function
  const char* helloStr = "hi hi!";
  size_t helloSize = strlen(helloStr) + 1;
  tainted_mylib<char*> taintedStr = sandbox.malloc_in_sandbox<char>(helloSize);
  strncpy(sandbox, taintedStr, helloStr, helloSize);
// ANCHOR_END: echo-pre
// ANCHOR: echo
  sandbox.invoke_sandbox_function(echo, taintedStr);
  sandbox.free_in_sandbox(taintedStr);
// ANCHOR_END: echo

// ANCHOR: call_cb
  // register callback
  auto cb = sandbox.register_callback(hello_cb);
  // Call the library function call_cb, passing in the callback hello_cb
  sandbox.invoke_sandbox_function(call_cb, cb);
// ANCHOR_END: call_cb

// ANCHOR: destroy
  // destroy sandbox
  sandbox.destroy_sandbox();
// ANCHOR_END: destroy

  return 0;
}

// ANCHOR: callback-signature
void hello_cb(rlbox_sandbox_mylib& sandbox, tainted_mylib<const char*> str)
// ANCHOR_END: callback-signature
{
  release_assert(str != nullptr, "Expected value for string");
  auto checked_string =
    str.copy_and_verify_string([](unique_ptr<char[]> val) {
        release_assert(val != nullptr && strlen(val.get()) < 1024, "val is null or greater than 1024\n");
        return move(val);
    });
  printf("hello_cb: %s\n", checked_string.get());
}
// ANCHOR_END: callback
