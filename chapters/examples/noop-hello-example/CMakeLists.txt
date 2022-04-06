cmake_minimum_required(VERSION 3.13)

project(rlbox_cmake_test
	VERSION 0.1
	DESCRIPTION "RLBox rlbox_cmake_test")

find_package(rlbox-sandboxing-api CONFIG REQUIRED)

add_library(lib mylib.c)

add_executable(main main.cpp)
target_link_libraries(main rlbox-sandboxing-api lib)
