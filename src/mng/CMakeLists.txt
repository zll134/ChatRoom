
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/main src)

add_executable(crm_mng ${src})
target_link_libraries(crm_mng PRIVATE pthread)

install(TARGETS crm_mng
        DESTINATION bin)