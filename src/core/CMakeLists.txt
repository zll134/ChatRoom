set(include_dir ${CMAKE_CURRENT_SOURCE_DIR}/.)

INCLUDE_DIRECTORIES(${include_dir})
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/. src)

add_library(toy_core ${src} )
target_link_libraries(toy_core PRIVATE pthread)
