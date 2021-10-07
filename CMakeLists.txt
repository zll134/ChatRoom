cmake_minimum_required(VERSION 3.5)

# set the project name
project (ChattingRoom)

# set compiler
set (CMAKE_C_COMPILER "gcc")
set (CMAKE_CXX_COMPILER "g++")

# add compile options

add_compile_options(-g -o2)
add_subdirectory(src)