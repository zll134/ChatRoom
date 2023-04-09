#! /bin/bash

project_path=$(pwd)
source $project_path/build/build_src.sh
source $project_path/build/build_test.sh
source $project_path/build/run_test.sh

increment=""
clean_flag=""
build_test=""
run_test=""

function usage()
{
    echo "Script build.sh's usage:"
    echo "  ./build.sh [option]"
    echo "Options:"
    echo " -i      Incremental build."
    echo " -c      Clean build result."
    echo " -t      Build test file"
    echo " -r      Run all test"
    echo " -h      Print help."
}

while getopts 'icthr' opt;
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
        h)
            usage
            exit 0;;
        ?)
            ;;   
    esac
done

if [ "$clean_flag"x == "true"x ]; then
    clean $project_path
    exit 0
fi

if [ "$increment"x == "true"x ];then
    [ -d $project_path/output ] && rm -rf $project_path/output
fi

if [ "$build_test"x == "true"x ]; then
    build_test  $project_path
else
    build $project_path
    install_bin $project_path
fi

# 运行测试用例
if [ "$run_test"x == "true"x ];then
    run_test $project_path
fi

