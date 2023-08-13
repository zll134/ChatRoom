set(include_dir ${CMAKE_CURRENT_SOURCE_DIR}/.
                ${CMAKE_CURRENT_SOURCE_DIR}/base
                ${CMAKE_CURRENT_SOURCE_DIR}/test_frame
                ${CMAKE_CURRENT_SOURCE_DIR}/compress)

INCLUDE_DIRECTORIES(${include_dir})
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/base base_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/test_frame test_frame_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/compress compress_src)

add_library(toy_core ${src} ${base_src} ${test_frame_src} ${compress_src})
target_link_libraries(toy_core PRIVATE pthread)
