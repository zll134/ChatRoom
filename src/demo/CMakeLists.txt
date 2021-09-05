
aux_source_directory(. src)

add_executable(server ${src})
#target_link_libraries(server PRIVATE pthread)

message(WARNING  "=========message to display=========" ${CMAKE_CURRENT_SOURCE_DIR}) 
install(TARGETS server
        DESTINATION bin)