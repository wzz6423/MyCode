#!/usr/bin/env bash
#
# CloudDisk 一键环境初始化脚本
# 自动检测并安装缺失依赖，版本满足要求则跳过
#
# 用法: bash setup.sh [选项]
#   --skip-docker    跳过 Docker 容器启动
#   --skip-web       跳过前端构建
#   --skip-backend   跳过后端依赖下载
#   --pull           强制重新拉取 Docker 镜像
#   --down           停止并移除所有容器
#   --help           显示帮助
#

set -euo pipefail

# ============ 颜色 ============
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

info()  { echo -e "${CYAN}[INFO]${NC}  $*"; }
ok()    { echo -e "${GREEN}[ OK ]${NC}  $*"; }
warn()  { echo -e "${YELLOW}[WARN]${NC}  $*"; }
fail()  { echo -e "${RED}[FAIL]${NC}  $*"; exit 1; }

# ============ 版本比较 ============
# 返回 0 表示 $1 >= $2
ver_gte() {
  [ "$(printf '%s\n' "$1" "$2" | sort -V | head -1)" = "$2" ]
}

# 从字符串中提取第一个 x.y.z 或 x.y 版本号
extract_ver() {
  echo "$1" | grep -oE '[0-9]+\.[0-9]+(\.[0-9]+)?' | head -1
}

# ============ 参数解析 ============
SKIP_DOCKER=false
SKIP_WEB=false
SKIP_BACKEND=false
FORCE_PULL=false
ACTION_DOWN=false

for arg in "$@"; do
  case "$arg" in
    --skip-docker)  SKIP_DOCKER=true ;;
    --skip-web)     SKIP_WEB=true ;;
    --skip-backend) SKIP_BACKEND=true ;;
    --pull)         FORCE_PULL=true ;;
    --down)         ACTION_DOWN=true ;;
    --help|-h)
      head -14 "$0" | tail -12
      exit 0
      ;;
    *) warn "未知参数: $arg" ;;
  esac
done

# ============ 项目根目录 ============
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"
info "项目根目录: $SCRIPT_DIR"

# ============ 系统检测 ============
OS=$(uname -s | tr '[:upper:]' '[:lower:]')
ARCH=$(uname -m)
case "$ARCH" in
  x86_64)  GOARCH="amd64" ;;
  aarch64) GOARCH="arm64" ;;
  armv7l)  GOARCH="armv6l" ;;
  *)       GOARCH="$ARCH" ;;
esac

# 检测包管理器
HAS_APT=false
HAS_YUM=false
if command -v apt-get &>/dev/null; then HAS_APT=true; fi
if command -v yum &>/dev/null; then HAS_YUM=true; fi

# ============ --down: 停止容器 ============
if $ACTION_DOWN; then
  info "停止并移除所有 Docker 容器..."
  docker compose down -v 2>/dev/null || docker-compose down -v 2>/dev/null || true
  ok "容器已停止"
  exit 0
fi

# ============ 最低版本要求 ============
REQUIRED_GO="1.23"
REQUIRED_NODE="18.0"

# ============================================================
#  环境检测 & 自动安装
# ============================================================
info "检测运行环境..."
echo ""

# ---------- Docker ----------
if ! $SKIP_DOCKER; then
  if command -v docker &>/dev/null; then
    DOCKER_VER=$(extract_ver "$(docker --version 2>&1)")
    ok "Docker 已安装: $DOCKER_VER（跳过安装）"
  else
    info "未检测到 Docker，开始安装..."
    if [ "$OS" = "linux" ]; then
      curl -fsSL https://get.docker.com | sudo sh
      sudo systemctl enable --now docker
      sudo usermod -aG docker "$USER"
      ok "Docker 安装完成（可能需要重新登录以使用 docker 免 sudo）"
    else
      fail "请手动安装 Docker: https://docs.docker.com/get-docker/"
    fi
  fi

  # Docker Compose
  if docker compose version &>/dev/null; then
    COMPOSE="docker compose"
    ok "Docker Compose: $(extract_ver "$($COMPOSE version 2>&1)")"
  elif command -v docker-compose &>/dev/null; then
    COMPOSE="docker-compose"
    ok "Docker Compose (v1): $(extract_ver "$($COMPOSE version 2>&1)")"
  else
    fail "未检测到 docker compose，请升级 Docker 或安装 docker-compose"
  fi
  echo ""
