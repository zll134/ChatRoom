#! /bin/bash

project_path=$(pwd)
source $project_path/build/build_src.sh
source $project_path/build/build_test.sh

while getopts 'ict' opt;
do
    case $opt in
        i)
            increment=1;; # 表示增量编译
        c)
            clean_flag=1;; # 表示清理之前的编译结果
        t)
            build_test=1;; # 表示编译测试的二进制模块
        ?)
            ;;   
    esac
done

if [ ! -z $clean_flag ]; then
    clean $project_path
    exit 0
fi

if [ -z $increment ];then
    [ -d $project_path/output ] && rm -rf $project_path/output
fi

if [ ! -z $build_test ]; then
    build_test  $project_path
else
    build $project_path
fi
