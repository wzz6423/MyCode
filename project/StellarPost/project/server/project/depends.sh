#!/bin/bash

# 传递两个参数
# 1.可执行程序名
# 2.将该程序的依赖库拷贝到指定目录下
declare deplist
get_depends(){
    deplist=$(ldd $1 | awk '{if (match($3, "/")) {print $3}}')
    mkdir -p $2
    cp -Lr $deplist $2
}

# $1: executable_file_name 可执行程序名
# $2: depends_dir 依赖库存放目录

get_depends ./gateway/build/gateway_server ./gateway/depends
get_depends ./file/build/file_server ./file/depends
get_depends ./crony/build/crony_server ./crony/depends
get_depends ./message/build/message_server ./message/depends
get_depends ./speech/build/speech_server ./speech/depends
get_depends ./transmite/build/transmite_server ./transmite/depends
get_depends ./user/build/user_server ./user/depends
cp -Lr /usr/lib/x86_64-linux-gnu/libcurl.so ./user/depends

cp /bin/nc ./gateway/build
cp /bin/nc ./file/build
cp /bin/nc ./crony/build
cp /bin/nc ./message/build
cp /bin/nc ./speech/build
cp /bin/nc ./transmite/build
cp /bin/nc ./user/build

get_depends /bin/nc ./gateway/depends
get_depends /bin/nc ./file/depends
get_depends /bin/nc ./crony/depends
get_depends /bin/nc ./message/depends
get_depends /bin/nc ./speech/depends
get_depends /bin/nc ./transmite/depends
get_depends /bin/nc ./user/depends
