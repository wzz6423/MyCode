#!/usr/bin/env bash
#
# CloudDisk 编译并启动所有服务
# 用法: bash start.sh [选项]
#   --dev            开发模式（前端启动 dev server 而非构建）
#   --build-only     只编译不启动
#   --skip-docker    跳过 Docker 容器
#   --skip-web       跳过前端
#   --stop           停止所有服务
#   --status         查看服务状态
#   --help           显示帮助
#

set -euo pipefail

# ============ 颜色 ============
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'

info()  { echo -e "${CYAN}[INFO]${NC}  $*"; }
ok()    { echo -e "${GREEN}[ OK ]${NC}  $*"; }
warn()  { echo -e "${YELLOW}[WARN]${NC}  $*"; }
fail()  { echo -e "${RED}[FAIL]${NC}  $*"; exit 1; }

# ============ 项目根目录 ============
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# ============ 常量 ============
APP_NAME="clouddisk-server"
CMD_PATH="./cmd/server"
BIN_DIR="./bin"
WEB_DIR="./web"
PID_DIR="./.pids"
BACKEND_PID="$PID_DIR/backend.pid"
FRONTEND_PID="$PID_DIR/frontend.pid"
LOG_DIR="./logs"
BACKEND_LOG="$LOG_DIR/backend.log"
FRONTEND_LOG="$LOG_DIR/frontend.log"

# ============ 参数解析 ============
DEV_MODE=false
BUILD_ONLY=false
SKIP_DOCKER=false
SKIP_WEB=false
ACTION_STOP=false
ACTION_STATUS=false

for arg in "$@"; do
  case "$arg" in
    --dev)          DEV_MODE=true ;;
    --build-only)   BUILD_ONLY=true ;;
    --skip-docker)  SKIP_DOCKER=true ;;
    --skip-web)     SKIP_WEB=true ;;
    --stop)         ACTION_STOP=true ;;
    --status)       ACTION_STATUS=true ;;
    --help|-h)
      head -13 "$0" | tail -11
      exit 0
      ;;
    *) warn "未知参数: $arg" ;;
  esac
done

# ============ 工具函数 ============
detect_compose() {
  if docker compose version &>/dev/null; then
    echo "docker compose"
  elif command -v docker-compose &>/dev/null; then
    echo "docker-compose"
  else
    echo ""
  fi
}

detect_pkg_mgr() {
  if command -v pnpm &>/dev/null; then
    echo "pnpm"
  elif command -v npm &>/dev/null; then
    echo "npm"
  else
    echo ""
  fi
}

# 检查进程是否存活
is_running() {
  local pidfile="$1"
  if [ -f "$pidfile" ]; then
    local pid
    pid=$(cat "$pidfile")
    if kill -0 "$pid" 2>/dev/null; then
      return 0
    fi
    rm -f "$pidfile"
  fi
  return 1
}

stop_process() {
  local pidfile="$1"
  local name="$2"
  if is_running "$pidfile"; then
    local pid
    pid=$(cat "$pidfile")
    info "停止 $name (PID: $pid)..."
    kill "$pid" 2>/dev/null || true
    # 等待进程退出
    local waited=0
    while kill -0 "$pid" 2>/dev/null && [ $waited -lt 10 ]; do
      sleep 1
      waited=$((waited + 1))
    done
    if kill -0 "$pid" 2>/dev/null; then
      warn "$name 未响应，强制终止..."
      kill -9 "$pid" 2>/dev/null || true
    fi
    rm -f "$pidfile"
    ok "$name 已停止"
  else
    info "$name 未在运行"
  fi
}

