#! /bin/bash

project_path=$(pwd)
source $project_path/build/build_src.sh

while getopts 'ic' opt;
do
    case $opt in
        i)
            increment=1;;
        c)
            clean_flag=1;;
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

build $project_path
