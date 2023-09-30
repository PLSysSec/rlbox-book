// ANCHOR: imports
// We're going to use RLBox in a single-threaded environment.
#define RLBOX_SINGLE_THREADED_INVOCATIONS

// All calls into the sandbox are resolved statically.
#define RLBOX_USE_STATIC_CALLS() rlbox_wasm2c_sandbox_lookup_symbol

// Provide the wasm2c module a name
#define RLBOX_WASM2C_MODULE_NAME mylib

#include <stdio.h>
#include <cassert>
#include "mylib.h"
#include "mylib.wasm.h"
#include "rlbox.hpp"
#include "rlbox_wasm2c_sandbox.hpp"


using namespace std;
using namespace rlbox;

// Define base type for mylib using the noop sandbox
RLBOX_DEFINE_BASE_TYPES_FOR(mylib, wasm2c);

// Declare callback function we're going to call from sandboxed code.
void hello_cb(rlbox_sandbox_mylib& _, tainted_mylib<const char*> str);

int main(int argc, char const *argv[]) {
// ANCHOR_END: imports
// ANCHOR: hello
  // Declare and create a new sandbox
  rlbox_sandbox_mylib sandbox;
  sandbox.create_sandbox();

  // Call the library hello function:
  sandbox.invoke_sandbox_function(hello);
// ANCHOR_END: hello

// ANCHOR: add
  // call the add function and check the result:
  auto ok = sandbox.invoke_sandbox_function(add, 3, 4)
                   .copy_and_verify([](unsigned ret){
    printf("Adding... 3+4 = %d\n", ret);
    return ret == 7;
  });
  printf("OK? = %d\n", ok);
// ANCHOR_END: add

// ANCHOR: echo-pre
  // Call the library echo function
  const char* helloStr = "hi hi!";
  size_t helloSize = strlen(helloStr) + 1;
  tainted_mylib<char*> taintedStr = sandbox.malloc_in_sandbox<char>(helloSize);
  strncpy(taintedStr
            .unverified_safe_pointer_because(helloSize, "writing to region")
         , helloStr, helloSize);
// ANCHOR_END: echo-pre
// ANCHOR: echo
  sandbox.invoke_sandbox_function(echo, taintedStr);
  sandbox.free_in_sandbox(taintedStr);
// ANCHOR_END: echo

// ANCHOR: call_cb
  // register callback and call it
  auto cb = sandbox.register_callback(hello_cb);
  sandbox.invoke_sandbox_function(call_cb, cb);
// ANCHOR_END: call_cb

// ANCHOR: main-end
  // destroy sandbox
  sandbox.destroy_sandbox();

  return 0;
}
// ANCHOR_END: main-end

// ANCHOR: callback
void hello_cb(rlbox_sandbox_mylib& _, tainted_mylib<const char*> str) {
  auto checked_string =
    str.copy_and_verify_string([](unique_ptr<char[]> val) {
        assert(val != nullptr && strlen(val.get()) < 1024);
        return move(val);
    });
  printf("hello_cb: %s\n", checked_string.get());
}
// ANCHOR_END: callback
