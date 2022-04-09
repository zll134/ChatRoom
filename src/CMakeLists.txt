set(dirs ${CMAKE_CURRENT_SOURCE_DIR}/core
        ${CMAKE_CURRENT_SOURCE_DIR}/client
        ${CMAKE_CURRENT_SOURCE_DIR}/server
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
include_directories(${dirs})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/core core)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/client client)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/server server)

add_executable(crmserver ${core} ${server})
target_link_libraries(crmserver PRIVATE pthread)

add_executable(crmclient ${core} ${client})
target_link_libraries(crmclient PRIVATE pthread)

install(TARGETS crmserver
        DESTINATION bin)

install(TARGETS crmclient
        DESTINATION bin)
