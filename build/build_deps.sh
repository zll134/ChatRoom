#! /bin/bash

function build_deps()
{
    local project_path=$1

    cd ${project_path}/deps/nginx-release-1.26.0/
    ./auto/configure --sbin-path=/usr/local/nginx/nginx  --conf-path=/usr/local/nginx/nginx.conf \
                     --pid-path=/usr/local/nginx/nginx.pid \
                     --with-http_ssl_module 
    make -j 8
    make install
    cp ${project_path}/src/web/conf/nginx.conf /usr/local/nginx/ -f
}