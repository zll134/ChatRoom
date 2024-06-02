#! /bin/bash

project_path=$(pwd)
source $project_path/build/build_src.sh
source $project_path/build/build_test.sh
source $project_path/build/run_test.sh
source $project_path/build/build_deps.sh

increment="false"
clean_flag=""
build_test=""
run_test=""
test_module=""
debug_enable="false"
deps_build_enable="false"

function usage()
{
    echo "Script usage:"
    echo "  Build all:           ./build.sh"
    echo "  Build increment:     ./build.sh -i"
    echo "  Build and run test:  ./build.sh -i -t -r [ -m module ] [-d]"
    echo "  Build deps:          ./build.sh -p"
    echo "Options:"
    echo " -i      Incremental build."
    echo " -c      Clean build result."
    echo " -t      Build test file"
    echo " -r      Run test case"
    echo " -d      Enable debug info"
    echo " -p      Build Deps pakage"
    echo " -h      Print help."
}

while getopts 'icthrm:dp' opt;
do
    case $opt in
        i)
            increment="true";; # 表示增量编译
        c)
            clean_flag="true";; # 表示清理之前的编译结果
        t)
            build_test="true";; # 表示编译测试的二进制模块
        r)
            run_test="true";; # 表示编译测试的二进制模块
        d)
            debug_enable="true";; # 表示编译测试的二进制模块
        m)
            test_module="$OPTARG";;
        p)
            deps_build_enable="true";;
        h)
            usage
            exit 0;;
        ?)
            ;;   
    esac
done

echo $deps_build_enable
# 编译deps
if [ "$deps_build_enable"x == "true"x ]; then
    build_deps $project_path
    exit 0
fi

if [ "$clean_flag"x == "true"x ]; then
    clean $project_path
    exit 0
fi

# 非增量编译需要删除output目录
if [ "$increment"x == "false"x ];then
    [ -d $project_path/output ] && rm -rf $project_path/output
fi

if [ "$build_test"x == "true"x ]; then
    build_test  $project_path $debug_enable
else
    build $project_path $debug_enable
    install_bin $project_path
fi

# 运行测试用例
if [ "$run_test"x == "true"x ];then
    run_test $project_path $test_module
fi

