# 依赖库

* 编译后使用 ldd 命令查看所链接的库文件并复制到各目录下, 进行打包

```bash
# 获取库文件名 -- print: 输出, $n: 第n列, 以制表符/空格等作为分割
ldd ${executable_file_name} | awk '{print $3}'
# or 更严格匹配
ldd ${executable_file_name} | awk '{if (match($3, "/")) {print $3}}'

# 完整命令 -- match 进行匹配, 去除可能存在的空行等问题
deplist=$(ldd ${executable_file_name} | awk '{if (match($3, "/")) {print $3}}')
# L: 如果为软链接, 则找到实际文件进行复制 r: 递归复制
cp -Lr $deplist $2

# 直接运行 depends.sh 脚本即可(不能使用最外层 CMakeLists.txt, 必须在每个目录下单独编译)

```

# 依赖基础组件

> etcd -- 服务注册发现
> mysql 
> redis
> elasticsearch
> rabbitmq

* 要启动的子服务
  
> 文件子服务
> 语音识别子服务
> 消息存储子服务
> 消息转发子服务
> 好友子服务
> 用户子服务
> 网关子服务

# 使用指南

```bash
cd /StellarPost/project/server/project

cd crony
mkdir build && cd build
cmake ..
make

cd ../gateway
mkdir build && cd build
cmake ..
make

cd ../user
mkdir build && cd build
cmake ..
make

cd ../file
mkdir build && cd build
cmake ..
make

cd ../speech
mkdir build && cd build
cmake ..
make

cd ../transmite
mkdir build && cd build
cmake ..
make

cd ../message
mkdir build && cd build
cmake ..
make

cd ..
docker-compose up -d .

```