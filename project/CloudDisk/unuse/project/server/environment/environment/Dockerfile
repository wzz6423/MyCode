FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive 
ENV TZ=Asia/Shanghai                
ENV LANG=zh_CN.UTF-8                

# 设置时区
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# 动态获取系统代号并替换 apt 源为清华源
RUN apt-get update && apt-get install -y lsb-release && \
    CODENAME=$(lsb_release -cs) && \
    echo "deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ $CODENAME main restricted universe multiverse" > /etc/apt/sources.list && \
    echo "deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ $CODENAME-updates main restricted universe multiverse" >> /etc/apt/sources.list && \
    echo "deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ $CODENAME-backports main restricted universe multiverse" >> /etc/apt/sources.list && \
    echo "deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ $CODENAME-security main restricted universe multiverse" >> /etc/apt/sources.list && \
    apt-get clean && \
    apt-get update && \
    apt-get upgrade -y

# 安装中文语言包
RUN apt-get install -y language-pack-zh-hans && \
    update-locale LANG=$LANG

# 确保使用bash作为默认shell
RUN ln -sf /bin/bash /bin/sh

# 安装基础依赖
RUN apt update --allow-unauthenticated && \
    apt install -y --no-install-recommends \
    ca-certificates && \
    apt update && \
    apt install -y --no-install-recommends \
    sudo build-essential wget curl git gcc g++ zip cmake make lrzsz vim tree \
    bash unzip net-tools open-vm-tools open-vm-tools-desktop automake autoconf \
    libtool pkg-config libssl-dev zlib1g-dev libbz2-dev libreadline-dev libsqlite3-dev \
    libboost-all-dev libmysqlclient-dev libhiredis-dev redis-server libgrpc++-dev \
    libmpfr-dev libgmp-dev libmpc-dev flex bison texinfo libjson-c-dev libjsoncpp-dev \
    python3 ssh libprotobuf-dev libprotoc-dev protobuf-compiler libleveldb-dev libgrpc-dev \
    libcurl4-openssl-dev uuid-dev gcc-13-plugin-dev coreutils openssh-server protobuf-compiler-grpc \
    libpq-dev libisl-dev libc6-dev libcutl-dev libodb-boost-dev  libcpprest-dev\
    libodb-dev libodb-sqlite-dev odb libboost-system-dev libwebsocketpp-dev libev-dev \
    librabbitmq-dev g++-12 libmicrohttpd-dev && \
    apt upgrade -y --no-install-recommends && \
    apt clean && \
    rm -rf /var/lib/apt/lists/*

# 2. 创建默认用户
# -m：创建用户家目录
# -s：指定默认shell为bash
RUN useradd -m -s /bin/bash user

# 3. 设置用户密码
# 使用chpasswd非交互设置密码(格式：用户名:密码)
RUN echo "user:user" | chpasswd

# sudo 提权
RUN echo "user ALL=(ALL) ALL" > /etc/sudoers.d/user && \
    chmod 0440 /etc/sudoers.d/user

# 创建临时目录并进入
RUN mkdir -p /root/tmp && chmod 755 /root/tmp

# 安装 g++15.2.0 -- 提前下载好文件版
COPY ./downloads/gcc-15.2.0.tar.gz /root/tmp/
RUN cd /root/tmp && \
    tar xzf gcc-15.2.0.tar.gz && \
    cd gcc-15.2.0 && \
    mkdir build && cd build && \
    ../configure --prefix=/usr/local/gcc-15.2.0 --enable-languages=c,c++ --disable-multilib --enable-checking=release --with-system-zlib && \
    make clean && \
    make -j$(nproc) && \
    rm -rf /usr/local/gcc-15.2.0/* && \ 
    mkdir -p /usr/local/gcc-15.2.0 && chmod 755 /usr/local/gcc-15.2.0 && \
    make install && \
    ln -sf /usr/local/gcc-15.2.0/bin/gcc /usr/bin/gcc && \
    ln -sf /usr/local/gcc-15.2.0/bin/g++ /usr/bin/g++ && \
    cd /root/tmp && rm -rf gcc-15.2.0* 

# 安装 CMake 4.1.1 -- 提前下载好文件版
COPY ./downloads/cmake-4.1.1.tar.gz /root/tmp/
RUN cd /root/tmp && \
    tar xzf cmake-4.1.1.tar.gz && \
    cd cmake-4.1.1 && \
    ./bootstrap --prefix=/usr/local && \
    make -j$(nproc) && \
    make install && \
    cd /root/tmp && rm -rf cmake-4.1.1*

# 安装 g++15.2.0 -- 在线下载版
# 清华源: wget https://mirrors.tuna.tsinghua.edu.cn/gnu/gcc/gcc-15.2.0/gcc-15.2.0.tar.gz
# RUN cd /root/tmp && \
#     wget https://ftp.gnu.org/gnu/gcc/gcc-15.2.0/gcc-15.2.0.tar.gz && \
#     tar xzf gcc-15.2.0.tar.gz && \
#     cd gcc-15.2.0 && \
#     mkdir build && cd build && \
#     ../configure --prefix=/usr/local/gcc-15.2.0 --enable-languages=c,c++ --disable-multilib --enable-checking=release --with-system-zlib && \
#     make clean && \
#     make -j$(nproc) && \
#     rm -rf /usr/local/gcc-15.2.0/* && \ 
#     mkdir -p /usr/local/gcc-15.2.0 && chmod 755 /usr/local/gcc-15.2.0 && \
#     make install && \
#     ln -sf /usr/local/gcc-15.2.0/bin/gcc /usr/bin/gcc && \
#     ln -sf /usr/local/gcc-15.2.0/bin/g++ /usr/bin/g++ && \
#     cd /root/tmp && rm -rf gcc-15.2.0* 

# 安装 CMake 4.1.1 -- 在线下载版
# RUN cd /root/tmp && \
#     wget https://github.com/Kitware/CMake/releases/download/v4.1.1/cmake-4.1.1-linux-x86_64.tar.gz && \
#     tar xzf cmake-4.1.1.tar.gz && \
#     cd cmake-4.1.1 && \
#     ./bootstrap --prefix=/usr/local && \
#     make -j$(nproc) && \
#     make install && \
#     cd /root/tmp && rm -rf cmake-4.1.1*

ENV PATH=/usr/local/gcc-15.2.0/bin:/usr/local/bin:$PATH
ENV LD_LIBRARY_PATH=/usr/local/gcc-15.2.0/lib64:/usr/local/lib:$LD_LIBRARY_PATH

# 安装 gflags
RUN git clone https://gitee.com/wzz6423/gflags.git /root/tmp/gflags && \
    cd /root/tmp/gflags && \
    mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc) && \
    make install && \
    cd /root/tmp && rm -rf gflags

# 安装 gtest
RUN git clone https://gitee.com/wzz6423/googletest.git /root/tmp/gtest && \ 
    cd /root/tmp/gtest && \ 
    mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc) && \
    make install && \
    cd /root/tmp && rm -rf gtest 

# 安装 spdlog
RUN git clone https://gitee.com/wzz6423/spdlog.git /root/tmp/spdlog/ && \
    cd /root/tmp/spdlog && \
    mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc) && \
    make install && \
    cd /root/tmp/ && rm -rf spdlog 

# 安装 cpp-httplib
RUN git clone https://gitee.com/wzz6423/cpp-httplib.git  /root/tmp/httplib/ && \
    cd /root/tmp/httplib/ && \
    mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc) && \
    make install && \
    cd /root/tmp && rm -rf httplib

# 安装 redis-plus-plus
RUN git clone https://gitee.com/wzz6423/redis-plus-plus.git  /root/tmp/redis-plus-plus/ && \
    cd /root/tmp/redis-plus-plus/ && \ 
    mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc) && \
    make install && \
    cd /root/tmp && rm -rf redis-plus-plus

# 安装 brpc
RUN git clone https://gitee.com/wzz6423/brpc.git /root/tmp/brpc/ && \
    cd /root/tmp/brpc && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_INSTALL_PREFIX=/usr .. && \
    cmake --build . -j$(nproc) && \
    make install && \
    cd /root/tmp && \
    rm -rf brpc

# 安装 etcd
RUN git clone https://gitee.com/wzz6423/etcd-cpp-apiv3.git /root/tmp/etcd && \
    cd /root/tmp/etcd && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr .. && \
    cmake --build . -j$(nproc) && \
    make install && \
    cd /root/tmp && \
    rm -rf etcd

# 安装 es
RUN git clone https://gitee.com/wzz6423/elasticlient.git /root/tmp/es && \
    cd /root/tmp/es && \
    sed -i 's#https://github.com/whoshuu/cpr#https://gitee.com/wzz6423/cpr.git#g' .gitmodules && \
    sed -i 's#https://github.com/google/googletest#https://gitee.com/wzz6423/googletest.git#g' .gitmodules && \
    sed -i 's#https://github.com/seznam/httpmockserver#https://gitee.com/wzz6423/httpmockserver.git#g' .gitmodules && \
    sed -i 's#https://github.com/open-source-parsers/jsoncpp#https://gitee.com/wzz6423/jsoncpp.git#g' .gitmodules && \
    git submodule sync && \
    git submodule update --init external/cpr external/googletest external/jsoncpp external/httpmockserver && \
    cd external/httpmockserver && \
    sed -i 's#https://github.com/whoshuu/cpr.git#https://gitee.com/wzz6423/cpr.git#g' .gitmodules && \
    sed -i 's#https://github.com/google/googletest.git#https://gitee.com/wzz6423/googletest.git#g' .gitmodules && \
    git submodule sync && \
    git submodule update --init --recursive && \
    cd ../../ && \
    git submodule update --init --recursive external/cpr external/googletest external/jsoncpp && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DUSE_SYSTEM_CURL=ON -DUSE_SYSTEM_MHD=ON .. && \ 
    make -j$(nproc) && \
    make install && \
    cd /root/tmp && \
    rm -rf es

# 安装 amqpcpp
RUN git clone https://gitee.com/wzz6423/AMQP-CPP.git /root/tmp/amqpcpp/ && \
    cd /root/tmp/amqpcpp/ && \
    make -j$(nproc) && \
    make install && \
    cd /root/tmp && rm -rf amqpcpp

# 安装阿里云 DMS SDK
RUN git clone https://gitee.com/wzz6423/aliyun-openapi-cpp-sdk.git /root/tmp/dms/ && \
    cd /root/tmp/dms/ && \
    # 修复 CMake 版本问题
    sed -i '1s/VERSION [0-9.]*/VERSION 3.10/' CMakeLists.txt && \
    # 在 NetworkProxy.h 中添加缺失的头文件包含
    sed -i '1i #include <cstdint>' core/include/alibabacloud/core/NetworkProxy.h && \
    # 确认修改已应用
    head -n 5 core/include/alibabacloud/core/NetworkProxy.h && \
    sh easyinstall.sh core && \
    cd /root/tmp && rm -rf dms

