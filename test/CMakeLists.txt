INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)

set(rbtree_srcs ${PROJECT_SOURCE_DIR}/src/rbtree.c /
                ${PROJECT_SOURCE_DIR}/src/log.c)

add_executable(rbtree_test ${rbtree_srcs} rbtree_test.c)

install(TARGETS rbtree_test
        DESTINATION test)