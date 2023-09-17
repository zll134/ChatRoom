
# compress_test 测试模块
add_executable(compress_test compress_test.c)
target_link_libraries(compress_test PRIVATE pthread toy_core)


install(TARGETS compress_test
        DESTINATION test_bin)
