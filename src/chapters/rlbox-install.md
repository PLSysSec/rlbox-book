# Setting up your RLBox environment.

RLBox current spans two repositories. One that contains just the RLBox C++ framework
which you can grab with:

```bash
git clone git@github.com:PLSysSec/rlbox.git
```

The other, which contains our modified version of `wasm2c` and related backend
tools, for converting your C library to an isolated and sandboxed version can
be grabbed with:

```bash
git clone https://github.com/PLSysSec/rlbox_wasm2c_sandbox
```

This repo contains our modified version of wasm2c, a wasm runtime (and very
limited wasi runtime), and pulls down a copy of the wasi-sdk and the rlbox
framework as part of its build process, providing a single location for all our
tools, which is handy for example purposes and getting started.

Many folks perfer to do a system wide or per-user install of the wasi-sdk and
RLBox. The latest releases of the wasi-sdk (which will given you everything you
need to compile your library from C to wasm) be found
[here](https://github.com/WebAssembly/wasi-sdk/releases).




### Quick Install

To quickly install the RLBox repo, you can run the following:
```bash
git clone git@github.com:PLSysSec/rlbox.git
cd rlbox
cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Release
cmake --build ./build --config Release --parallel
cd build && sudo make install
```

To quickly install the rlbox tools repo, which includes everything you will
need for the tutorial you can run the following:

```bash
git clone https://github.com/PLSysSec/rlbox_wasm2c_sandbox
cd rlbox_wasm2c_sandbox
cmake -S . -B ./build
cmake --build ./build --target all
```

More detailed setup instructions can be found in the README of each repo.


