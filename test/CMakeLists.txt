INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/core)

# rbtree.c 测试模块
set(rbtree_srcs ${PROJECT_SOURCE_DIR}/src/core/rbtree.c /
                ${PROJECT_SOURCE_DIR}/src/core/log.c)

add_executable(rbtree_test ${rbtree_srcs} rbtree_test.c)
install(TARGETS rbtree_test
        DESTINATION test)


# list.c 测试模块
set(list_srcs ${PROJECT_SOURCE_DIR}/src/core/list.c /
              ${PROJECT_SOURCE_DIR}/src/core/log.c)
add_executable(list_test ${list_srcs} list_test.c)
install(TARGETS rbtree_test
        DESTINATION test)