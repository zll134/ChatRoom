#! /bin/bash

function build_test()
{
    project_path=$1
    debug_enable=$2

    build_dir=$project_path/output
    # 编译目录不存在，则需要创建目录
    [ ! -d $build_dir ] && mkdir $build_dir

    cd $build_dir

    # 调试模式会开启调试日志
    if [ $debug_enable == "true" ]; then
        cmake -DCMAKE_INSTALL_PREFIX=$build_dir .. -DBUILD_TEST=1 -DDEBUG_MODE=1
    else
        cmake -DCMAKE_INSTALL_PREFIX=$build_dir .. -DBUILD_TEST=1
    fi

    make -j 8
    make install
}