# ============ --status: 查看状态 ============
if $ACTION_STATUS; then
  echo -e "${BOLD}CloudDisk 服务状态${NC}"
  echo "─────────────────────────────────────"

  # Docker 容器
  COMPOSE=$(detect_compose)
  if [ -n "$COMPOSE" ]; then
    for svc in clouddisk-mysql clouddisk-redis clouddisk-es clouddisk-rabbitmq; do
      status=$(docker inspect -f '{{.State.Status}}' "$svc" 2>/dev/null || echo "未创建")
      if [ "$status" = "running" ]; then
        echo -e "  ${GREEN}●${NC} $svc  ($status)"
      else
        echo -e "  ${RED}●${NC} $svc  ($status)"
      fi
    done
  else
    echo -e "  ${YELLOW}●${NC} Docker Compose 未安装"
  fi

  echo ""

  # 后端
  if is_running "$BACKEND_PID"; then
    echo -e "  ${GREEN}●${NC} 后端服务  (PID: $(cat "$BACKEND_PID"))  → http://127.0.0.1:8080"
  else
    echo -e "  ${RED}●${NC} 后端服务  (未运行)"
  fi

  # 前端
  if is_running "$FRONTEND_PID"; then
    echo -e "  ${GREEN}●${NC} 前端 Dev  (PID: $(cat "$FRONTEND_PID"))  → http://127.0.0.1:3000"
  else
    echo -e "  ${RED}●${NC} 前端 Dev  (未运行)"
  fi

  echo ""
  exit 0
fi

# ============ --stop: 停止所有 ============
if $ACTION_STOP; then
  echo -e "${BOLD}停止所有 CloudDisk 服务${NC}"
  echo ""

  stop_process "$FRONTEND_PID" "前端 Dev Server"
  stop_process "$BACKEND_PID" "后端服务"

  if ! $SKIP_DOCKER; then
    COMPOSE=$(detect_compose)
    if [ -n "$COMPOSE" ]; then
      info "停止 Docker 容器..."
      $COMPOSE stop
      ok "Docker 容器已停止"
    fi
  fi

  echo ""
  ok "所有服务已停止"
  exit 0
fi

# ============ 准备目录 ============
mkdir -p "$PID_DIR" "$LOG_DIR" "$BIN_DIR" uploads

# ============ 1. Docker 基础服务 ============
if ! $SKIP_DOCKER; then
  echo -e "${BOLD}[1/3] Docker 基础服务${NC}"

  COMPOSE=$(detect_compose)
  [ -z "$COMPOSE" ] && fail "未检测到 docker compose"

  # 启动容器
  info "启动 Docker 容器..."
  $COMPOSE up -d
  ok "容器已启动"

  # 等待 MySQL
  info "等待 MySQL..."
  waited=0
  until docker exec clouddisk-mysql mysqladmin ping -h127.0.0.1 -uroot -p123456 --silent 2>/dev/null; do
    waited=$((waited + 2))
    [ $waited -ge 60 ] && fail "MySQL 启动超时"
    printf "."
    sleep 2
  done
  echo ""
  ok "MySQL 就绪"

  # 等待 Redis
  info "等待 Redis..."
  waited=0
  until docker exec clouddisk-redis redis-cli ping 2>/dev/null | grep -q PONG; do
    waited=$((waited + 2))
    [ $waited -ge 30 ] && fail "Redis 启动超时"
    printf "."
    sleep 2
  done
  echo ""
  ok "Redis 就绪"

  # 等待 ES（非阻塞）
  info "等待 Elasticsearch..."
  waited=0
  until curl -s -u elastic:123456 http://127.0.0.1:9200/_cluster/health 2>/dev/null | grep -qE '"status":"(green|yellow)"'; do
    waited=$((waited + 3))
    if [ $waited -ge 60 ]; then
      warn "Elasticsearch 仍在启动，后端可能需要稍后重连"
      break
    fi
    printf "."
    sleep 3
  done
  echo ""
  ok "Elasticsearch 就绪"

  echo ""
fi

# ============ 2. 编译后端 ============
echo -e "${BOLD}[2/3] 编译后端${NC}"

# 检测平台
GOOS=$(go env GOOS)
GOARCH=$(go env GOARCH)
EXT=""
[ "$GOOS" = "windows" ] && EXT=".exe"
BINARY="$BIN_DIR/${APP_NAME}-${GOOS}-${GOARCH}${EXT}"

info "编译 $APP_NAME ($GOOS/$GOARCH)..."
VERSION=$(git describe --tags --always --dirty 2>/dev/null || echo "dev")
BUILD_TIME=$(date -u '+%Y-%m-%dT%H:%M:%SZ' 2>/dev/null || date '+%Y-%m-%dT%H:%M:%SZ')
LDFLAGS="-s -w -X main.version=$VERSION -X main.buildTime=$BUILD_TIME"

CGO_ENABLED=0 go build -ldflags "$LDFLAGS" -o "$BINARY" "$CMD_PATH"
ok "编译完成 → $BINARY"

