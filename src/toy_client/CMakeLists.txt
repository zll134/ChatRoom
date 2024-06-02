set(include_dir ${CMAKE_CURRENT_SOURCE_DIR}/.
                ${CMAKE_CURRENT_SOURCE_DIR}/../core
                ${CMAKE_CURRENT_SOURCE_DIR}/../core/base
)
INCLUDE_DIRECTORIES(${include_dir})

set(link_dir ${PROJECT_SOURCE_DIR}/output/src/core/)
link_directories(${link_dir})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/. src)
add_executable(toyc ${src} ${core})

target_link_libraries(toyc PRIVATE pthread toy_core)

install(TARGETS toyc
        DESTINATION bin)