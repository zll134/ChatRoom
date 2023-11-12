cmake_minimum_required(VERSION 3.5)

# set the project name
project(Toys)

# set compiler
set (CMAKE_C_COMPILER "gcc")
set (CMAKE_CXX_COMPILER "g++")

# add compile options

add_compile_options(-g)

# add debug mode
if(DEBUG_MODE)
add_compile_options(-DDEBUG_MODE)
endif()

add_subdirectory(src)

if(BUILD_TEST)
add_subdirectory(test)
endif()
