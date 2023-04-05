#! /bin/bash

project_path=$(pwd)
source $project_path/build/build_src.sh
source $project_path/build/build_test.sh

increment=""
clean_flag=""
build_test=""

function usage()
{
    echo "Script build.sh's usage:"
    echo "  ./build.sh [option]"
    echo "Options:"
    echo " -i      Incremental build."
    echo " -c      Clean build result."
    echo " -t      Build test file"
    echo " -h      Print help."
}

while getopts 'icth' opt;
do
    case $opt in
        i)
            increment="true";; # 表示增量编译
        c)
            clean_flag="true";; # 表示清理之前的编译结果
        t)
            build_test="true";; # 表示编译测试的二进制模块
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
    install $project_path
fi
