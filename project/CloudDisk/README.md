# CloudDisk - 云盘服务

基于 Golang 的云盘系统，提供文件上传/下载/分享、用户管理、文件广场等功能，同时支持手机号和邮箱双通道注册登录。

## 技术栈

| 层 | 技术 |
|---|------|
| 后端框架 | Gin |
| ORM | GORM + MySQL 8.0 |
| 缓存 | go-redis + Redis 7.4 |
| 搜索 | Elasticsearch 7.17 |
| 认证 | JWT (golang-jwt/v5) |
| 前端 | Vue 3 + Vite + TypeScript + Element Plus + Pinia |
| 测试 | testify + miniredis + httptest |
| 部署 | Docker Compose |

## 架构

```
┌─────────────────────────────────────────────────┐
│                   Vue 3 前端                    │
│  Login / Register / FileManager / PublicSquare  │
│              Profile / AppLayout                │
└──────────────────────┬──────────────────────────┘
                       │ HTTP (Axios)
┌──────────────────────▼──────────────────────────┐
│                  Gin HTTP Server                │
│  middleware: CORS → Logger → JWT Auth           │
├─────────────────────────────────────────────────┤
│  Handler 层 (请求解析 / 参数校验 / 响应封装)      │
│  UserHandler · FileHandler · ShareHandler       │
├─────────────────────────────────────────────────┤
│  Service 层 (业务逻辑)                           │
│  UserService · FileService · ShareService       │
│  VerifyService (验证码)                          │
├─────────────────────────────────────────────────┤
│  Repository 层 (数据访问接口)                     │
│  UserRepo · FileRepo · ShareRepo · ESRepo       │
├──────────┬──────────┬───────────┬───────────────┤
│  MySQL   │  Redis   │  Elastic  │  本地文件存储  │
└──────────┴──────────┴───────────┴───────────────┘
```

分层职责：
- Handler：HTTP 请求解析、参数绑定校验、调用 Service、统一 JSON 响应
- Service：核心业务逻辑，依赖 Repository 接口（便于 mock 测试）
- Repository：数据库 CRUD 操作，对上层屏蔽存储细节
- Middleware：JWT 鉴权、CORS 跨域、请求日志

## 项目结构

```
clouddisk/
├── cmd/server/main.go              # 服务入口
├── config/config.yaml              # 配置文件
├── migrations/001_init.sql         # 数据库建表 SQL
├── internal/
│   ├── config/config.go            # 配置加载
│   ├── middleware/                  # 中间件 (auth, cors, logger)
│   ├── model/                      # GORM 数据模型 (user, file, share)
│   ├── handler/                    # HTTP 处理器
│   ├── service/                    # 业务逻辑层
│   ├── repository/                 # 数据访问层
│   ├── router/router.go            # 路由注册
│   └── pkg/                        # 工具包 (jwt, hash, uuid, storage, response)
├── web/                            # Vue 3 前端
│   ├── src/
│   │   ├── api/                    # Axios 请求封装
│   │   ├── stores/                 # Pinia 状态管理
│   │   ├── views/                  # 页面组件
│   │   ├── components/             # 通用组件
│   │   └── router/                 # 前端路由
│   ├── package.json
│   └── vite.config.ts
├── Makefile                        # 构建脚本 (多平台交叉编译)
├── Dockerfile
└── docker-compose.yml
```

## 核心功能

- 用户：手机号/邮箱双通道注册登录、验证码、个人资料管理（头像/昵称/签名/密码/绑定手机/绑定邮箱）
- 文件：上传（支持秒传）、下载、删除、目录列表、全文搜索（ES）、新建文件夹（虚拟目录）
- 文件夹：虚拟文件夹管理、文件夹下载（递归打包 ZIP）、文件夹分享/公开（递归包含子文件）
- 批量操作：批量下载（ZIP 打包）、批量分享、批量公开、批量删除
- 分享：私密分享（链接+留言）、公开分享、接收文件（物理复制）、过期时间、次数限制
- 文件广场：浏览所有公开文件/文件夹、显示分享者昵称和头像、一键接收
- 安全：文件扩展名黑名单、文件名净化、路径穿越防护、Content-Disposition RFC 5987 编码、CSP 头、SQL LIKE 通配符转义
- 账号：注销账号（级联清理文件、分享记录、物理文件）

## 测试

### 测试概览

共 **13 个测试文件**，**134 个测试函数**，**172 个测试用例**（含子测试），全部通过。

