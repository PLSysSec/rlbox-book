#Using RLBox with a Wasm Sandbox.

Securely isolating your example library you will need to:

1. compile your library e.g. (`mylib.c` to wasm).
2. compile that resulting *.wasm file to C code (using our custom `wasm2c`)
3. compile and link that code into your application.



