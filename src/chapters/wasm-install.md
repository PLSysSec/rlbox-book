# Building and Installing our Wasm sandboxing toolchain

Clone the repo containing the portion of the rlbox toolkit that supports wasm
sandboxing.

```bash
git clone https://github.com/PLSysSec/rlbox_wasm2c_sandbox
```

`cd` into the repo, and build the tools. This will also download the wasi-sdk, the
wasm2c compiler, and rlbox.


```bash
cd rlbox_wasm2c_sandbox
cmake -S . -B ./build
cmake --build ./build --target all
```
