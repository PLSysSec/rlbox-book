RLBOX_ROOT=../rlbox_wasm2c_sandbox

#RLBOX headers
RLBOX_INCLUDE=$(RLBOX_ROOT)/build/_deps/rlbox-src/code/include


#Our Wasi-SDK
WASI_SDK_ROOT=$(RLBOX_ROOT)/build/_deps/wasiclang-src/

#location of our wasi/wasm runtime
WASM2C_RUNTIME_PATH=$(RLBOX_ROOT)/build/_deps/mod_wasm2c-src/wasm2c
WASI_RUNTIME_FILES=$(addprefix $(WASM2C_RUNTIME_PATH), /wasm-rt-impl.c /wasm-rt-mem-impl.c)

#Some more wasi/wasm runtime files
WASM2C_RUNTIME_FILES=$(addprefix $(RLBOX_ROOT)/src, /wasm2c_rt_minwasi.c /wasm2c_rt_mem.c)

WASI_CLANG=$(WASI_SDK_ROOT)/bin/clang
WASI_SYSROOT=$(WASI_SDK_ROOT)/share/wasi-sysroot
WASM2C=$(RLBOX_ROOT)/build/_deps/mod_wasm2c-src/bin/wasm2c

#CFLAGS for compiling files to place nice with wasm2c
WASM_CFLAGS=-Wl,--export-all -Wl,--stack-first -Wl,-z,stack-size=262144 -Wl,--no-entry -Wl,--growable-table -Wl,--import-memory -Wl,--import-table

all: mylib.wasm mylib.wasm.c myapp

clean:
	rm -rf mylib.wasm mylib.wasm.c mylib.wasm.h myapp *.o

#Step 1: build our library into wasm, using clang from the wasi-sdk
mylib.wasm: mylib.c
	$(WASI_CLANG) --sysroot $(WASI_SYSROOT) $(WASM_CFLAGS) dummy_main.c mylib.c -o mylib.wasm

#Step 2: use wasm2c to convert our wasm to a C implementation of wasm we can link with our app.
mylib.wasm.c: mylib.wasm
	$(WASM2C) mylib.wasm -o mylib.wasm.c

#Step 3: compiling and linking our application with our library
myapp: mylib.wasm.c
	$(CC) -c $(WASI_RUNTIME_FILES) $(WASM2C_RUNTIME_FILES) -I$(RLBOX_INCLUDE) -I$(RLBOX_ROOT)/include -I$(WASM2C_RUNTIME_PATH) mylib.wasm.c
	$(CXX) -std=c++17 main.cpp -o myapp -I$(RLBOX_INCLUDE) -I$(RLBOX_ROOT)/include -I$(WASM2C_RUNTIME_PATH) *.o -lpthread