# 在构建阶段完成所有配置
RUN mkdir -p /var/run/sshd && \
    chmod 0755 /var/run/sshd && \
    chown root:root /etc/ssh && \
    chmod 755 /etc/ssh

# 生成SSH主机密钥
RUN rm -f /etc/ssh/ssh_host_* && \
    ssh-keygen -t rsa -b 4096 -f /etc/ssh/ssh_host_rsa_key -N "" && \
    ssh-keygen -t ecdsa -f /etc/ssh/ssh_host_ecdsa_key -N "" && \
    ssh-keygen -t ed25519 -f /etc/ssh/ssh_host_ed25519_key -N "" && \
    chmod 600 /etc/ssh/ssh_host_*_key

# 配置SSHD
RUN echo "PermitRootLogin yes" >> /etc/ssh/sshd_config && \
    echo "PubkeyAuthentication yes" >> /etc/ssh/sshd_config && \
    echo "PasswordAuthentication yes" >> /etc/ssh/sshd_config

# 设置root密码
RUN echo 'root:root' | chpasswd

# 暴露SSH端口
EXPOSE 22

# 清除临时文件
RUN rm -rf /root/tmp

# 环境变量
ENV SSH_PORT=22

# 6. 设置工作目录(默认在用户家目录)
WORKDIR /home/user

# 7. 容器启动命令 -- 启动SSH服务
CMD ["/usr/sbin/sshd", "-D"]