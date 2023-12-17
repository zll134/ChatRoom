
# compress_test 测试模块
add_executable(compress_test compress_test.c)
target_link_libraries(compress_test PRIVATE pthread toy_core)

# compress_test 测试模块
add_executable(huffman_test huffman_test.c)
target_link_libraries(huffman_test PRIVATE pthread toy_core)


install(TARGETS compress_test
                huffman_test
        DESTINATION test_bin)
