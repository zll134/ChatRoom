#! /bin/bash
function run_test()
{
    echo "start run test case"
    project_path=$1

    for file in $(ls ${project_path}/output/test_bin); do
        ${project_path}/output/test_bin/$file
    done
}
