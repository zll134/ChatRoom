#! /bin/bash
function run_test()
{
    echo "start run test case"
    project_path=$1
    test_module=$2

    if [ "$test_module"x == ""x ]; then
        for file in $(ls ${project_path}/output/test_bin); do
            ${project_path}/output/test_bin/$file
        done
    else
        ${project_path}/output/test_bin/$test_module
    fi
}
