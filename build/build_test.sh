#! /bin/bash

function build_test()
{
    project_path=$1

    build_dir=$project_path/output
    # 编译目录不存在，则需要创建目录
    [ ! -d $build_dir ] && mkdir $build_dir

    cd $build_dir
    cmake -DCMAKE_INSTALL_PREFIX=$build_dir .. -DBUILD_TEST=1
    make -j 8
    make install
}
