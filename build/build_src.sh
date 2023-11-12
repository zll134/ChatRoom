#! /bin/bash

function build()
{
    project_path=$1
    debug_enable=$2

    build_dir=$project_path/output
    [ ! -d $build_dir ] && mkdir $build_dir

    cd $build_dir

    # 调试模式会开启调试日志
    if [ $debug_enable == "true" ]; then
        cmake -DCMAKE_INSTALL_PREFIX=$build_dir .. -DDEBUG_MODE=1
    else
        cmake -DCMAKE_INSTALL_PREFIX=$build_dir ..
    fi

    make -j 8
    make install
}

function clean()
{
    project_path=$1
    output_path=$project_path/output
    [ -d $output_path ] && rm -rf $output_path
}

# 将二进制文件拷贝到工作目录中
function install_bin()
{
    project_path=$1
    work_dir="$project_path/workspace"

    if [ -d $work_dir ]; then
        rm -rf $work_dir
    fi

    mkdir $work_dir
    cp -f $project_path/output/bin/* $work_dir
}