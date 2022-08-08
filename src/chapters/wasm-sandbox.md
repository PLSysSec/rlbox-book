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


## Our example Makefile

Doing all of these steps a command at a time would be terribly tedious. Instead,
we automate all these steps with a simple make file. Lets take a look at
our full make file, then walk through each part.

```c
{{#include examples/wasm-hello-example/Makefile}}
```

## definitions

To start we can see our Makefile begins with RLBOX_ROOT, which just specificies
where our `rlbox_wasm2c_sandbox` repo's root directory lives. 

This repo contains
our modified version of wasm2c, a wasm runtime (and very limited wasi runtime),
and pulls down a copy of the wasi-sdk and the rlbox framework as part of its
build process, providing a single location for all our tools, which is handy
for example purposes and some developers.

Many users folks perfer to do a system wide or per-user install of the wasi-sdk
and RLBox. 

















## Compiling your library to Wasm

To compile your library to wasm, use the wasi-sdki clang compiler to
convert `mylib.c` to wasm.

```bash
cd myapp
```










## Compiling from Wasm to C

## Linking the library and R



