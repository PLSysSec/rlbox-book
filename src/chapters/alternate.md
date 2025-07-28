# Alternate isolation backends

WebAssembly with wasm2c is just one way to isolate running code. We have
focussed on this so far as this is the approach that is being used in production
in [Firefox
today](https://hacks.mozilla.org/2021/12/webassembly-and-back-again-fine-grained-sandboxing-in-firefox-95/).
However, RLBox's plugin model is completely general, and can be configured to
support other isolation backends as well.

Note that the below plugins are experimental and are not actively maintained
(which may mean they have compilation bugs that you'd have to fix). This is
meant purely as a reference for you to write your own plugins.

## Experimental/previously-used RLBox plugins for isolation

- Using [LFI](https://dl.acm.org/doi/pdf/10.1145/3620665.3640408)\
[https://github.com/UT-Security/rlbox_lfi_sandbox](https://github.com/UT-Security/rlbox_lfi_sandbox)

- Using WebAssembly through [Lucet](https://github.com/bytecodealliance/lucet)\
[https://github.com/PLSysSec/rlbox_lucet_sandbox](https://github.com/PLSysSec/rlbox_lucet_sandbox)

- Using WebAssembly through [Wamr](https://bytecodealliance.github.io/wamr.dev/)\
[https://github.com/PLSysSec/rlbox_wamr_sandbox](https://github.com/PLSysSec/rlbox_wamr_sandbox)

- Using WebAssembly through [Wasmtime](https://wasmtime.dev/)\
[https://github.com/PLSysSec/rlbox_wasmtime_sandbox](https://github.com/PLSysSec/rlbox_wasmtime_sandbox)

- Using Google's [Native Client](https://www.chromium.org/nativeclient/getting-started/getting-started-background-and-basics/)\
[https://github.com/PLSysSec/rlbox_nacl_sandbox](https://github.com/PLSysSec/rlbox_nacl_sandbox)
