
add_executable(hello_cmake test.c)

install(TARGETS hello_cmake 
        DESTINATION bin)