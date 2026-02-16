# dms 安装教程

```bash
sudo apt-get install ca-certificates curl gnupg lsb-release

sudo mkdir -p /etc/docker

sudo tee /etc/docker/daemon.json <<-'EOF'
{
"registry-mirrors": [
"https://do.nark.eu.org",
"https://dc.j8.work",
"https://docker.m.daocloud.io",
"https://dockerproxy.com",
"https://docker.mirrors.ustc.edu.cn",
"https://docker.nju.edu.cn"
]
}
EOF

sudo systemctl daemon-reload

curl -fsSL http://mirrors.aliyun.com/docker-ce/linux/ubuntu/gpg | sudo apt-key add -

sudo add-apt-repository "deb [arch=amd64] http://mirrors.aliyun.com/docker-ce/linux/ubuntu $(lsb_release -cs) stable"

sudo apt-get install docker-ce docker-ce-cli containerd.io

# -------------------------------------------------------------------------------------------------
# apt
sudo apt install docker-compose-plugin
# 下载
# sudo curl -L "https://github.com/docker/compose/releases/download/v2.13.0/docker-compose-linux-x86_64" -o /usr/bin/docker-compose
# -------------------------------------------------------------------------------------------------

sudo chmod +x /usr/bin/docker-compose
# 验证
docker-compose --version

sudo groupadd docker

sudo gpasswd -a $USER docker

newgrp docker

```