| 包 | 测试文件 | 测试函数数 | 类型 |
|----|---------|-----------|------|
| `internal/pkg` | pkg_test.go | 12 (44 子测试) | 单元测试 |
| `internal/middleware` | auth_test.go | 6 | 单元测试 |
| `internal/service` | verify_test.go | 10 | 单元测试 |
| `internal/service` | user_test.go | 21 | 单元测试 |
| `internal/service` | share_test.go | 13 | 单元测试 |
| `internal/service` | file_test.go | 13 | 单元测试 |
| `internal/service` | chaos_test.go | 9 (15 子测试) | 混沌测试 |
| `internal/service` | regression_test.go | 13 | 回归测试 |
| `internal/handler` | user_test.go | 8 | 单元测试 |
| `internal/handler` | file_test.go | 7 | 单元测试 |
| `internal/handler` | share_test.go | 17 | 单元测试 |
| `internal/handler` | system_test.go | 5 | 系统测试 |

### 测试分类与测试点

#### 单元测试 (Unit Tests)

工具包 (`pkg`):
- 密码哈希：正确密码、错误密码、空密码、超长密码(500字符)、bcrypt 72字符上限、Unicode 密码、相同密码不同哈希、无效哈希
- JWT：生成+解析、错误密钥、空 UserID、篡改 Token、无效字符串、空字符串
- UUID：1000 个 UUID 唯一性、UUID v4 格式校验
- 文件类型检测：图片/音频/视频/压缩包/未知扩展名、大小写不敏感、无扩展名、纯点文件名
- 响应工具：Success/Error/ErrorWithCode JSON 输出格式
- 文件操作：FileExists 存在/不存在、DeleteFile 存在/不存在

中间件 (`middleware`):
- JWT 鉴权：有效 Token、缺少 Authorization、无 Bearer 前缀、无效 Token、空 Bearer、不同密钥签名

Service 层:
- 验证码：手机/邮箱发送、不支持的类型、验证成功/失败/过期、TTL 存储、Redis 宕机、默认长度/TTL
- 用户：手机/邮箱注册、无效验证码、过期验证码、重复手机/邮箱、不支持类型、手机/邮箱登录、错误密码、用户不存在、获取资料、修改密码/昵称/头像/签名/手机/邮箱
- 文件：下载成功/不存在/无权限、删除成功/不存在/无权限、列表分页/空结果/越界、公开列表
- 分享：分享成功/无过期/非所有者/文件不存在、取消分享、公开/取消公开、接收成功/过期/超限/不存在/无限次数

Handler 层:
- 用户 Handler：登录成功/参数错误/密码错误/空 Body/无效 JSON、获取资料成功/未授权/无效 Token
- 文件 Handler：列表成功/未授权/默认参数/空结果、公开列表成功/默认参数/分页
- 分享 Handler：分享/取消分享/公开/取消公开/接收 的成功/未授权/不存在/无权限/无效请求体

#### 混沌测试 (Chaos Tests)

- 并发注册：10 个 goroutine 同时注册不同手机号
- 并发接收分享：多个 goroutine 同时接收同一个分享
- 并发列表+删除：一个 goroutine 列表查询，另一个同时删除文件
- 边界值-空字符串：空字符串注册/登录
- 边界值-超长昵称：10000 字符昵称
- 边界值-特殊字符：Unicode/Emoji 昵称和签名
- 边界值-负数过期时间：负数 expire_hours 分享
- 边界值-零次数限制：MaxCount=0 无限分享
- 边界值-最大计数：MaxCount 接近 uint32 上限

#### 系统测试 (System/E2E Tests)

- 手机注册→登录→获取资料 完整流程
- 邮箱注册→登录→获取资料 完整流程
- 登录→文件列表为空 验证
- 分享完整流程：创建用户+文件→分享→另一用户接收→验证新文件
- 公开完整流程：创建用户+文件→公开→公开列表→验证文件出现

#### 回归测试 (Regression Tests)

- 注册后立即登录
- 修改手机号为已存在手机号 → 拒绝
- 修改邮箱为已存在邮箱 → 拒绝
- 分享→取消分享 文件状态正确重置
- 公开→取消公开 文件状态正确重置
- 接收自己的分享（无限制）
- 连续接收 3 次，计数正确递增
- 过期时间边界（恰好过期）
- 密码不泄露（json:"-" 标签生效）
- 两个用户 phone/email 均为 nil 不冲突
- 修改密码后旧密码失效、新密码生效
- 删除不存在的文件 → file not found
- 下载他人文件 → access denied

