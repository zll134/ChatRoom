
# dict_test 测试模块
add_executable(dict_test dict_test.c)
target_link_libraries(dict_test PRIVATE pthread toy_core)

# ringbuff_test 测试模块
add_executable(ringbuff_test ringbuff_test.c)
target_link_libraries(ringbuff_test PRIVATE pthread toy_core)

install(TARGETS ringbuff_test
                dict_test
        DESTINATION test_bin)
