#! /bin/bash

function build()
{
    project_path=$1

    build_dir=$project_path/output
    [ ! -d $build_dir ] && mkdir $build_dir

    cd $build_dir
    cmake -DCMAKE_INSTALL_PREFIX=$build_dir .. 
    make -j 8
    make install
}

function clean()
{
    project_path=$1
    output_path=$project_path/output
    [ -d $output_path ] && rm -rf $output_path
}