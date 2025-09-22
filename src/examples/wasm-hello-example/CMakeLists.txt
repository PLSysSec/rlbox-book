cmake_minimum_required(VERSION 3.30)

# TODO: Migrate from FetchContent_Populate
cmake_policy(SET CMP0169 OLD)

project(rlbox_wasm_example
	VERSION 0.1
	DESCRIPTION "RLBox rlbox_wasm_example")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(rlbox
  GIT_REPOSITORY https://github.com/PLSysSec/rlbox.git)
FetchContent_GetProperties(rlbox)
if(NOT rlbox_POPULATED)
  FetchContent_Populate(rlbox)
endif()

FetchContent_Declare(rlbox_wasm2c_sandbox
  GIT_REPOSITORY https://github.com/PLSysSec/rlbox_wasm2c_sandbox.git)
FetchContent_GetProperties(rlbox_wasm2c_sandbox)
if(NOT rlbox_wasm2c_sandbox_POPULATED)
  FetchContent_Populate(rlbox_wasm2c_sandbox)
endif()

string(TOLOWER "${CMAKE_HOST_SYSTEM_PROCESSOR}" CMAKE_HOST_SYSTEM_PROCESSOR_LOWER)

if (${CMAKE_HOST_SYSTEM_PROCESSOR_LOWER} MATCHES "x86_64" OR ${CMAKE_HOST_SYSTEM_PROCESSOR_LOWER} MATCHES "amd64")
  if (WIN32)
    set (wasi-clang-url "https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-27/wasi-sdk-27.0-x86_64-windows.tar.gz")
  elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (wasi-clang-url "https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-27/wasi-sdk-27.0-x86_64-macos.tar.gz")
  elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set (wasi-clang-url "https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-27/wasi-sdk-27.0-x86_64-linux.tar.gz")
  else()
    message (ERROR "Unknown OS")
  endif()
elseif (${CMAKE_HOST_SYSTEM_PROCESSOR_LOWER} MATCHES "arm64")
  if (WIN32)
    set (wasi-clang-url "https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-27/wasi-sdk-27.0-arm64-windows.tar.gz")
  elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (wasi-clang-url "https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-27/wasi-sdk-27.0-arm64-macos.tar.gz")
  elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set (wasi-clang-url "https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-27/wasi-sdk-27.0-arm64-linux.tar.gz")
  else()
    message (ERROR "Unknown OS")
  endif()
else()
  message (ERROR "Unknown arch")
endif()

FetchContent_Declare(wasiclang
  URL ${wasi-clang-url}
  DOWNLOAD_EXTRACT_TIMESTAMP false
)
FetchContent_GetProperties(wasiclang)
if(NOT wasiclang_POPULATED)
  FetchContent_Populate(wasiclang)
endif()

FetchContent_Declare(
  wasm2c_compiler
  GIT_REPOSITORY https://github.com/WebAssembly/wabt/
  GIT_TAG main)
FetchContent_GetProperties(wasm2c_compiler)
if(NOT wasm2c_compiler_POPULATED)
  FetchContent_Populate(wasm2c_compiler)
endif()

################### Build wasm2c ###################

set(WASM2C_RUNTIME_DIR "${wasm2c_compiler_SOURCE_DIR}/build/")

if(MSVC)
  set(WASM2C_PATH ${WASM2C_RUNTIME_DIR}/${CMAKE_BUILD_TYPE}/wasm2c.exe)
else()
  set(WASM2C_PATH ${WASM2C_RUNTIME_DIR}/wasm2c)
endif()

add_custom_command(OUTPUT "${WASM2C_PATH}"
                   WORKING_DIRECTORY "${wasm2c_compiler_SOURCE_DIR}"
                   COMMAND ${CMAKE_COMMAND}
                           -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                           -S  .
                           -B ${WASM2C_RUNTIME_DIR}
                   COMMAND ${CMAKE_COMMAND}
                           --build ${WASM2C_RUNTIME_DIR}
                           --config ${CMAKE_BUILD_TYPE}
                           --parallel
                   COMMENT "Building wasm2c compiler and runtime")

################### Build mylib.wasm && mylib.wasm.h,mylib.wasm.c ###################

set(MYLIB_WASM "${CMAKE_BINARY_DIR}/mylib.wasm")
set(MYLIB_H "${CMAKE_BINARY_DIR}/mylib.wasm.h")
set(MYLIB_C "${CMAKE_BINARY_DIR}/mylib.wasm.c")
set(C_DUMMY_MAIN "${rlbox_wasm2c_sandbox_SOURCE_DIR}/c_src/wasm2c_sandbox_wrapper.c")

add_custom_command(OUTPUT "${MYLIB_H}" "${MYLIB_C}" "${MYLIB_WASM}"
                   DEPENDS ${C_DUMMY_MAIN} ${WASM2C_PATH}
                    COMMAND ${wasiclang_SOURCE_DIR}/bin/clang
                            --sysroot ${wasiclang_SOURCE_DIR}/share/wasi-sysroot/
                            -O3
                            -Wl,--export-all -Wl,--no-entry -Wl,--growable-table -Wl,--stack-first -Wl,-z,stack-size=1048576 -Wl,--export-table
                            -o ${MYLIB_WASM}
                            ${C_DUMMY_MAIN}
                            ${CMAKE_SOURCE_DIR}/mylib.c
                    COMMAND ${WASM2C_PATH}
                            -o ${MYLIB_C}
                            ${MYLIB_WASM}
                    COMMENT "Building wasm sandboxed library")

################### Build the wasm sandboxed native library ###################

set(WASM2C_RUNTIME_CODE ${wasm2c_compiler_SOURCE_DIR}/wasm2c/wasm-rt-impl.c
                        ${wasm2c_compiler_SOURCE_DIR}/wasm2c/wasm-rt-mem-impl.c
                        ${rlbox_wasm2c_sandbox_SOURCE_DIR}/src/wasm2c_rt_minwasi.c
                        ${rlbox_wasm2c_sandbox_SOURCE_DIR}/src/wasm2c_rt_mem.c)

add_library(mylib STATIC ${MYLIB_C} ${WASM2C_RUNTIME_CODE})
target_include_directories(mylib PUBLIC ${wasm2c_compiler_SOURCE_DIR}/wasm2c
                                 PUBLIC ${wasm2c_compiler_SOURCE_DIR}/third_party/simde
                                 PUBLIC ${rlbox_wasm2c_sandbox_SOURCE_DIR}/include
                                 PUBLIC ${CMAKE_BINARY_DIR})

target_compile_definitions(mylib PUBLIC WASM_RT_USE_MMAP=1
                                 PUBLIC WASM_RT_SKIP_SIGNAL_RECOVERY=1
                                 PUBLIC WASM_RT_NONCONFORMING_UNCHECKED_STACK_EXHAUSTION=1)


add_executable(main main.cpp)
target_include_directories(main PUBLIC ${rlbox_SOURCE_DIR}/code/include
                                PUBLIC ${rlbox_wasm2c_sandbox_SOURCE_DIR}/include)
target_link_libraries(main mylib)