fi

# ---------- Go ----------
if ! $SKIP_BACKEND; then
  NEED_GO_INSTALL=false

  if command -v go &>/dev/null; then
    GO_VER=$(extract_ver "$(go version 2>&1)")
    if ver_gte "$GO_VER" "$REQUIRED_GO"; then
      ok "Go 已安装: $GO_VER >= $REQUIRED_GO（跳过安装）"
    else
      warn "Go 版本 $GO_VER < $REQUIRED_GO，需要升级"
      NEED_GO_INSTALL=true
    fi
  else
    info "未检测到 Go"
    NEED_GO_INSTALL=true
  fi

  if $NEED_GO_INSTALL; then
    info "正在获取最新 Go 版本..."
    # 从中国镜像获取最新版本号
    GO_LATEST=$(curl -fsSL "https://golang.google.cn/dl/?mode=json" 2>/dev/null \
      | grep -oE '"version":"go[0-9]+\.[0-9]+(\.[0-9]+)?"' \
      | head -1 \
      | grep -oE 'go[0-9]+\.[0-9]+(\.[0-9]+)?')

    if [ -z "$GO_LATEST" ]; then
      # 回退：手动指定
      GO_LATEST="go1.24.1"
      warn "无法自动获取最新版本，使用 $GO_LATEST"
    fi

    GO_TARBALL="${GO_LATEST}.${OS}-${GOARCH}.tar.gz"
    GO_URL="https://golang.google.cn/dl/${GO_TARBALL}"

    info "下载 $GO_LATEST ($OS/$GOARCH)..."
    curl -fSL -o "/tmp/$GO_TARBALL" "$GO_URL" \
      || wget -q -O "/tmp/$GO_TARBALL" "$GO_URL" \
      || fail "Go 下载失败，请手动下载: $GO_URL"

    info "安装 Go 到 /usr/local/go..."
    sudo rm -rf /usr/local/go
    sudo tar -C /usr/local -xzf "/tmp/$GO_TARBALL"
    rm -f "/tmp/$GO_TARBALL"

    # 确保 PATH 包含 go
    export PATH="/usr/local/go/bin:$PATH"
    if ! grep -q '/usr/local/go/bin' ~/.bashrc 2>/dev/null; then
      echo 'export PATH=/usr/local/go/bin:$PATH' >> ~/.bashrc
    fi
    if [ -f ~/.zshrc ] && ! grep -q '/usr/local/go/bin' ~/.zshrc 2>/dev/null; then
      echo 'export PATH=/usr/local/go/bin:$PATH' >> ~/.zshrc
    fi

    # 清除旧的 go 残留
    for old_go in /usr/bin/go /usr/lib/go; do
      if [ -e "$old_go" ] && [ "$(readlink -f "$old_go" 2>/dev/null)" != "/usr/local/go/bin/go" ]; then
        sudo rm -rf "$old_go"
      fi
    done
    hash -r 2>/dev/null || true

    GO_VER=$(extract_ver "$(go version 2>&1)")
    ok "Go 安装完成: $GO_VER"
  fi

  # 配置 Go 国内代理
  go env -w GOPROXY=https://goproxy.cn,direct 2>/dev/null || true
  echo ""
fi

