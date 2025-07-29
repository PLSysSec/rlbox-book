cmake_minimum_required(VERSION 3.24)
cmake_policy(VERSION 3.24)

project(rlbox_lfi_example
	VERSION 0.1
	DESCRIPTION "RLBox rlbox_lfi_example")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_language(C CXX ASM)

include(FetchContent)

FetchContent_Declare(rlbox
  GIT_REPOSITORY https://github.com/PLSysSec/rlbox.git)
FetchContent_GetProperties(rlbox)
if(NOT rlbox_POPULATED)
  FetchContent_Populate(rlbox)
endif()

FetchContent_Declare(rlbox_lfi_sandbox
  GIT_REPOSITORY https://github.com/UT-Security/rlbox_lfi_sandbox.git
  GIT_TAG main)
FetchContent_GetProperties(rlbox_lfi_sandbox)
if(NOT rlbox_lfi_sandbox_POPULATED)
  FetchContent_Populate(rlbox_lfi_sandbox)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  if (${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "x86_64")
    set (lfi-clang-url https://github.com/lfi-project/lfi/releases/download/v0.10/x86_64-lfi-clang.tar.gz)
  elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "aarch64")
    set (lfi-clang-url https://github.com/lfi-project/lfi/releases/download/v0.10/aarch64-lfi-clang.tar.gz)
  else()
    message(FATAL_ERROR "LFI binaries only available for x86_64 and aarch64.")
  endif()
else()
  message(FATAL_ERROR "LFI binaries only available for linux.")
endif()

FetchContent_Declare(lficlang
  URL ${lfi-clang-url}
  DOWNLOAD_EXTRACT_TIMESTAMP
)
FetchContent_GetProperties(lficlang)
if(NOT lficlang_POPULATED)
  FetchContent_Populate(lficlang)
endif()

FetchContent_Declare(
  lfi-runtime
  GIT_REPOSITORY https://github.com/lfi-project/lfi-runtime/)
FetchContent_GetProperties(lfi-runtime)
if(NOT lfi_POPULATED)
  FetchContent_Populate(lfi-runtime)
endif()

################### Build lfi runtime ###################

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  set(LFI_BUILD_TYPE "debug")
  set(LFI_SBX_BUILD_TYPE_FLAGS -O0)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
  set(LFI_BUILD_TYPE "debugoptimized")
  set(LFI_SBX_BUILD_TYPE_FLAGS -O3)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
  set(LFI_BUILD_TYPE "release")
  set(LFI_SBX_BUILD_TYPE_FLAGS -O3)
else()
  set(LFI_BUILD_TYPE "release")
  set(LFI_SBX_BUILD_TYPE_FLAGS -O3)
endif()

set(LFI_RUNTIME_DIR_NAME "build-liblfi-${LFI_BUILD_TYPE}")
set(LFI_RUNTIME_DIR "${lfi-runtime_SOURCE_DIR}/${LFI_RUNTIME_DIR_NAME}")
set(LIBLFI_PATH ${LFI_RUNTIME_DIR}/liblfi.a)

add_custom_command(OUTPUT "${LFI_RUNTIME_DIR}"
                   WORKING_DIRECTORY "${lfi-runtime_SOURCE_DIR}"
                   COMMAND meson setup ${LFI_RUNTIME_DIR_NAME} --buildtype ${LFI_BUILD_TYPE}
                   COMMENT "Configuring LFI runtime")

add_custom_command(OUTPUT "${LIBLFI_PATH}"
                   WORKING_DIRECTORY "${LFI_RUNTIME_DIR}"
                   DEPENDS ${LFI_RUNTIME_DIR}
                   COMMAND ninja
                   COMMENT "Building LFI runtime")

add_custom_target(build-liblfi ALL DEPENDS ${LIBLFI_PATH})

add_library(liblfi STATIC IMPORTED)
add_dependencies(liblfi build-liblfi)
set_target_properties(liblfi PROPERTIES
  IMPORTED_LOCATION "${LIBLFI_PATH}"
  INTERFACE_INCLUDE_DIRECTORIES "${lfi-runtime_SOURCE_DIR}/core/include;${lfi-runtime_SOURCE_DIR}/linux/include"
)

################### Build sandboxed mylib ###################

set(INCSTUB_FILENAME "mylib")
set(MYLIB_ELF "${CMAKE_BINARY_DIR}/${INCSTUB_FILENAME}")
set(MYLIB_EMBED_SRC "${rlbox_lfi_sandbox_SOURCE_DIR}/src/incstub.s")
set(MYLIB_EMBED_DEST "${CMAKE_BINARY_DIR}/incstub.s")
set(LFI_SYSROOT_PATH "${lficlang_SOURCE_DIR}/sysroot/usr/lib")

configure_file(${MYLIB_EMBED_SRC} ${MYLIB_EMBED_DEST})

add_custom_command(OUTPUT "${MYLIB_ELF}"
                   DEPENDS "${MYLIB_EMBED_SRC}" "${MYLIB_EMBED_DEST}" ${CMAKE_SOURCE_DIR}/mylib.c
                   COMMAND  PATH=$ENV{PATH}:${lficlang_SOURCE_DIR}/lfi-bin
                            ${lficlang_SOURCE_DIR}/bin/clang
                            ${LFI_SBX_BUILD_TYPE_FLAGS}
                            -Wl,--export-dynamic
                            -static-pie
                            -o ${MYLIB_ELF}
                            ${CMAKE_SOURCE_DIR}/mylib.c
                            -L ${LFI_SYSROOT_PATH} -lboxrt
                   COMMENT "Building lfi sandboxed library")

add_custom_target(build-sandboxed-library ALL DEPENDS ${MYLIB_ELF})

################### Build the sandboxed native library ###################

add_executable(main main.cpp ${MYLIB_EMBED_DEST})
target_include_directories(main PUBLIC ${rlbox_SOURCE_DIR}/code/include
                                PUBLIC ${rlbox_lfi_sandbox_SOURCE_DIR}/include)
target_link_libraries(main liblfi)
add_dependencies(main build-sandboxed-library)