* 系统选择 ubuntu 22.04 或者 ubuntu 24.04 都可以
* 本项目要求云服务器配置, 太贵, 自己安装虚拟机(使用 VMWare + 镜像安装, Xshell + VScode 配环境、写项目)
* 如果使用 github 网络不方便请自行搜索加速器并下载安装(包括但不限于 Watt Toolkit  &&  dev-sidecar) / 登录 github 网页下载zip如何使用 lrzsz 工具通过 Xshell 传到虚拟机上解压安装

请自行ai + 文档 + 搜索引擎了解: 
1. 如何创建普通用户并设置密码(密码请设置的复杂一些, 虚拟机问题不大但是之后用云服务器密码太简单被破解服务器容易被拿走挖矿)并提权(赋予root权限)
2. 更换 apt源 (可以参考: https://mirrors.tuna.tsinghua.edu.cn/help/ubuntu/)
3. 安装 tree make cmake lrzsz 等基础工具
4. 了解最简单的命令, 包括但不限于: cd ls ll mkdir touch rm(及其参数) cp mv cat head grep find make gcc g++ gdb sudo su apt vim(及其简单操作) zip unzip tar(及其参数) git(及其简单操作)
5. 请看上级目录下各个基础组件的 `install.md` 文件并安装各个基础组件并完成 mysql 普通用户创建更改密码(root用户 & 新用户)的操作
6. 请看上级目录下 `cmake-use` 目录下的 cmake 使用教程, 看 `cmake-study`
7. 请看上级目录下各个目录下各个基础组件的使用示例
8. 请看上级目录下 `cmake-use` 目录下的 cmake 使用教程, 看 `odb-cmake`
9. 请自行了解 json & protobuf 语法及使用(及编译 -- 仅 protobuf)

** 访问github **
> 1. 自行科学上网(如Watt Toolkit  &&  dev-sidecar)
> 2. 下载 zip 包(有时候不行)
> 3. 使用 ssh+密钥 方式下载(有时候还是慢)
> 4. 使用 gitee 的 使用url克隆github仓库操作, 然后从gitee克隆