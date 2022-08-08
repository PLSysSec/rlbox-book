#ANCHOR root
RLBOX_ROOT=../rlbox_wasm2c_sandbox
#ANCHOR_END root

#RLBOX headers
RLBOX_INCLUDE=$(RLBOX_ROOT)/build/_deps/rlbox-src/code/include


#Our Wasi-SDK
WASI_SDK_ROOT=$(RLBOX_ROOT)/build/_deps/wasiclang-src/

#location of our wasi/wasm runtime
WASM2C_RUNTIME_PATH=$(RLBOX_ROOT)/build/_deps/mod_wasm2c-src/wasm2c
WASI_RUNTIME_FILES=$(addprefix $(WASM2C_RUNTIME_PATH), /wasm-rt-impl.c /wasm-rt-os-win.c  /wasm-rt-os-unix.c  /wasm-rt-wasi.c)

WASI_CLANG=$(WASI_SDK_ROOT)/bin/clang
WASI_SYSROOT=$(WASI_SDK_ROOT)/share/wasi-sysroot
WASM2C=$(RLBOX_ROOT)/build/_deps/mod_wasm2c-src/bin/wasm2c

#cflags for building wasm2c output
WASM_CFLAGS=-Wl,--export-all -Wl,--no-entry -Wl,--growable-table -Wl,--stack-first -Wl,-z,stack-size=1048576

all: mylib.wasm mylib.wasm.c myapp

clean:
	rm -rf mylib.wasm mylib.wasm.c mylib.wasm.h myapp *.o

mylib.wasm: mylib.c
	$(WASI_CLANG) --sysroot $(WASI_SYSROOT) $(WASM_CFLAGS) dummy_main.c mylib.c -o mylib.wasm

mylib.wasm.c:
	$(WASM2C) mylib.wasm -o mylib.wasm.c

myapp:
	$(CC) -c $(WASI_RUNTIME_FILES) -I$(RLBOX_INCLUDE) -I$(RLBOX_ROOT)/include -I$(WASM2C_RUNTIME_PATH) mylib.wasm.c
	$(CXX) main.cpp -o myapp -I$(RLBOX_INCLUDE) -I$(RLBOX_ROOT)/include -I$(WASM2C_RUNTIME_PATH) *.o
