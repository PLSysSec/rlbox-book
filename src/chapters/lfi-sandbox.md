# Adding isolation with the LFI sandbox backend

Like with the wasm2c-Wasm isolation backend, you can also add isolation with
LFI. To do this, we will need to:


1. Update the application `main.cpp` to use the `lfi` sandbox backend instead
   of `noop`.

2. Compile our library e.g. `mylib.c` using the lfi compiler i.e. `mylib.so` -
   adding isolation to your library.

3. Compile and link the sandboxed library and our application.

We will look at each these steps next.

## Switching to the lfi backend