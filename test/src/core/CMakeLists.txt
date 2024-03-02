INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/core
                    ${PROJECT_SOURCE_DIR}/src/core/base
                    ${PROJECT_SOURCE_DIR}/src/core/test_frame
                    ${PROJECT_SOURCE_DIR}/src/core/test_frame/include
                    ${PROJECT_SOURCE_DIR}/src/core/compress
                    ${PROJECT_SOURCE_DIR}/test/test_data)

set(link_dir ${PROJECT_SOURCE_DIR}/output/src/core/)

link_directories(${link_dir})

# rbtree.c 测试模块
add_executable(rbtree_test rbtree_test.c)
target_link_libraries(rbtree_test PRIVATE pthread toy_core)

# t_list.c 测试模块
add_executable(list_test list_test.c)
target_link_libraries(list_test PRIVATE pthread toy_core)

add_subdirectory(base)
add_subdirectory(compress)

install(TARGETS list_test
                rbtree_test
        DESTINATION test_bin)