# ---------- Node.js ----------
if ! $SKIP_WEB; then
  NEED_NODE_INSTALL=false

  if command -v node &>/dev/null; then
    NODE_VER=$(extract_ver "$(node --version 2>&1)")
    if ver_gte "$NODE_VER" "$REQUIRED_NODE"; then
      ok "Node.js 已安装: $NODE_VER >= $REQUIRED_NODE（跳过安装）"
    else
      warn "Node.js 版本 $NODE_VER < $REQUIRED_NODE，需要升级"
      NEED_NODE_INSTALL=true
    fi
  else
    info "未检测到 Node.js"
    NEED_NODE_INSTALL=true
  fi

  if $NEED_NODE_INSTALL; then
    info "安装最新 LTS Node.js..."
    if [ "$OS" = "linux" ]; then
      # 使用 NodeSource 安装最新 LTS
      if $HAS_APT; then
        curl -fsSL https://deb.nodesource.com/setup_lts.x | sudo -E bash - 2>/dev/null \
          || curl -fsSL https://mirrors.tuna.tsinghua.edu.cn/nodesource/setup_lts.x | sudo -E bash - 2>/dev/null \
          || true
        sudo apt-get install -y nodejs
      elif $HAS_YUM; then
        curl -fsSL https://rpm.nodesource.com/setup_lts.x | sudo bash - 2>/dev/null || true
        sudo yum install -y nodejs
      else
        fail "不支持的包管理器，请手动安装 Node.js: https://nodejs.org/"
      fi
      NODE_VER=$(extract_ver "$(node --version 2>&1)")
      ok "Node.js 安装完成: $NODE_VER"
    else
      fail "请手动安装 Node.js: https://nodejs.org/"
    fi
  fi

  # npm 国内镜像
  if command -v npm &>/dev/null; then
    npm config set registry https://registry.npmmirror.com 2>/dev/null || true
  fi

  # pnpm
  if command -v pnpm &>/dev/null; then
    PNPM_VER=$(extract_ver "$(pnpm --version 2>&1)")
    ok "pnpm 已安装: $PNPM_VER（跳过安装）"
  else
    info "安装 pnpm..."
    sudo npm install -g pnpm
    PNPM_VER=$(extract_ver "$(pnpm --version 2>&1)")
    ok "pnpm 安装完成: $PNPM_VER"
  fi
  pnpm config set registry https://registry.npmmirror.com 2>/dev/null || true

  PKG_MGR="pnpm"
  echo ""
fi

# ============================================================
#  Docker 镜像 & 容器
# ============================================================
if ! $SKIP_DOCKER; then
  info "========== Docker 环境 =========="

  IMAGES=(
    "mysql:8.0.40"
    "redis:7.4.1"
    "elasticsearch:7.17.18"
    "rabbitmq:4.0-management"
  )

  # 拉取镜像
  for img in "${IMAGES[@]}"; do
    if $FORCE_PULL || ! docker image inspect "$img" &>/dev/null; then
      info "拉取镜像: $img"
      docker pull "$img"
      ok "$img 拉取完成"
    else
      ok "$img 已存在（使用 --pull 强制更新）"
    fi
  done

  echo ""

  # 启动容器
  info "启动 Docker 容器..."
  $COMPOSE up -d
  ok "容器已启动"

  echo ""

  # 等待 MySQL 就绪
  info "等待 MySQL 就绪..."
  MAX_WAIT=60
  WAITED=0
  until docker exec clouddisk-mysql mysqladmin ping -h127.0.0.1 -uroot -p123456 --silent 2>/dev/null; do
    WAITED=$((WAITED + 2))
    if [ $WAITED -ge $MAX_WAIT ]; then
      fail "MySQL 启动超时（${MAX_WAIT}s），请检查 docker logs clouddisk-mysql"
    fi
    printf "."
    sleep 2
  done
  echo ""
  ok "MySQL 已就绪"

  # 等待 Redis 就绪
  info "等待 Redis 就绪..."
  WAITED=0
  until docker exec clouddisk-redis redis-cli ping 2>/dev/null | grep -q PONG; do
    WAITED=$((WAITED + 2))
    if [ $WAITED -ge $MAX_WAIT ]; then
      fail "Redis 启动超时（${MAX_WAIT}s），请检查 docker logs clouddisk-redis"
    fi
    printf "."
    sleep 2
  done
  echo ""
  ok "Redis 已就绪"

  # 等待 Elasticsearch 就绪
  info "等待 Elasticsearch 就绪..."
  WAITED=0
  until curl -s -u elastic:123456 http://127.0.0.1:9200/_cluster/health 2>/dev/null | grep -qE '"status":"(green|yellow)"'; do
    WAITED=$((WAITED + 3))
    if [ $WAITED -ge 90 ]; then
      warn "Elasticsearch 启动超时（90s），可稍后手动检查: curl -u elastic:123456 http://127.0.0.1:9200"
      break
    fi
    printf "."
    sleep 3
  done
  echo ""
  ok "Elasticsearch 已就绪"

  # 等待 RabbitMQ 就绪
  info "等待 RabbitMQ 就绪..."
  WAITED=0
  until curl -s -u admin:123456 http://127.0.0.1:15672/api/overview &>/dev/null; do
    WAITED=$((WAITED + 3))
    if [ $WAITED -ge 60 ]; then
      warn "RabbitMQ Management 启动超时，可稍后访问: http://127.0.0.1:15672"
      break
    fi
    printf "."
    sleep 3
  done
  echo ""
  ok "RabbitMQ 已就绪"

  # 验证数据库初始化
  info "验证数据库..."
  if docker exec clouddisk-mysql mysql -uroot -p123456 -e "USE cloud_disk; SHOW TABLES;" 2>/dev/null | grep -q "users"; then
    ok "数据库 cloud_disk 已初始化（users, files, shares）"
  else
    warn "数据库表可能尚未创建，MySQL 可能仍在执行初始化脚本，请稍等后重试"
  fi

  echo ""
