INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/core
                    ${PROJECT_SOURCE_DIR}/test/framework)

set(frame_srcs ${PROJECT_SOURCE_DIR}/test/framework/unittest.c)
set(link_dir ${PROJECT_SOURCE_DIR}/output/src/core/)
link_directories(${link_dir})

# rbtree.c 测试模块
add_executable(rbtree_test rbtree_test.c ${frame_srcs})
target_link_libraries(rbtree_test PRIVATE pthread toy_core)

# t_list.c 测试模块
add_executable(list_test list_test.c ${frame_srcs})
target_link_libraries(list_test PRIVATE pthread toy_core)

# dict_test 测试模块
add_executable(dict_test list_test.c ${frame_srcs})
target_link_libraries(dict_test PRIVATE pthread toy_core)

install(TARGETS list_test
                rbtree_test
                dict_test
        DESTINATION test_bin)
