INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/.)

set(srcs ${CMAKE_CURRENT_SOURCE_DIR}/compress.c
         ${CMAKE_CURRENT_SOURCE_DIR}/zlib_wrapper.c )

add_executable(crmcomp ${srcs})
target_link_libraries(crmcomp PRIVATE z)