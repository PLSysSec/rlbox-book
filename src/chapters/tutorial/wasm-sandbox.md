# Adding isolation with the Wasm sandbox backend

The `noop` backend makes it easy to add security checks. However, it does not enforce
isolation. To finish sandboxing your library, we will need to:

1. Update the application `main.cpp` to use the `wasm2c` sandbox backend instead
   of `noop`.

2. Compile our library e.g. `mylib.c` to wasm i.e. `mylib.wasm` - adding
   isolation to your library. Compile that resulting `mylib.wasm` file to C
   (`mylib.wasm.c` and `mylib.wasm.h`) with the `wasm2c` compiler - allow it to
   be compiled and linked with our application.

We will look at each these steps next and end with instructions on how you can
try this out.

## Modifying the application to use the wasm2c RLBox plugin/backend

Making this change is very simple with RLBox. In fact, it can be done
exclusively in the boilerplate. Here is the boilerplate to use the `wasm2c`
backend.
```
{{#include ../../examples/wasm-hello-example/main-withanchors.cpp:imports}}
```

You'll probably notice that there are only a handful of changes.
- We now use `"rlbox_wasm2c_sandbox.hpp` instead of `rlbox_noop_sandbox.hpp`
- The sandbox type which is the second parameter to the macro
`RLBOX_DEFINE_BASE_TYPES_FOR` has now changed to `wasm2c` from `noop`
- The boilerplate for `RLBOX_USE_STATIC_CALLS` has changed to use the wasm2c backend's boilerplate
- The wasm2c backend/plugin requires an extra piece of boilerplate which is the name of the wasm module as specified in the macro `RLBOX_WASM2C_MODULE_NAME`
- The wasm2c backend/plugin requires the produces `mylib.wasm.h` (we'll discuss how to produce this in the next section), to be included in the file

These are mostly mechanical changes and are straightforward. Modifying the build
is perhaps slightly more challenging as building wasm libraries involves
multiple steps.


## Modifying the build to produce to wasm sandboxed library

To show how we will update the build, we will use two CMakeLists.txt files as a reference
- The [CMakeLists](/examples/noop-hello-example/CMakeLists.txt) used by the noop sandbox
- The [CMakeLists](/examples/wasm-hello-example/CMakeLists.txt) used by the wasm2c sandbox

As you can see the wasm CMakeLists is quite a bit longer. Below, we will give a
high-level overview of the steps, so you can follow what is happening in the
Wasm build.

To build and use the Wasm sandboxed library, we need several additional repos/tools
- We will need the [rlbox wasm2c plugin/backend](https://github.com/PLSysSec/rlbox_wasm2c_sandbox.git)
- We will need a version of clang that can produce Wasm files, specifically,
  Wasm files that target WebAssembly System Interface (WASI). WASI is a group of
  standards-track API specifications designed to provide a secure standard
  interface for Wasm applications. Specifically, you need WASI if you want to
  use printf, timers, anything that makes a syscall. We will thus rely on the
  [wasi-sdk](https://github.com/WebAssembly/wasi-sdk), which provides
  wasi-clang, a version of clang that can target WASI, and wasi-libc (a custom
  version of musl libc modified for this use case).
- Finally, we will use the [wasm2c](https://github.com/WebAssembly/wabt/) Wasm
  compiler. Wasm files need to be compiled into native libraries that can be
  linked in your application. Unlike regular native libraries however, these
  libraries are produces by sandboxed compiler is guaranteed to be sandboxed.
  The `wasm2c` compiler in particular compiles Wasm files by first transpiling
  it to C (this produced C is basically machine code with a lot of sandboxing
  checks, and is not going to be readable), and then compiling the resulting C
  with a regular C compiler to produce native objects.

After we download these repos, we can then take the following steps

1. Build the wasm2c sandbox compiler and runtime. This is a project that can be
   built using CMake. You can read more about how to build wasm2c in their
   [readme](https://github.com/WebAssembly/wabt?tab=readme-ov-file#building-using-cmake-directly-linux-and-macos)

2. We need to compile our `mylib.c` to  `mylib.wasm` using wasi-clang. The
   command in the `CMakeLists.txt` that does this is

   ```
   ${wasiclang_SOURCE_DIR}/bin/clang
      --sysroot ${wasiclang_SOURCE_DIR}/share/wasi-sysroot/
      -O3
      -Wl,--export-all -Wl,--no-entry -Wl,--growable-table -Wl,--stack-first -Wl,-z,stack-size=1048576 -Wl,--export-table
      -o ${MYLIB_WASM}
      ${C_DUMMY_MAIN}
      ${CMAKE_SOURCE_DIR}/mylib.c
   ```

   There are a number of flags that start with `-Wl` that must be specified so
   we produce a Wasm file with the properties we'd expect. You can read more
   about these flags in the [Wasm lld docs
   page](https://lld.llvm.org/WebAssembly.html). The output file `${MYLIB_WASM}`
   corresponds to `mylib.wasm` and the input C files are `mylib.c` and
   `${C_DUMMY_MAIN}`. `${C_DUMMY_MAIN}` as the name indicates is an empty main
   function seen
   [here](https://github.com/PLSysSec/rlbox_wasm2c_sandbox/blob/master/c_src/wasm2c_sandbox_wrapper.c).
   You could avoid this dummy main by using Wasm's reactor flag.

3. Next we need to run `wasm2c` to transpile our wasm file back to a C file with
   checks. This is fairly straightforward, and you can read more about it in the
   [wasm2c documentation](https://github.com/WebAssembly/wabt/tree/main/wasm2c)

4. We now have to compile the transpiled wasm file. The process for doing this
   is described in detail in the [wasm2c
   repo](https://github.com/WebAssembly/wabt/tree/main/wasm2c#compiling-the-wasm2c-output).
   Broadly, we need to compile the transpiled files with the [wasm2c
   runtime](https://github.com/WebAssembly/wabt/tree/main/wasm2) and appropriate
   includes. This will now generate our native sandboxed library `mylib`

5. We can now build our application using `mylib`

## Building and running the wasm2c backend

To build this example on your machine, run the following commands

```bash
cd cd rlbox-book/src/chapters/examples/wasm-hello-example
cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Debug
cmake --build ./build --config Debug --parallel
```

Then run it to make sure everything is working.

```bash
./build/main
```

You should see the following output:

```bash
Hello from mylib
Got array value 7
echo: hi hi!
hello_cb: hi again!
```
