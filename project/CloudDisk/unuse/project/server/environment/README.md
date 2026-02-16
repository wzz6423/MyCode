# 为了简化项目环境配置难度, 故采用 docker 镜像进行配置, 按照本说明书执行即可

```bash
sudo apt update

sudo apt install ca-certificates curl gnupg lsb-release make gcc g++ libz-dev lrzsz vim software-properties-common

sudo mkdir -p /etc/docker

sudo tee /etc/docker/daemon.json <<-'EOF'
{
  "registry-mirrors": [
    "https://docker.m.daocloud.io",
    "https://docker.mirrors.ustc.edu.cn/",
    "http://hub-mirror.c.163.com",
    "https://mirror.ccs.tencentyun.com",
    "https://docker.m.daocloud.io",
    "https://docker.nju.edu.cn",
    "https://hub.alauda.cn",
    "https://docker.1ms.run"
  ]
}
EOF

sudo install -m 0755 -d /etc/apt/keyrings

curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg

sudo chmod a+r /etc/apt/keyrings/docker.gpg

echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu \
  $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

sudo apt update

sudo apt install docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin

sudo curl -L "https://github.com/docker/compose/releases/download/v2.20.3/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose

sudo chmod +x /usr/local/bin/docker-compose

sudo groupadd docker

sudo usermod -aG docker $USER

newgrp docker

sudo systemctl daemon-reload

sudo systemctl restart docker

cp -r ./cloud_disk ~/

cd ~/cloud_disk

sudo docker-compose up --build -d
```

---
* 删除曾经下载的镜像并拉取最新的(更新环境)
```bash
# 停止所有容器
docker stop $(docker ps -aq)

# 删除所有容器
docker rm $(docker ps -aq)

# 删除所有镜像（-f 强制删除，即使有依赖关系）
docker rmi -f $(docker images -aq)

sudo rm -rf ~/cloud_disk

cp -r ./cloud_disk ~/

cd ~/cloud_disk

sudo docker-compose up --build -d
```

* 容器成功运行之后通过 `2222` 端口进入容器克隆代码即可
> 注意: 容器内进行的操作是无法保存的, 除非进行了目录映射(例如: 将容器内的 `/workspace` 目录映射到宿主机的 `/workspace` 目录), 或者在关闭容器前进行 `commit` 操作(将容器内的修改保存到镜像中)(具体操作自行查阅)

--- 
* 注意: 如果使用 vscode 连接 docker 写代码时发生无法保存的问题:
确定 VS Code 连接容器时的用户 ID
在 VS Code 的容器终端中执行：
```bash
id -u  # 输出用户 ID，比如 1000
id -g  # 输出用户组 ID，比如 1000
```
修改容器内文件的所有者为该用户
在容器终端中执行（替换 1000:1000 为上一步得到的 ID）：
```bash
# 假设项目目录是 /workspace，递归修改所有者
sudo chown -R 1000:1000 /workspace
```

--- 
* 注意: 
> 本目录下的 `cloud_disk` 提供了预打包好的 `docker` 镜像, 可以直接使用(结项之后可能会删除)
> 里面使用的 `gitee` 仓库链接可能之后也会失效, 自行替换为新的 `github/gitee` 链接即可
> 本目录下的 `environment` 提供了原生的自己构建 `docker` 镜像的完整流程(如有需要自行修改)
> 由于版本会变化故需使用其他版本的组件也需自行修改 `gitee` 链接
> 由于 `gcc15.2.0` 压缩包体积过大故未放入仓库中, `environment` 中使用了 `wget` 方法下载, 也可以自行下载后放入使用 `COPY` 命令, 解除注释并注释掉 `wget` 相关即可