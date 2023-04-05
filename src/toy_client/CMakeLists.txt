set(include_dir ${CMAKE_CURRENT_SOURCE_DIR}/../core
                ${CMAKE_CURRENT_SOURCE_DIR}/.
)

INCLUDE_DIRECTORIES(${include_dir})
add_compile_options(-g)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../core core)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/. src)

add_executable(toyc ${src} ${core})
target_link_libraries(toyc PRIVATE pthread)

install(TARGETS toyc
        DESTINATION bin)