echo ""

# ============ 3. 前端 ============
if ! $SKIP_WEB; then
  echo -e "${BOLD}[3/3] 前端${NC}"

  PKG_MGR=$(detect_pkg_mgr)
  [ -z "$PKG_MGR" ] && fail "未检测到 pnpm 或 npm"

  # 确保依赖已安装
  if [ ! -d "$WEB_DIR/node_modules" ]; then
    info "安装前端依赖..."
    (cd "$WEB_DIR" && $PKG_MGR install)
    ok "依赖安装完成"
  fi

  if $DEV_MODE; then
    info "构建前端（生产模式）..."
    (cd "$WEB_DIR" && $PKG_MGR run build)
    ok "前端构建完成 → web/dist/"
  else
    info "构建前端..."
    (cd "$WEB_DIR" && $PKG_MGR run build)
    ok "前端构建完成 → web/dist/"
  fi

  echo ""
fi

# ============ 编译完成 ============
if $BUILD_ONLY; then
  echo -e "${GREEN}============================================${NC}"
  echo -e "${GREEN}  编译完成！${NC}"
  echo -e "${GREEN}============================================${NC}"
  echo ""
  echo "  后端二进制: $BINARY"
  [ ! $SKIP_WEB ] && echo "  前端产物:   web/dist/"
  echo ""
  echo "  手动启动: $BINARY"
  exit 0
fi

# ============ 启动服务 ============
echo -e "${BOLD}启动服务${NC}"
echo "─────────────────────────────────────"

# 停止旧进程
stop_process "$BACKEND_PID" "旧后端进程" 2>/dev/null || true
stop_process "$FRONTEND_PID" "旧前端进程" 2>/dev/null || true

# 启动后端
info "启动后端服务..."
nohup "$BINARY" > "$BACKEND_LOG" 2>&1 &
BACKEND_REAL_PID=$!
echo "$BACKEND_REAL_PID" > "$BACKEND_PID"

# 等待后端端口就绪
waited=0
until curl -s http://127.0.0.1:8080 &>/dev/null || [ $waited -ge 15 ]; do
  sleep 1
  waited=$((waited + 1))
  # 检查进程是否还活着
  if ! kill -0 "$BACKEND_REAL_PID" 2>/dev/null; then
    echo ""
    fail "后端启动失败，查看日志: $BACKEND_LOG"
  fi
done
ok "后端服务已启动 (PID: $BACKEND_REAL_PID) → http://127.0.0.1:8080"

# 开发模式下启动前端 dev server
if $DEV_MODE && ! $SKIP_WEB; then
  PKG_MGR=$(detect_pkg_mgr)
  info "启动前端 Dev Server..."
  (cd "$WEB_DIR" && nohup $PKG_MGR run dev > "$SCRIPT_DIR/$FRONTEND_LOG" 2>&1 &
   echo $! > "$SCRIPT_DIR/$FRONTEND_PID")
  sleep 3
  if is_running "$FRONTEND_PID"; then
    ok "前端 Dev Server 已启动 (PID: $(cat "$FRONTEND_PID")) → http://127.0.0.1:3000"
  else
    warn "前端 Dev Server 启动可能失败，查看日志: $FRONTEND_LOG"
  fi
fi

# ============ 完成 ============
echo ""
echo -e "${GREEN}============================================${NC}"
echo -e "${GREEN}  CloudDisk 所有服务已启动！${NC}"
echo -e "${GREEN}============================================${NC}"
echo ""
echo "  服务地址:"
echo "    后端 API        http://127.0.0.1:8080"
if $DEV_MODE && ! $SKIP_WEB; then
  echo "    前端 (Dev)      http://127.0.0.1:3000"
fi
echo "    MySQL           127.0.0.1:3306"
echo "    Redis           127.0.0.1:6379"
echo "    Elasticsearch   http://127.0.0.1:9200"
echo "    RabbitMQ        http://127.0.0.1:15672"
echo ""
echo "  日志:"
echo "    后端: tail -f $BACKEND_LOG"
if $DEV_MODE && ! $SKIP_WEB; then
  echo "    前端: tail -f $FRONTEND_LOG"
fi
echo ""
echo "  管理:"
echo "    查看状态: bash start.sh --status"
echo "    停止所有: bash start.sh --stop"
echo ""
