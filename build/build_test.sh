#! /bin/bash

function build_test()
{
    project_path=$1

    build_dir=$project_path/output
    [ ! -d $build_dir ] && mkdir $build_dir

    cd $build_dir
    cmake -DCMAKE_INSTALL_PREFIX=$build_dir .. -DBUILD_TEST=1
    make -j 8
    make install
}