fi

# ============ 后端依赖 ============
if ! $SKIP_BACKEND; then
  info "========== 后端环境 =========="

  info "下载 Go 依赖..."
  go mod download
  ok "Go 依赖下载完成"

  # 创建上传目录
  mkdir -p uploads
  ok "uploads 目录已就绪"

  echo ""
fi

# ============ 前端依赖 & 构建 ============
if ! $SKIP_WEB; then
  info "========== 前端环境 =========="

  info "安装前端依赖..."
  cd web
  $PKG_MGR install
  ok "前端依赖安装完成"

  info "构建前端..."
  $PKG_MGR run build
  ok "前端构建完成 → web/dist/"

  cd "$SCRIPT_DIR"
  echo ""
fi

# ============ 完成 ============
echo ""
echo -e "${GREEN}============================================${NC}"
echo -e "${GREEN}  CloudDisk 环境初始化完成！${NC}"
echo -e "${GREEN}============================================${NC}"
echo ""
echo "  已安装环境:"
command -v go &>/dev/null && echo "    Go          $(extract_ver "$(go version 2>&1)")"
command -v node &>/dev/null && echo "    Node.js     $(extract_ver "$(node --version 2>&1)")"
command -v pnpm &>/dev/null && echo "    pnpm        $(extract_ver "$(pnpm --version 2>&1)")"
command -v docker &>/dev/null && echo "    Docker      $(extract_ver "$(docker --version 2>&1)")"
echo ""
echo "  服务端口:"
echo "    后端 API        http://127.0.0.1:8080"
echo "    MySQL           127.0.0.1:3306  (root / 123456)"
echo "    Redis           127.0.0.1:6379"
echo "    Elasticsearch   http://127.0.0.1:9200  (elastic / 123456)"
echo "    RabbitMQ        http://127.0.0.1:15672  (admin / 123456)"
echo ""
echo "  启动方式:"
echo "    后端:   make run  或  go run ./cmd/server"
echo "    前端:   cd web && pnpm dev"
echo "    全量构建: make all"
echo ""
echo "  其他命令:"
echo "    停止容器: bash setup.sh --down"
echo "    重新拉取: bash setup.sh --pull"
echo ""
