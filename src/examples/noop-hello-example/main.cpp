#include <stdio.h>
#include <stdlib.h>

#define release_assert(cond, msg) if (!(cond)) { fputs(msg "\n", stderr); abort(); }

#include "mylib.h"

using namespace std;

// We're going to use RLBox in a single-threaded environment.
#define RLBOX_SINGLE_THREADED_INVOCATIONS
// The fixed configuration line we need to use for the noop sandbox.
// It specifies that all calls into the sandbox are resolved statically.
#define RLBOX_USE_STATIC_CALLS() rlbox_noop_sandbox_lookup_symbol

#include "rlbox.hpp"
#include "rlbox_noop_sandbox.hpp"

using namespace rlbox;

// Define base type for mylib using the noop sandbox
RLBOX_DEFINE_BASE_TYPES_FOR(mylib, noop);

// Declare callback function that's going to be invoked from the library.
void hello_cb(rlbox_sandbox_mylib& _, tainted_mylib<const char*> str);

int main(int argc, char const *argv[]) {
  // Declare and create a new sandbox
  rlbox_sandbox_mylib sandbox;
  sandbox.create_sandbox();

  // Call the library hello function
  sandbox.invoke_sandbox_function(hello);

int array[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
// Call the library add function
  auto ret = sandbox.invoke_sandbox_function(add, 3, 4)
                   .copy_and_verify([](unsigned val){
    release_assert(val < 10, "Unexpected result");
    return val;
  });
  auto array_val = array[ret];
  printf("Got array value %d\n", ret);

  // Call the library echo function
  const char* helloStr = "hi hi!";
  size_t helloSize = strlen(helloStr) + 1;
  tainted_mylib<char*> taintedStr = sandbox.malloc_in_sandbox<char>(helloSize);
  strncpy(taintedStr
            .unverified_safe_pointer_because(helloSize, "writing to region")
         , helloStr, helloSize);
  sandbox.invoke_sandbox_function(echo, taintedStr);
  sandbox.free_in_sandbox(taintedStr);

  // register callback
  auto cb = sandbox.register_callback(hello_cb);
  // Call the library function call_cb, passing in the callback hello_cb
  sandbox.invoke_sandbox_function(call_cb, cb);

  // destroy sandbox
  sandbox.destroy_sandbox();

  return 0;
}

void hello_cb(rlbox_sandbox_mylib& _, tainted_mylib<const char*> str) {
  auto checked_string =
    str.copy_and_verify_string([](unique_ptr<char[]> val) {
        release_assert(val != nullptr && strlen(val.get()) < 1024, "val is null or greater than 1024\n");
        return move(val);
    });
  printf("hello_cb: %s\n", checked_string.get());
}
