# Adding isolation with the Wasm sandbox backend

The `noop` backend makes it easy to add security checks to enforce a trust
boundary between your library and application code. However, it does not enforce
memory or control isolation, thus provides no protection.

To add isolation, you will need to switch from using the `noop` to the `wasm2c`
backend.

Securely isolating your example library you will need to:

1. Switch you application to use the wasm backend.
2. Compile your library e.g. (`mylib.c` to wasm).
3. Compile that resulting *.wasm file to C code (using our custom `wasm2c`)
4. Compile and link the wasm2c generated code, and the Wasm and Wasi runtime
   with your application.

## Switching to the Wasm backend

## Compiling your library to Wasm

To compile your library to wasm, you will use the clang compiler provided in the
wasi-sdk (sometimes called "wasi clang"), to compile all of your libraries
C files into a single wasm file.

## Compiling from Wasm to C

## Linking the library and R



