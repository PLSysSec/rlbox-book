# Adding isolation with the Wasm sandbox backend

The `noop` backend makes it easy to add security checks. However, it does not enforce
isolation. To finish sandboxing your library, you will need to.

1. Compile your library e.g. `mylib.c` to wasm i.e. `mylib.wasm` -- adding
   isolation to your library.
2. Compile that resulting `mylib.wasm` file to C (`mylib.wasm.c` and `mylib.wasm.h`)
   with the `wasm2c` compiler -- converting it to a form that can be compiled and
   linked with your application.
3. Update the application `main.hpp` to use the `wasm2c` sandbox backend instead
   of `noop`.
4. Compile and link your application `main.cpp`, with the now sandboxed library.

We will look at each these steps next.


##Our example Makefile

Doing all of these steps a command at a time would be terribly tedious. Instead,
we automate all these steps with a simple make file. Here we will walk through
the make file to explain each step.






## Compiling your library to Wasm

To compile your library to wasm, use the wasi-sdki clang compiler to
convert `mylib.c` to wasm.

```bash
cd myapp
```










## Compiling from Wasm to C

## Linking the library and R



