cmake_minimum_required(VERSION 3.24)
cmake_policy(VERSION 3.24)

project(rlbox_noop_example
	VERSION 0.1
	DESCRIPTION "RLBox rlbox_noop_example")

include(FetchContent)

FetchContent_Declare(rlbox
  GIT_REPOSITORY https://github.com/PLSysSec/rlbox.git)
FetchContent_GetProperties(rlbox)
if(NOT rlbox_POPULATED)
  FetchContent_Populate(rlbox)
endif()

add_library(mylib mylib.c)

add_executable(main main.cpp)
target_include_directories(main PUBLIC ${rlbox_SOURCE_DIR}/code/include)
target_link_libraries(main mylib)