### 运行测试

```bash
# 运行全部测试
go test ./... -v -count=1

# 运行指定包
go test ./internal/service/... -v
go test ./internal/handler/... -v
go test ./internal/pkg/... -v
go test ./internal/middleware/... -v

# 运行指定测试
go test ./internal/service/... -run TestConcurrent -v
go test ./internal/service/... -run TestRegression -v
go test ./internal/handler/... -run TestSystem -v
```

## 快速启动

### 1. 一键初始化环境

```bash
bash setup.sh
```

自动检测并安装所有依赖（Docker、Go、Node.js、pnpm），拉取镜像，启动容器，下载依赖并构建前端。

| 参数 | 说明 |
|------|------|
| `--skip-docker` | 跳过 Docker 容器启动 |
| `--skip-web` | 跳过前端依赖安装和构建 |
| `--skip-backend` | 跳过后端依赖下载 |
| `--pull` | 强制重新拉取 Docker 镜像 |
| `--down` | 停止并移除所有容器 |

### 2. 编译并启动服务

```bash
bash start.sh
```

编译后端、构建前端并启动所有服务。

| 参数 | 说明 |
|------|------|
| `--dev` | 开发模式（启动前端 dev server，支持热更新） |
| `--build-only` | 只编译不启动 |
| `--skip-docker` | 跳过 Docker 容器 |
| `--skip-web` | 跳过前端 |
| `--stop` | 停止所有服务 |
| `--status` | 查看服务状态 |

### 常用操作

```bash
# 开发模式（前端热更新）
bash start.sh --dev

# 查看服务状态
bash start.sh --status

# 停止所有服务
bash start.sh --stop

# 停止并移除 Docker 容器
bash setup.sh --down
```

### 交叉编译

```bash
# 编译当前平台
make build

# 编译全部平台 (linux/darwin/windows × amd64/arm64)
make build-all
```

### 服务端口

| 服务 | 地址 |
|------|------|
| 后端 API | http://127.0.0.1:8080 |
| 前端 Dev | http://127.0.0.1:3000 |
| MySQL | 127.0.0.1:3306 |
| Redis | 127.0.0.1:6379 |
| Elasticsearch | http://127.0.0.1:9200 |
| RabbitMQ 管理面板 | http://127.0.0.1:15672 |

## API 概览

所有接口统一响应格式：`{"code": 0, "message": "success", "data": {...}}`

| 方法 | 路径 | 鉴权 | 说明 |
|------|------|------|------|
| POST | `/api/v1/user/verify/code` | 否 | 获取验证码 |
| POST | `/api/v1/user/register` | 否 | 注册 |
| POST | `/api/v1/user/login` | 否 | 登录 |
| GET | `/api/v1/user/profile` | 是 | 获取个人信息 |
| PUT | `/api/v1/user/avatar` | 是 | 修改头像 |
| PUT | `/api/v1/user/nickname` | 是 | 修改昵称 |
| PUT | `/api/v1/user/description` | 是 | 修改签名 |
| PUT | `/api/v1/user/password` | 是 | 修改密码 |
| PUT | `/api/v1/user/phone` | 是 | 绑定/修改手机 |
| PUT | `/api/v1/user/email` | 是 | 绑定/修改邮箱 |
| DELETE | `/api/v1/user/account` | 是 | 注销账号 |
| POST | `/api/v1/file/upload` | 是 | 上传文件 |
| GET | `/api/v1/file/download/:file_id` | 是 | 下载文件 |
| DELETE | `/api/v1/file/:file_id` | 是 | 删除文件 |
| GET | `/api/v1/file/list` | 是 | 文件列表 |
| GET | `/api/v1/file/search` | 是 | 搜索文件 |
| POST | `/api/v1/file/folder` | 是 | 新建文件夹 |
| POST | `/api/v1/file/batch-download` | 是 | 批量下载（ZIP） |
| GET | `/api/v1/file/public` | 否 | 文件广场 |
| POST | `/api/v1/file/:file_id/share` | 是 | 分享文件 |
| DELETE | `/api/v1/file/:file_id/share` | 是 | 取消分享 |
| POST | `/api/v1/file/:file_id/public` | 是 | 公开文件 |
| DELETE | `/api/v1/file/:file_id/public` | 是 | 取消公开 |
| POST | `/api/v1/file/receive/:share_id` | 是 | 接收分享 |
| GET | `/api/v1/share/:share_id` | 否 | 获取分享信息 |
