# Modifying a Makefile based project to compile to Wasm

This tutorial assumes you have the
[rlbox_wasm2c_sandbox](https://github.com/PLSysSec/rlbox_wasm2c_sandbox/) git
repo is in the path $RLBOX_WASM2C_PATH, and you have installed
[wasi-sdk](https://github.com/WebAssembly/wasi-sdk/releases) on your computer in
the path $WASI_SDK_PATH

Build the sources of your library along with the file
`$RLBOX_WASM2C_PATH/c_src/wasm2c_sandbox_wrapper.c`. Pass the flags `-Wl,--export-all -Wl,--stack-first -Wl,-z,stack-size=262144 -Wl,--no-entry -Wl,--growable-table -Wl,--import-memory -Wl,--import-table`
to the linker using the wasi-clang compiler. This will produce a wasm module.

To edit an existing Make based build system, you can run the commmand.

```bash
$WASI_SDK_PATH/bin/clang --sysroot $WASI_SDK_PATH/share/wasi-sysroot $RLBOX_WASM2C_PATH/c_src/wasm2c_sandbox_wrapper.c -c -o $RLBOX_WASM2C_PATH/c_src/wasm2c_sandbox_wrapper.o

AR=$WASI_SDK_PATH/bin/ar                                  \
CC=$WASI_SDK_PATH/bin/clang                               \
CXX=$WASI_SDK_PATH/bin/clang++                            \
CFLAGS="--sysroot $WASI_SDK_PATH/share/wasi-sysroot"      \
LD=$WASI_SDK_PATH/bin/wasm-ld                             \
LDLIBS=$RLBOX_WASM2C_PATH/c_src/wasm2c_sandbox_wrapper.o  \
LDFLAGS="-Wl,--export-all -Wl,--stack-first -Wl,-z,stack-size=262144 -Wl,--no-entry -Wl,--growable-table -Wl,--import-memory -Wl,--import-table"   \
make
```

If the Makefile you are modifying produces a binary executable, this will change it produce a Wasm executable instead and that should be it.

If the Makefile produces a static library (e.g., `mylib.a`) instead. You need
one additional step to make it usable as a Wasm library that can be imported by
rlbox.

First create a dummy main file somewhere called `dummy-main.c` consisting of

```c
int main(int argc, char**argv){
    return 0;
}
```

Then you can run the following command

```bash
$WASI_SDK_PATH/bin/clang --sysroot $WASI_SDK_PATH/share/wasi-sysroot \
    -o mylib.wasm dummy-main.c                                       \
    -Wl,--whole-archive mylib.a -Wl,--no-whole-archive               \
    -Wl,--export-all
```

This compiles a Wasm file with all symbols from the library included and exported.