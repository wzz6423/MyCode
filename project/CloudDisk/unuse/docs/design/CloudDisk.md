# CloudDisk

## 项目概述

## 需求文档接口设计
> 获取手机短信验证码   /service/user/get_phone_verify_code
> 获取邮箱验证码   /service/user/get_email_verify_code
> 注册   /service/user/register
> 登录   /service/user/login
> 获取个人信息   /service/user/get_user_info
> 修改头像   /service/user/set_avatar
> 修改昵称   /service/user/set_nickname
> 修改签名    /service/user/set_description
> 修改手机号   /service/user/set_phone
> 修改邮箱号   /service/user/set_email
> 修改密码   /service/user/set_password
> 上传文件 /service/file/upload
> 下载文件 /service/file/download
> 删除文件 /service/file/delete
> 展示文件列表 /service/file/show_file_list
> 分享文件 /service/file/share
> 取消分享 /service/file/unshare
> 公开文件 /service/file/public
> 取消公开 /service/file/unpublic
> 搜索文件 /service/file/search
> 获取分享/公开文件 /service/file/receive

### 功能设计
**采用微服务架构: 服务拆分、独立部署、编程语言与数据多样性、轻量级通信、去中心化、弹性管理、可扩展、容错性好、自动化部署、便于监控和日志记录、安全、服务注册与发现**

### 项目所用框架：

1. 框架: 
* RPC框架: bRPC
* 服务注册与发现框架: etcd + bRPC
* 数据库: odb
* 搜索引擎: elastic search
* C++ 标准: C++23(但不使用模块、协程等新特性)(对应 gcc15.1)
* 服务部署维护: docker

1. 文件存储方案

功能1：目录自动监测与备份
检测到选定目录下文件发生变化则进行云端同步

功能2：多端文件传输
客户端适配策略：
| 客户端 | 并发数 | 分片大小 | 压缩算法 | 重试机制 |
| ------ | ------ | -------- | -------- | -------- |
| PC端   | 8线程  | 10MB     | bundle   | 指数退避 |
| Web端  | 4线程  | 5MB      | bundle   | 固定间隔 |

功能3：断点续传(httplib 库天然支持)

功能4：文件分享(时间 & 次数)
访问控制逻辑：
1. 解码并验证签名有效性
2. 检查当前时间 < 过期时间
3. 检查当前下载次数 < 指定最大次数

功能5: 共享文件广场

功能6：秒传功能(结合布隆过滤器)