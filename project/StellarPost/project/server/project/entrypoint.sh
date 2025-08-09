#!/bin/bash

# 端口探测函数, 连接成功则通过, 不成功则循环等待
wait_for(){
    while !nc -z $1 $2
    do
        sleep 1;
    done
}
# $1: ip
# $2: port

# 对脚本运行参数进行解析, 获取到 ip, port, command
declare ip
declare ports
declare command
while getopts "h:p:c:" arg
do
    case $arg in
        h)
            ip=$OPTARG
        ;;
        p)
            ports=$OPTARG
        ;;
        c)
            command=$OPTARG
        ;;
    esac
done

# 通过执行脚本进行端口检测
# {port//,/ } 将 port 字符串中的逗号替换为空格, 并将结果赋值给 port
# shell 中的数组 -- 以空格间隔的字符串
for port in ${ports//,/ }
do
    wait_for $ip $port
done

# 执行 command
# eval: 对字符串进行二次检测并将其当作命令执行
# exec: 类似 C 中的 exec* 函数, 会用命令替换当前 shell 脚本进程
eval $command