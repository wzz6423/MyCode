#pragma once

/*----------------------------------------------------------------------------------------------------------------*/

// C++
#include <iostream>
#include <optional>
#include <vector>
#include <unordered_map>
#include <any> // 或用自己实现的 Any
#include <memory>
#include <typeinfo>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

// C
#include <cstdint>
#include <cstring>
#include <cerrno>
#include <cassert>

// system call
#include <unistd.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/eventfd.h>

/*----------------------------------------------------------------------------------------------------------------*/

enum
{
    Info = 0,
    Debug,
    Warning,
    Error
};

#define DEFAULE_LOG_LEVEL Debug

#define Log(level, format, ...)                                                                               \
    do                                                                                                        \
    {                                                                                                         \
        if ((level) < (DEFAULE_LOG_LEVEL))                                                                    \
            break;                                                                                            \
        time_t current_time = time(nullptr);                                                                  \
        struct tm *ltm = localtime(&current_time);                                                            \
        char tmp[32] = {0};                                                                                   \
        strftime(tmp, sizeof(tmp) - 1, "%H:%M:%S", ltm);                                                      \
        std::string lev;                                                                                      \
        switch (level)                                                                                        \
        {                                                                                                     \
        case 0:                                                                                               \
            lev = "Info";                                                                                     \
            break;                                                                                            \
        case 1:                                                                                               \
            lev = "Debug";                                                                                    \
            break;                                                                                            \
        case 2:                                                                                               \
            lev = "Warning";                                                                                  \
            break;                                                                                            \
        case 3:                                                                                               \
            lev = "Error";                                                                                    \
            break;                                                                                            \
        default:                                                                                              \
            lev = "Unknown";                                                                                  \
            break;                                                                                            \
        }                                                                                                     \
        fprintf(stdout, "[%s] [%s %s:%d] " format "\n", lev.c_str(), tmp, __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (false)

#define Info_Log(format, ...) Log(Info, format, ##__VA_ARGS__)
#define Debug_Log(format, ...) Log(Debug, format, ##__VA_ARGS__)
#define Warning_Log(format, ...) Log(Warning, format, ##__VA_ARGS__)
#define Error_Log(format, ...) Log(Error, format, ##__VA_ARGS__)

/*----------------------------------------------------------------------------------------------------------------*/

#define DEFAULT_BUFFER_SIZE 4096

class Buffer
{
public:
    Buffer()
        : _reader_idx(0),
          _writer_idx(0),
          _buffer(DEFAULT_BUFFER_SIZE)
    {
    }

    std::optional<char *> Begin()
    {
        std::vector<char>::iterator it = _buffer.begin();
        if (it != _buffer.end())
        {
            return &(*it);
        }
        else
        {
            return std::nullopt;
        }
    }

    // 获取当前写起始地址 -- _buffer空间起始地址加写偏移量
    std::optional<char *> GetWritePos()
    {
        std::optional<char *> ret = Begin();
        if (ret)
        {
            return ret.value() + _writer_idx;
        }
        else
        {
            return std::nullopt;
        }
    }

    // 获取当前读起始地址
    std::optional<char *> GetReadPos()
    {
        std::optional<char *> ret = Begin();
        if (ret)
        {
            return ret.value() + _reader_idx;
        }
        else
        {
            return std::nullopt;
        }
    }

    // 获取缓冲区末尾空闲空间大小 -- 写偏移之后的空闲空间
    uint64_t TailIdleSize()
    {
        return _buffer.size() - _writer_idx;
    }

    // 获取缓冲区起始空闲空间大小 -- 读偏移之前的空闲空间
    uint64_t HeadIdleSize()
    {
        return _reader_idx;
    }

    // 获取可读数据大小
    uint64_t ReadAbleSize()
    {
        return _writer_idx - _reader_idx;
    }

    // 将读偏移向后移动
    bool MoveReaderOffset(uint64_t len)
    {
        if (len == 0)
        {
            return true;
        }

        // 向后移动的大小必须小于可读的大小
        assert(len <= ReadAbleSize());
        if (len <= ReadAbleSize())
        {
            _reader_idx += len;
            return true;
        }
        else
        {
            return false;
        }
    }

    // 将写偏移向后移动
    bool MoveWriterOffset(uint64_t len)
    {
        if (len == 0)
        {
            return true;
        }

        // 向后移动的大小必须小于可写的大小 -- 后面的空闲空间
        assert(len <= TailIdleSize());
        if (len <= TailIdleSize())
        {
            _writer_idx += len;
            return true;
        }
        else
        {
            return false;
        }
    }

    // 确保可写空间足够 -- 整体空闲空间足够就移动数据, 否则扩容(若回头部写入设计成环回的不好维护)
    bool EnsureWriteSpace(uint64_t len)
    {
        // 如果末尾空闲空间大小足够就直接返回
        if (len <= TailIdleSize())
        {
            return true;
        }
        // 末尾空闲空间不足, 判断起始位置空闲空间与末尾空闲空间总和是否足够, 足够则移动数据到起始位置腾出空间
        else if (len <= HeadIdleSize() + TailIdleSize())
        {
            // 将数据移动到起始位置
            uint64_t rsz = ReadAbleSize(); // 保存当前数据大小
            std::copy(GetReadPos().value(), GetReadPos().value() + rsz, Begin().value());
            _reader_idx = 0;   // 读偏移归零
            _writer_idx = rsz; // 将写位置重置为可读数据大小 -- 即写偏移量
            return true;
        }
        // 总体空间不够, 需要扩容, 多扩一点方便后续使用
        else if (len > HeadIdleSize() + TailIdleSize())
        {
            uint64_t rsz = ReadAbleSize(); // 保存当前数据大小
            std::copy(GetReadPos().value(), GetReadPos().value() + rsz, Begin().value());
            _reader_idx = 0;   // 读偏移归零
            _writer_idx = rsz; // 将写位置重置为可读数据大小 -- 即写偏移量
            size_t addSize = len > (DEFAULT_BUFFER_SIZE / 8) ? len : (DEFAULT_BUFFER_SIZE / 8);
            _buffer.resize(_buffer.size() + addSize);
            return true;
        }
        else
        {
            return false;
        }
    }

    // 写入数据
    bool Write(const void *data, uint64_t len)
    {
        if (len == 0)
        {
            return true;
        }

        // 先保证空间足够
        EnsureWriteSpace(len);

        // 写入数据
        const char *cd = static_cast<const char *>(data);
        std::copy(cd, cd + len, GetWritePos().value());
        return true;
    }

    bool WriteAndPush(const void *data, uint64_t len)
    {
        bool retWri = Write(data, len);
        bool retMov = MoveWriterOffset(len);
        if (retWri && retMov)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool WriteString(const std::string &data)
    {
        return Write(data.c_str(), data.size());
    }

    bool WriteStringAndPush(const std::string &data)
    {
        bool retWri = Write(data.c_str(), data.size());
        bool retMov = MoveWriterOffset(data.size());
        if (retWri && retMov)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool WriteBuffer(const Buffer &buf)
    {
        return Write(const_cast<Buffer &>(buf).GetReadPos().value(), const_cast<Buffer &>(buf).ReadAbleSize());
    }

    bool WriteBufferAndPush(const Buffer &buf)
    {
        bool retWri = Write(const_cast<Buffer &>(buf).GetReadPos().value(), const_cast<Buffer &>(buf).ReadAbleSize());
        bool retMov = MoveWriterOffset(const_cast<Buffer &>(buf).ReadAbleSize());
        if (retWri && retMov)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    // 读取数据
    bool Read(void *buf, uint64_t &len)
    {
        // 要获取的数据大小必须小于可读数据大小
        assert(len <= ReadAbleSize());
        if (len <= ReadAbleSize())
        {
            std::copy(GetReadPos().value(), GetReadPos().value() + len, static_cast<char *>(buf));
            return true;
        }
        else
        {
            return false;
        }
    }

    bool ReadAndPop(void *buf, uint64_t &len)
    {
        bool retRed = Read(buf, len);
        bool retMov = MoveReaderOffset(len);
        if (retRed && retMov)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    std::optional<std::string> ReadAsString(uint64_t len)
    {
        std::string ret;
        ret.resize(len);
        bool retRed = Read(&ret[0], len); // c_str() 返回的是 const 的, 无法进行操作, 这样写可以正常写入
        if (retRed)
        {
            return ret;
        }
        else
        {
            return std::nullopt;
        }
    }

    std::optional<std::string> ReadAsStringAndPop(uint64_t len)
    {
        std::optional<std::string> ret = ReadAsString(len);
        if (ret)
        {
            MoveReaderOffset(len);
            return ret;
        }
        else
        {
            return std::nullopt;
        }
    }

    std::optional<char *> FindCRLF()
    {
        std::optional<char *> readPos = GetReadPos();
        char *ret = nullptr;
        if (readPos)
        {
            ret = static_cast<char *>(memchr(readPos.value(), '\n', ReadAbleSize()));
        }
        if (ret)
        {
            return ret;
        }
        else
        {
            return std::nullopt;
        }
    }

    // 获取一行数据是安全的, 因为通常(此处)针对的是ASCII码
    std::optional<std::string> GetLine()
    {
        std::optional<char *> pos = FindCRLF();
        if (pos)
        {
            std::optional<char *> readPos = GetReadPos();
            if (readPos)
            {
                std::optional<std::string> ret = ReadAsString(pos.value() - readPos.value() + 1);
                return ret; // 在 ReadAsString 已经进行过安全性检查, 故此处不必再次进行检查
            }
            else
            {
                return std::nullopt;
            }
        }
        else
        {
            return std::nullopt;
        }
    }

    std::optional<std::string> GetLineAndPop()
    {
        std::optional<std::string> res = GetLine();
        if (res)
        {
            bool retMov = MoveReaderOffset(res.value().size());
            if (retMov)
            {
                return res;
            }
        }
        return std::nullopt;
    }

    // 清空缓冲区
    bool Clear()
    {
        // 不用清除内容, 归零两个偏移量即可
        _reader_idx = 0;
        _writer_idx = 0;
        return true;
    }

public:
    std::vector<char> _buffer; // 使用 vector 进行内存空间管理
    uint64_t _reader_idx;      // 读偏移
    uint64_t _writer_idx;      // 写偏移
};

/*----------------------------------------------------------------------------------------------------------------*/

// 通用类型
class Any
{
public:
    Any()
        : _content(nullptr)
    {
    }

    template <typename T>
    Any(const T &val)
        : _content(new placeholder<T>(val))
    {
    }

    Any(const Any &other)
        : _content(other._content ? other._content->Clone() : nullptr)
    {
    }

    Any &Swap(Any &other)
    {
        std::swap(_content, other._content);
        return *this;
    }

    // 这三个函数用于优化
    Any(Any &&other) noexcept
        : _content(other._content)
    {
        other._content = nullptr;
    }
    template <typename T, typename... Args>
    void emplace(Args &&...args)
    {
        delete _content;
        _content = new placeholder<T>(std::forward<Args>(args)...);
    }
    Any &operator=(Any &&other) noexcept
    {
        if (this != &other)
        {
            delete _content;
            _content = other._content;
            other._content = nullptr;
        }
        return *this;
    }

    // 返回子类对象保存数据的指针
    template <typename T>
    std::optional<T *> Get()
    {
        // 想要获取的数据类型必须和保存的数据类型一致
        if (_content && typeid(T) == _content->Type())
        {
            return &static_cast<placeholder<T> *>(_content)->_val;
        }
        return std::nullopt;
    }

    // 赋值运算符重载
    template <typename T>
    Any &operator=(const T &val)
    {
        // 为 val 构造一个临时通用容器, 然后与当前容器自身指针交换, 临时对象释放时就释放掉原先资源
        Any(val).Swap(*this);
        return *this;
    }

    Any &operator=(const Any &other)
    {
        Any(other).Swap(*this);
        return *this;
    }

    ~Any()
    {
        delete _content;
    }

private:
    class holder
    {
    public:
        virtual ~holder() {}
        virtual const std::type_info &Type() = 0;
        virtual holder *Clone() = 0;
    };

    template <typename T>
    class placeholder : public holder
    {
    public:
        placeholder(const T &val)
            : _val(val)
        {
        }

        // 获取子类对象保存的数据类型
        virtual const std::type_info &Type()
        {
            return typeid(T);
        }

        // 针对当前对象克隆出一个新的子类对象
        virtual holder *Clone()
        {
            return new placeholder(_val);
        }

    public:
        T _val;
    };

private:
    holder *_content;
};

/*----------------------------------------------------------------------------------------------------------------*/

#define MAX_LISTEN_SIZE 1024
class Socket
{
public:
    Socket()
        : _sockfd(-1)
    {
    }

    Socket(int sockfd)
        : _sockfd(sockfd)
    {
    }

    ~Socket()
    {
        Close();
    }

    // 获取套接字
    int Fd()
    {
        return _sockfd;
    }

    // 创建套接字
    std::optional<int> Create()
    {
        // int socket(int domain, int type, int protocol);
        _sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (_sockfd < 0)
        {
            Error_Log("create socket error!\n");
            return std::nullopt;
        }
        else
        {
            return _sockfd;
        }
    }

    // 绑定地址信息
    bool Bind(const std::string &ip, const uint16_t &port)
    {
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        socklen_t len = sizeof(addr);
        // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
        int ret = bind(_sockfd, reinterpret_cast<struct sockaddr *>(&addr), len);
        if (ret < 0)
        {
            Error_Log("bind address error!\n");
            return false;
        }
        else
        {
            return true;
        }
    }

    // 监听
    bool Listen(int backlog = MAX_LISTEN_SIZE)
    {
        // int listen(int sockfd, int backlog);
        int ret = listen(_sockfd, backlog);
        if (ret < 0)
        {
            Error_Log("listen error!\n");
            return false;
        }
        else
        {
            return true;
        }
    }

    // 向服务器发起连接
    bool Connect(const std::string &ip, const uint16_t port)
    {
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        socklen_t len = sizeof(addr);
        // int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
        // int ret = connect(_sockfd, (struct sockaddr*)(&addr), len);
        int ret = connect(_sockfd, reinterpret_cast<struct sockaddr *>(&addr), len);
        if (ret < 0)
        {
            Error_Log("connect address error!\n");
            return false;
        }
        else
        {
            return true;
        }
    }

    // 获取新连接
    int Accept()
    {
        // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
        int newfd = accept(_sockfd, nullptr, nullptr);
        if (newfd < 0)
        {
            Error_Log("accept address error!\n");
            return -1;
        }
        else
        {
            return newfd;
        }
    }

    int Accept(std::string &ip, uint16_t &port)
    {
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        socklen_t len = sizeof(addr);
        // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
        int newfd = accept(_sockfd, reinterpret_cast<struct sockaddr *>(&addr), &len);
        if (newfd < 0)
        {
            Error_Log("accept address error!\n");
            return -1;
        }
        else
        {
            ip = addr.sin_addr.s_addr;
            port = ntohs(addr.sin_port);
            return newfd;
        }
    }

    // 接收数据
    ssize_t Recv(void *buf, size_t len, int flag = 0)
    {
        // ssize_t recv(int sockfd, void *buf, size_t len, int flags);
        ssize_t ret = recv(_sockfd, buf, len, flag);
        if (ret <= 0)
        {
            // EAGAIN 表示当前 socket 接收缓冲区中没有数据, 在非阻塞情况下会出现这个错误
            // EINTR 表示当前 socket 的阻塞等待被信号打断了
            if (errno == EAGAIN || errno == EINTR)
            {
                return 0; // 表示本次没有接收到数据
            }
            else
            {
                Error_Log("recv error!\n");
                return -1;
            }
        }
        else
        {
            return ret; // 实际接收到的数据长度
        }
    }

    // 非阻塞接收数据
    ssize_t NonBlockRecv(void *buf, size_t len)
    {
        return Recv(buf, len, MSG_DONTWAIT); // MSG_DONTWAIT 表示当前接收为非阻塞
    }

    // 发送数据
    size_t Send(const void *buf, size_t len, int flag = 0)
    {
        if (len == 0)
        {
            return 0;
        }
        // ssize_t send(int sockfd, const void *buf, size_t len, int flags);
        ssize_t ret = send(_sockfd, buf, len, flag);
        if (ret <= 0)
        {
            // EAGAIN 表示当前 socket 接收缓冲区中没有数据, 在非阻塞情况下会出现这个错误
            // EINTR 表示当前 socket 的阻塞等待被信号打断了
            if (errno == EAGAIN || errno == EINTR)
            {
                return 0; // 表示本次没有接收到数据
            }
            else
            {
                Error_Log("send error!\n");
                return -1;
            }
        }
        else
        {
            return ret; // 实际发送出去的数据长度
        }
    }

    // 非阻塞发送数据
    ssize_t NonBlockSend(void *buf, size_t len)
    {
        if (len == 0)
        {
            return 0;
        }
        return Send(buf, len, MSG_DONTWAIT); // MSG_DONTWAIT 表示当前接收为非阻塞
    }

    // 关闭套接字
    void Close()
    {
        if (_sockfd != -1)
        {
            close(_sockfd);
            _sockfd = -1;
        }
    }

    // 创建一个服务端连接
    bool CreateServer(uint16_t port, const std::string &ip = "0.0.0.0", bool blockFlag = false)
    {
        // 创建套接字
        if (Create() == std::nullopt)
        {
            return false;
        }
        // 设置非阻塞
        if (blockFlag)
        {
            if (SetNonBlock() == false)
            {
                return false;
            }
        }

        // 绑定地址
        if (Bind(ip, port) == false)
        {
            return false;
        }

        // 开始监听
        if (Listen() == false)
        {
            return false;
        }

        // 设置地址端口重用
        ReuseAddress();

        return true;
    }

    // 创建一个客户端连接
    bool CreateClient(uint16_t port, const std::string &ip)
    {
        // 创建套接字
        if (Create() == std::nullopt)
        {
            return false;
        }

        // 指向连接服务器
        if (Connect(ip, port) == false)
        {
            return false;
        }

        return true;
    }

    // 设置套接字 ip 地址端口重用
    bool ReuseAddress()
    {
        // int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
        int val = 1;
        setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, static_cast<void *>(&val), sizeof(int)); // ip地址
        val = 1;
        setsockopt(_sockfd, SOL_SOCKET, SO_REUSEPORT, static_cast<void *>(&val), sizeof(int)); // 端口号
        return true;
    }

    // 设置套接字非阻塞
    bool SetNonBlock()
    {
        // int fcntl(int fd, int cmd, ... /* arg */ );
        int flag = fcntl(_sockfd, F_GETFL, 0);
        fcntl(_sockfd, F_SETFL, flag | O_NONBLOCK);
        return true;
    }

private:
    int _sockfd; // 套接字
};

/*----------------------------------------------------------------------------------------------------------------*/

class EPoller;
class EventLoop;

using EventCallback = std::function<void()>;
class Channel
{
public:
    Channel(int fd, EventLoop *loop)
        : _fd(fd),
          _loop(loop),
          _events(0),
          _revents(0)
    {
    }

    // 获得当前 Channel 监控的文件描述符
    int Fd()
    {
        return _fd;
    }

    // 获取想要监控的事件
    uint32_t GetEvents()
    {
        return _events;
    }

    // 设置实际就绪的事件
    bool SetEvents(uint32_t events)
    {
        _revents = events;
        return true;
    }

    // 设置可读回调函数
    bool SetReadCb(const EventCallback &readCb)
    {
        _read_cb = readCb;
        return true;
    }

    // 设置可写回调函数
    bool SetWriteCb(const EventCallback &writeCb)
    {
        _write_cb = writeCb;
        return true;
    }

    // 设置异常回调函数
    bool SetErrorCb(const EventCallback &errorCb)
    {
        _error_cb = errorCb;
        return true;
    }

    // 设置连接断开回调函数
    bool SetCloseCb(const EventCallback &closeCb)
    {
        _close_cb = closeCb;
        return true;
    }

    // 设置任意回调函数
    bool SetAnyCb(const EventCallback &anyCb)
    {
        _any_cb = anyCb;
        return true;
    }

    // 查询当前是否监控了可读
    bool ReadAble()
    {
        return (_events & EPOLLIN);
    }

    // 查询当前是否监控了可写
    bool WriteAble()
    {
        return (_events & EPOLLOUT);
    }

    // 启动读事件监控
    bool EnableRead()
    {
        _events |= EPOLLIN;
        return CreateAndUpdate();
    }

    // 启动写事件监控
    bool EnableWrite()
    {
        _events |= EPOLLOUT;
        return CreateAndUpdate();
    }

    // 关闭读事件监控
    bool DisableRead()
    {
        _events &= ~EPOLLIN;
        return CreateAndUpdate();
    }

    // 关闭写事件监控
    bool DisableWrite()
    {
        _events &= ~EPOLLOUT;
        return CreateAndUpdate();
    }

    // 关闭所有事件监控
    bool DisableAll()
    {
        _events = 0;
        return CreateAndUpdate();
    }

    // 移除监控
    bool Remove();

    // 创建/更新监控
    bool CreateAndUpdate();

    // 事件处理, 触发了事件就调用这个函数, 触发什么时间并如何应对由这个函数操作
    void HandleEvent()
    {
        if ((_revents & EPOLLIN) || (_revents & EPOLLRDHUP) || (_revents & EPOLLPRI))
        {
            if (_read_cb)
            {
                _read_cb();
            }
        }

        // 可能会释放连接的操作, 一次只处理一个
        if (_revents & EPOLLOUT)
        {
            if (_write_cb)
            {
                _write_cb();
            }
        }
        else if (_revents & EPOLLERR)
        {
            if (_error_cb)
            {
                _error_cb();
            }
        }
        else if (_revents & EPOLLHUP)
        {
            if (_close_cb)
            {
                _close_cb();
            }
        }

        if (_any_cb)
        {
            _any_cb();
        }
    }

private:
    int _fd; // 当前 Channel 要监控的文件描述符
    EventLoop *_loop;
    uint32_t _events;        // 当前要监控的事件
    uint32_t _revents;       // 当前连续触发的事件
    EventCallback _read_cb;  // 可读事件
    EventCallback _write_cb; // 可写事件
    EventCallback _error_cb; // 异常事件
    EventCallback _close_cb; // 连接断开事件
    EventCallback _any_cb;   // 任意事件
};

/*----------------------------------------------------------------------------------------------------------------*/

#define MAX_EPOLLEVENTS_SIZE 1024
class EPoller
{
public:
    EPoller()
    {
        _epfd = epoll_create(1); // 这个整数随意填写
        if (_epfd < 0)
        {
            Error_Log("create epoll error!\n");
            abort(); // 退出进程
        }
    }

    // 添加或修改监控事件
    bool CreateAndUpdateEvent(Channel *channel)
    {
        std::optional<Channel *> ret = HasChannel(channel);
        if (ret == std::nullopt)
        {
            // 不存在就添加
            _channels[channel->Fd()] = channel;
            return CreateAndUpdate(channel, EPOLL_CTL_ADD);
        }
        return CreateAndUpdate(channel, EPOLL_CTL_MOD);
    }

    // 移除监控
    bool RemoveEvent(Channel *channel)
    {
        std::unordered_map<int, Channel *>::iterator it = _channels.find(channel->Fd());
        if (it != _channels.end())
        {
            _channels.erase(it);
            return CreateAndUpdate(channel, EPOLL_CTL_DEL);
        }
        else
        {
            return false;
        }
    }

    // 开始监控, 返回活跃连接
    bool EPoll(std::vector<Channel *> &active)
    {
        // int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
        int nfds = epoll_wait(_epfd, _events, MAX_EPOLLEVENTS_SIZE, -1); // -1: 阻塞式等
        if (nfds < 0)
        {
            if (errno == EINTR)
            {
                return false;
            }
            Error_Log("epoll wait error: %s", strerror(errno));
            abort(); // 退出进程
        }
        else
        {
            for (size_t i = 0; i < nfds; ++i)
            {
                std::unordered_map<int, Channel *>::iterator it = _channels.find(_events[i].data.fd);
                assert(it != _channels.end());
                if (it == _channels.end())
                {
                    return false;
                }
                else
                {
                    it->second->SetEvents(_events[i].events); // 设置实际就绪的事件
                    active.push_back(it->second);
                }
            }
        }
        return true;
    }

private:
    // 对 epoll 的直接操作
    bool CreateAndUpdate(Channel *channel, int op)
    {
        // int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
        int fd = channel->Fd();
        struct epoll_event event;
        event.data.fd = fd;
        event.events = channel->GetEvents();
        int ret = epoll_ctl(_epfd, op, fd, &event);
        if (ret < 0)
        {
            Error_Log("epoll ctl error!\n");
            return false;
        }
        else
        {
            return true;
        }
    }

    // 判断一个 Channel 是否已经添加了事件监控
    std::optional<Channel *> HasChannel(Channel *channel)
    {
        std::unordered_map<int, Channel *>::iterator it = _channels.find(channel->Fd());
        if (it != _channels.end())
        {
            return it->second;
        }
        else
        {
            return std::nullopt;
        }
    }

private:
    int _epfd;                                        // epoll 的文件描述符/句柄
    struct epoll_event _events[MAX_EPOLLEVENTS_SIZE]; // epoll 监控的对象的事件数组
    std::unordered_map<int, Channel *> _channels;     // channel 监控的文件描述符和其 channel 的映射
};

/*----------------------------------------------------------------------------------------------------------------*/

using TaskFunc = std::function<void()>;
using ReleaseFunc = std::function<void()>;
class TimerTask
{
public:
    TimerTask(uint64_t id, uint32_t timeout, const TaskFunc &task_cb)
        : _id(id),
          _timeout(timeout),
          _task_cb(task_cb)
    {
    }

    void Cancel()
    {
        _canceled = true;
    }

    void SetRelease(const ReleaseFunc &release_cb)
    {
        _release_cb = release_cb;
    }

    uint32_t DelayTime()
    {
        return _timeout;
    }

    ~TimerTask()
    {
        if (_canceled == false)
        {
            _task_cb();
        }
        _release_cb();
    }

private:
    uint64_t _id = 0;        // 定时器对象任务 id
    uint32_t _timeout = 0;   // 定时任务的超时时间
    bool _canceled = false;  // false-没有被取消 true-被取消
    TaskFunc _task_cb;       // 定时器对象要执行的定时任务
    ReleaseFunc _release_cb; // 用于删除 TimeWheel 中保存的定时器对象的信息
};

class TimerWheel
{
public:
    TimerWheel(EventLoop *loop)
        : _tick(0),
          _capacity(60),
          _wheel(_capacity),
          _loop(loop),
          _timerfd(CreateTimerfd()),
          _timer_channel(new Channel(_timerfd, _loop))
    {
        _timer_channel->SetReadCb(std::bind(&TimerWheel::OnTime, this));
        _timer_channel->EnableRead(); // 启动读事件监控
    }

    // 定时器中有 _timers 成员, 定时器信息操作有线程安全问题, 加锁降低效率, 因此把定时器的操作放在一个线程中进行
    void TimerAdd(uint64_t id, uint32_t delayTime, const TaskFunc &task_cb);
    void TimerRefresh(uint64_t id);
    void TimerCancel(uint64_t id);

    // 此接口存在线程安全问题, 不能被外界使用者调用, 只能在模块内部, 在对应的 EventLoop 线程内执行
    bool HasTimer(uint64_t id)
    {
        std::unordered_map<uint64_t, std::weak_ptr<TimerTask>>::iterator it = _timers.find(id);
        if (it != _timers.end())
        {
            return true;
        }
        return false;
    }

    ~TimerWheel() = default;

private:
    // 取消定时任务
    void RemoveTimer(uint64_t id)
    {
        std::unordered_map<uint64_t, std::weak_ptr<TimerTask>>::iterator it = _timers.find(id);
        if (it != _timers.end())
        {
            _timers.erase(id);
        }
    }

    // 获得 timerfd
    static int CreateTimerfd()
    {
        int timerfd = timerfd_create(CLOCK_MONOTONIC, 0);
        if (timerfd < 0)
        {
            Error_Log("timerfd create error!\n");
            abort();
        }
        else
        {
            // int timerfd_settime(int fd, int flags, const struct itimerspec *new_value, struct itimerspec *old_value);
            struct itimerspec itime;
            itime.it_interval.tv_sec = 1;
            itime.it_interval.tv_nsec = 0; // 第一次超时时间为 1s 后
            itime.it_value.tv_sec = 1;
            itime.it_value.tv_nsec = 0; // 第一次超时后每次超时的间隔时间
            timerfd_settime(timerfd, 0, &itime, nullptr);
            return timerfd;
        }
    }

    // 读取 _timerfd
    int ReadTimefd()
    {
        uint64_t times;
        // 可能因为其它事务处理时间较长, 等到处理定时器描述符事件时已经超时多次
        // read 读到的是从上一次 read 之后超时次数
        int ret = read(_timerfd, &times, 8);
        if (ret < 0)
        {
            Error_Log("read timerfd error!\n");
            abort();
        }
        else
        {
            return times;
        }
    }

    // 此函数每秒钟被执行一次, 相当于嘀嗒指针 1s 向后走了一步, 执行一次定时任务
    void RunTimerTask()
    {
        _tick = (_tick + 1) % _capacity;
        _wheel[_tick].clear(); // 清空指定位置的数组就会把该数组中保存的所有管理定时器对象的 shared_ptr 释放掉
    }

    // 根据超时次数执行超时任务
    void OnTime()
    {
        // 根据实际超时次数, 执行对应超时任务
        int times = ReadTimefd();
        for (size_t i = 0; i < times; ++i)
        {
            RunTimerTask();
        }
    }

    // 保证线程安全
    // 添加定时任务
    bool TimerAddInLoop(uint64_t id, uint32_t delayTime, const TaskFunc &task_cb)
    {
        std::shared_ptr<TimerTask> ptr = std::make_shared<TimerTask>(id, delayTime, task_cb);
        ptr->SetRelease(std::bind(&TimerWheel::RemoveTimer, this, id));
        int pos = (_tick + delayTime) % _capacity;
        _wheel[pos].push_back(ptr);
        _timers[id] = std::weak_ptr<TimerTask>(ptr);
        return true;
    }

    // 刷新/延迟定时任务 -- 通过 shared_ptr 销毁时检查计数器的原理实现
    bool TimerRefreshInLoop(uint64_t id)
    {
        // 通过保存的 weak_ptr 构造出一个 shared_ptr 添加到时间轮中
        std::unordered_map<uint64_t, std::weak_ptr<TimerTask>>::iterator it = _timers.find(id);
        if (it == _timers.end())
        {
            return false; // 没有找到定时任务, 无法进行刷新延迟
        }
        std::shared_ptr<TimerTask> ptr = it->second.lock(); // 通过 it 获取定时任务对象的 weak_ptr 后通过 lock 函数获取其管理对象的 shared_ptr
        int pos = (_tick + ptr->DelayTime()) % _capacity;
        _wheel[pos].push_back(ptr);
        return true;
    }

    bool TimerCancelInLoop(uint64_t id)
    {
        std::unordered_map<uint64_t, std::weak_ptr<TimerTask>>::iterator it = _timers.find(id);
        if (it == _timers.end())
        {
            return false; // 没有找到定时任务, 无法进行取消
        }
        std::shared_ptr<TimerTask> ptr = it->second.lock(); // 通过 it 获取定时任务对象的 weak_ptr 后通过 lock 函数获取其管理对象的 shared_ptr
        if (ptr)
        {
            ptr->Cancel();
        }
        return true;
    }

    TimerWheel(const TimerWheel &) = delete;
    TimerWheel &operator=(const TimerWheel &) = delete;

private:
    int _tick = 0;                                                  // 嘀嗒指针, 走到哪里释放哪里(执行哪里的任务)
    int _capacity = 0;                                              // 表盘的最大容量 -- 本质是最大延迟时间
    std::vector<std::vector<std::shared_ptr<TimerTask>>> _wheel;    // 时间轮数组 -- 如果需要扩充定时时间只需要再创建 分钟数组、小时数组、日子数组、周数组、月数组、年数组... 进行扩展即可(时间到了把任务向上一级数组进行转移即可)
    std::unordered_map<uint64_t, std::weak_ptr<TimerTask>> _timers; // 记录 TimerTask 的定时器对象任务 id 与其对应的 weak_ptr
    EventLoop *_loop;
    int _timerfd; // 定时器描述符 -- 可读事件回调就是读取计数器, 执行定时任务
    std::unique_ptr<Channel> _timer_channel;
};

/*----------------------------------------------------------------------------------------------------------------*/

class EventLoop
{
private:
    using Functor = std::function<bool()>;

public:
    // 创建 eventfd
    static int CreatEventFd()
    {
        int efd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK); // 不允许该文件描述符被子进程拷贝, 设置非阻塞
        if (efd < 0)
        {
            Error_Log("create eventfd error!\n");
            abort(); // 进程异常退出
        }
        else
        {
            return efd;
        }
    }

    EventLoop()
        : _thread_id(std::this_thread::get_id()),
          _event_fd(CreatEventFd()),
          _event_channel(new Channel(_event_fd, this)),
          _timer_wheel(this)
    {
        // 为 _event_fd 添加可读事件回调函数, 读取 eventfd 事件通知次数
        _event_channel->SetReadCb(std::bind(&EventLoop::ReadEventFd, this));

        // 启动 eventfd 读事件监控
        _event_channel->EnableRead();
    }

    // 事件监控 -> 就绪事件处理(执行/入任务队列) -> 执行任务
    void Start()
    {
        while (true)
        {
            // 事件监控
            std::vector<Channel *> actives;
            _epoller.EPoll(actives);

            // 就绪事件处理(执行/入任务队列)
            for (auto &channel : actives)
            {
                channel->HandleEvent();
            }

            // 执行任务
            RunAllTask();
        }
    }

    // 用于判断当前线程是否为 EventLoop 对应线程
    bool IsInLoop()
    {
        return (_thread_id == std::this_thread::get_id());
    }

    inline void AssertInLoop()
    {
        assert(_thread_id == std::this_thread::get_id());
    }

    // 判断将要执行的任务是否处于当前线程中, 如果是则执行, 否则入执行队列
    bool RunInLoop(const Functor &cb)
    {
        if (IsInLoop())
        {
            return cb();
        }
        else
        {
            QueueInLoop(cb);
            return true;
        }
    }

    // 将操作压入任务池
    void QueueInLoop(const Functor &cb)
    {
        {
            std::unique_lock<std::mutex> lock(_mtx);
            _tasks.push_back(cb);
        }
        // 唤醒有可能因为没有事件就绪而阻塞的 epoll
        // 本质就是给 eventfd 写入一个数据, eventfd 会触发可读事件
        WeakUpEventFd();
    }

    // 添加/修改描述符的事件监控
    bool CreateAndUpdateEvent(Channel *channel)
    {
        return _epoller.CreateAndUpdateEvent(channel);
    }

    // 解除描述符监控
    bool RemoveEvent(Channel *channel)
    {
        return _epoller.RemoveEvent(channel);
    }

    // 添加定时任务
    void TimerAdd(uint64_t id, uint32_t delayTime, const TaskFunc &task_cb)
    {
        return _timer_wheel.TimerAdd(id, delayTime, task_cb);
    }

    // 刷新/延迟定时任务
    void TimerRefresh(uint64_t id)
    {
        return _timer_wheel.TimerRefresh(id);
    }

    // 取消定时任务
    void TimerCancel(uint64_t id)
    {
        return _timer_wheel.TimerCancel(id);
    }

    // 检查定时任务是否已经存在
    bool HasTimer(uint64_t id)
    {
        return _timer_wheel.HasTimer(id);
    }

    // 执行任务池中所有任务
    void RunAllTask()
    {
        std::vector<Functor> functor;
        {
            std::unique_lock<std::mutex> lock(_mtx);
            _tasks.swap(functor);
        }
        for (auto &f : functor)
        {
            f();
        }
    }

    // 读取 _event_fd, 清除上一次的提醒
    void ReadEventFd()
    {
        uint64_t res = 0;
        int ret = read(_event_fd, &res, sizeof(res)); // 必须 8 字节
        if (ret < 0)
        {
            // EINTR -- 被信号打断
            // EAGAIN -- 无数据
            if (errno == EINTR || errno == EAGAIN)
            {
                return;
            }
            else
            {
                Error_Log("read eventfd error!\n");
                abort();
            }
        }
    }

    // 向 _event_fd 中写入数据唤醒 IO 事件
    void WeakUpEventFd()
    {
        uint64_t val = 1;
        int ret = write(_event_fd, &val, sizeof(val));
        if (ret < 0)
        {
            if (errno == EINTR)
            {
                return;
            }
            Error_Log("write eventfd error!\n");
            abort();
        }
    }

private:
    std::thread::id _thread_id; // 当前线程 ID
    int _event_fd;              // eventfd 唤醒 IO 事件监控可能导致阻塞
    std::unique_ptr<Channel> _event_channel;
    EPoller _epoller;            // 对所有描述符的事件监控
    std::vector<Functor> _tasks; // 任务池
    std::mutex _mtx;             // 实现任务池操作安全
    TimerWheel _timer_wheel;     // 定时器模块
};

/*----------------------------------------------------------------------------------------------------------------*/

using ConnStatu = enum {
    DISCONNECTED = 0,
    CONNECTING,
    CONNECTED,
    DISCONNECTING,
};

class Connection : public std::enable_shared_from_this<Connection>
{
private:
    // 这四个回调函数是由服务器模块设置 -- 本质是使用者设置
    using ConnectionCallback = std::function<void(const std::shared_ptr<Connection> &)>;
    using MessageCallback = std::function<void(const std::shared_ptr<Connection> &, Buffer *)>;
    using ClosedCallback = std::function<void(const std::shared_ptr<Connection> &)>;
    using AnyCallback = std::function<void(const std::shared_ptr<Connection> &)>;

public:
    Connection(EventLoop *loop, uint64_t conn_id, int sockfd)
        : _conn_id(conn_id),
          _sockfd(sockfd),
          _enable_inactive_release(false),
          _loop(loop),
          _statu(CONNECTING),
          _socket(_sockfd),
          _channel(_sockfd, loop)
    {
        _channel.SetCloseCb(std::bind(&Connection::HandleClose, this));
        _channel.SetAnyCb(std::bind(&Connection::HandleAny, this));
        _channel.SetReadCb(std::bind(&Connection::HandleRead, this));
        _channel.SetWriteCb(std::bind(&Connection::HandleWrite, this));
        _channel.SetErrorCb(std::bind(&Connection::HandleError, this));
    }

    // 获取管理的文件描述符
    int Fd()
    {
        return _sockfd;
    }

    // 获取连接 ID
    int Id()
    {
        return _conn_id;
    }

    // 检查是否处于 CONNECTED 状态
    bool IsConnected()
    {
        return (_statu == CONNECTED);
    }

    // 设置上下文, 连接建立完成时调用
    bool SetContext(const std::any &context)
    {
        _context = context;
        return true;
    }

    // 获取上下文, 返回指针
    std::any *GetContext()
    {
        return &_context;
    }

    // 设置回调函数
    bool SetConnectedCallback(const ConnectionCallback &cb)
    {
        _conn_cb = cb;
        return true;
    }
    bool SetMessageCallback(const MessageCallback &cb)
    {
        _msg_cb = cb;
        return true;
    }
    bool SetClosedCallback(const ClosedCallback &cb)
    {
        _closed_cb = cb;
        return true;
    }
    bool SetSrvClosedCallback(const ClosedCallback &cb)
    {
        _server_closed_cb = cb;
        return true;
    }
    bool SetAnyCallback(const AnyCallback &cb)
    {
        _any_cb = cb;
        return true;
    }

    // 连接建立就绪后, 进行 channel 回调函数设置, 启动读监控, 调用 _conn_cb
    bool Established()
    {
        _loop->RunInLoop(std::bind(&Connection::EstablishedInLoop, this));
        return true;
    }

    // 发送数据, 将数据放入发送缓冲区, 并启动写事件监控
    bool Send(const char *data, size_t len)
    {
        // 外界传入 data 的指针可能把任务压入任务池, 等到处理时内存空间被销毁, 越权访问
        Buffer buf;
        buf.WriteAndPush(data, len);
        _loop->RunInLoop(std::bind(&Connection::SendInLoop, this, std::move(buf)));
        return true;
    }

    // 提供给组件使用者的关闭接口, 不是真实实际关闭
    void Shutdown()
    {
        _loop->RunInLoop(std::bind(&Connection::ShutdownInLoop, this));
    }

    // 提供给组件使用者的实际的释放接口 -- 内部使用
    void Release()
    {
        _loop->QueueInLoop(std::bind(&Connection::ReleaseInLoop, this));
    }

    // 启动非活跃销毁, 并定义时长, 添加任务
    void EnableInactiveRelease(int sec)
    {
        _loop->RunInLoop(std::bind(&Connection::EnableInactiveReleaseInLoop, this, sec));
    }

    // 取消非活跃销毁
    void CancelInactiveRelease()
    {
        _loop->RunInLoop(std::bind(&Connection::CancelInactiveReleaseInLoop, this));
    }

    // 切换协议, 重置上下文及回调函数, 这个接口必须在 EventLoop 线程中立即执行
    // 防备新事件触发后在处理时切换任务未执行导致使用原有协议处理数据
    void Upgrade(const std::any &context, const ConnectionCallback &conn, const MessageCallback &msg, const ClosedCallback &close, const AnyCallback &any)
    {
        _loop->AssertInLoop();
        _loop->RunInLoop(std::bind(&Connection::UpgradeInLoop, this, context, conn, msg, close, any));
    }

    ~Connection() = default;

private:
    // 五个 channel 的事件回调函数
    // 描述符可读事件触发后调用的函数, 接收 socket 数据放到接收缓冲区内, 然后调用 _msg_cb
    bool HandleRead()
    {
        // 接收socket的数据并放入到缓冲区
        char buffer[65536]; // 4kb
        ssize_t ret = _socket.NonBlockRecv(buffer, sizeof(buffer) - 1);
        if (ret < 0)
        {
            // 出错了, 但不能直接关闭连接
            ShutdownInLoop();
            return false;
        }
        else
        {
            // 0 : 没有读取到数据, 连接断开返回 -1
            // 将数据放入缓冲区, 并调整写偏移
            _in_buffer.WriteAndPush(buffer, ret);

            // 调用 _msg_cb 进行业务处理
            _msg_cb(shared_from_this(), &_in_buffer);
            return true;
        }
    }

    // 描述符触发可写事件后调用的函数, 将发送缓冲区的内容拷给 socket 进行发送
    bool HandleWrite()
    {
        // _out_buffer 中保存的就是要发送的数据
        std::optional<char *> readpos = _out_buffer.GetReadPos();
        if (readpos)
        {
            ssize_t ret = _socket.NonBlockSend(readpos.value(), _out_buffer.ReadAbleSize());
            if (ret < 0)
            {
                // 发送错误就关闭连接
                // 如果输入缓冲区还有数据就先处理
                if (_in_buffer.ReadAbleSize() > 0)
                {
                    _msg_cb(shared_from_this(), &_in_buffer);
                }

                // 实际真正的关闭释放
                Release();
                return false;
            }
            else
            {
                // 将读偏移向后移动
                _out_buffer.MoveReaderOffset(ret);
                if (_out_buffer.ReadAbleSize() == 0)
                {
                    // 没有数据发送, 关闭写事件监控
                    _channel.DisableWrite();

                    // 如果是连接待关闭状态则有数据, 处理完数据后释放连接, 没有数据直接释放
                    if (_statu == DISCONNECTED)
                    {
                        Release();
                    }
                }
                return true;
            }
        }
        else
        {
            return false;
        }
    }

    // 描述符触发挂断事件
    bool HandleClose()
    {
        // 连接挂断, 套接字无法通讯, 有数据就处理然后直接关闭连接
        if (_in_buffer.ReadAbleSize() > 0)
        {
            _msg_cb(shared_from_this(), &_in_buffer);
        }
        Release();
        return true;
    }

    // 描述符触发错误事件
    bool HandleError()
    {
        return HandleClose();
    }

    // 描述符触发任意事件
    bool HandleAny()
    {
        if (_enable_inactive_release)
        {
            _loop->TimerRefresh(_conn_id);
        }
        if (_any_cb)
        {
            _any_cb(shared_from_this());
        }
        return true;
    }

    // 获取连接之后, 所处的状态下要进行设置 -- 启动读监控, 调用回调函数
    bool EstablishedInLoop()
    {
        // 修改连接状态
        // 当前初始的状态必须是上层的半连接状态
        assert(_statu == CONNECTING);
        if (_statu != CONNECTING)
        {
            return false;
        }
        else
        {
            // 当前函数执行完毕, 连接进入已完成连接的状态
            _statu = CONNECTED;
        }

        // 启动读事件监控
        _channel.EnableRead();

        // 调用回调函数
        if (_conn_cb)
        {
            _conn_cb(shared_from_this());
        }
        return true;
    }

    // 实际的释放接口
    bool ReleaseInLoop()
    {
        // 修改连接状态为 DISCONNECTED
        _statu = DISCONNECTED;

        // 移除对所有事件的监控
        _channel.Remove();

        // 关闭描述符
        _socket.Close();

        // 如果有定时器任务则取消
        if (_loop->HasTimer(_conn_id))
        {
            _loop->TimerCancel(_conn_id);
        }

        // 调用关闭回调 -- 先用户后内部 -- 防止先移除内部信息导致 Connection 被释放, 导致用户回调出错
        if (_closed_cb)
        {
            _closed_cb(shared_from_this());
        }
        if (_server_closed_cb)
        {
            _server_closed_cb(shared_from_this());
        }
        return true;
    }

    // 不真实进行发送, 而是将数据放入发送缓冲区, 启动可写事件监控
    bool SendInLoop(Buffer &buf)
    {
        if (_statu != DISCONNECTED)
        {
            _out_buffer.WriteBufferAndPush(buf);
        }
        if (_channel.WriteAble() == false)
        {
            _channel.EnableWrite();
        }
        return true;
    }

    // 这个关闭操作不进行真正实际的释放连接, 判断是否有数据需要处理并改变状态
    bool ShutdownInLoop()
    {
        // 设置连接为半关闭状态
        _statu = DISCONNECTING;

        // 有数据就处理
        // 输入缓冲区
        if (_in_buffer.ReadAbleSize() > 0)
        {
            if (_msg_cb)
            {
                _msg_cb(shared_from_this(), &_in_buffer);
            }
        }

        // 输出缓冲区
        if (_out_buffer.ReadAbleSize() > 0)
        {
            if (_channel.WriteAble() == false)
            {
                _channel.EnableWrite();
            }
        }

        if (_out_buffer.ReadAbleSize() == 0)
        {
            Release();
        }
        return true;
    }

    // 启动非活跃连接超时释放规则
    bool EnableInactiveReleaseInLoop(int sec)
    {
        // 判断标志改为 true
        if (_enable_inactive_release == false)
        {
            _enable_inactive_release = true;
        }

        // 如果当前任务已存在, 刷新延迟
        if (_loop->HasTimer(_conn_id))
        {
            _loop->TimerRefresh(_conn_id);
        }
        // 不存在定时销毁任务则新增
        else
        {
            _loop->TimerAdd(_conn_id, sec, std::bind(&Connection::Release, this));
        }
        return true;
    }

    // 取消非活跃连接超时释放规则
    bool CancelInactiveReleaseInLoop()
    {
        // 判断标志改为 false
        _enable_inactive_release = false;

        // 删除定时销毁任务
        if (_loop->HasTimer(_conn_id))
        {
            _loop->TimerCancel(_conn_id);
        }
        return true;
    }

    // 更新//更换协议
    bool UpgradeInLoop(const std::any &context, const ConnectionCallback &conn, const MessageCallback &msg, const ClosedCallback &close, const AnyCallback &any)
    {
        _context = context;
        _conn_cb = conn;
        _msg_cb = msg;
        _closed_cb = close;
        _any_cb = any;
        return true;
    }

private:
    uint64_t _conn_id; // 连接的唯一 ID, 用于管理和查找
    // uint64_t _timer_id; // 定时器的唯一 ID, 用 _conn_id 即可
    int _sockfd;                   // 连接关联的文件描述符
    bool _enable_inactive_release; // 连接是否启动非活跃销毁, 默认 false 不启用
    EventLoop *_loop;              // 连接所关联的 EventLoop
    ConnStatu _statu;              // 连接状态
    Socket _socket;                // 套接字操作管理
    Channel _channel;              // 事件管理
    Buffer _in_buffer;             // 输入缓冲区, 存放从 socket 中读取到的数据
    Buffer _out_buffer;            // 输出缓冲区, 存放要向 socket 写入(发送给对端)的数据
    std::any _context;             // 请求的接收处理上下文

    ConnectionCallback _conn_cb;
    MessageCallback _msg_cb;
    ClosedCallback _closed_cb;
    AnyCallback _any_cb;
    ClosedCallback _server_closed_cb; // 组件内的连接关闭回调, 由组件内设置, 一旦某个连接关闭就要把组件内保存的相关信息移除
};

/*----------------------------------------------------------------------------------------------------------------*/

class Acceptor
{
private:
    using AcceptorCallback = std::function<void(int)>;

public:
    Acceptor(EventLoop *loop, int port)
        : _socket(CreateServer(port)),
          _loop(loop),
          _channel(_socket.Fd(), _loop)
    {
        _channel.SetReadCb(std::bind(&Acceptor::HandleRead, this));
    }

    // 设置 acceptor 的回调函数
    void SetAcceptCallback(const AcceptorCallback &cb)
    {
        _accept_cb = cb;
    }

    // 开始监听 -- 不能在构造函数进行, 此时还没有设置回调, 无法处理新连接
    void Listen()
    {
        _channel.EnableRead();
    }

private:
    // 监听套接字读事件回调处理函数, 获取新连接, 调用 _accept_cb 处理
    void HandleRead()
    {
        int newfd = _socket.Accept();
        if (newfd >= 0 && _accept_cb)
        {
            _accept_cb(newfd);
        }
    }

    int CreateServer(int port)
    {
        bool ret = _socket.CreateServer(port);
        assert(ret == true);
        if (ret == false)
        {
            return -1;
        }
        else
        {
            return _socket.Fd();
        }
    }

private:
    Socket _socket;   // 创建监听套接字
    EventLoop *_loop; // 监控监听套接字
    Channel _channel; // 管理监听套接字

    AcceptorCallback _accept_cb; // 处理新连接的回调函数
};

/*----------------------------------------------------------------------------------------------------------------*/

class LoopThread
{
public:
    // 创建线程, 设定线程入口函数
    LoopThread()
        : _loop(nullptr),
          _thread(std::thread(&LoopThread::ThreadEntry, this))
    {
    }

    // 返回当前线程关联的 EventLoop 对象指针
    EventLoop *GetLoop()
    {
        EventLoop *loop = nullptr;

        {
            std::unique_lock<std::mutex> lock(_mtx);
            _cond.wait(lock, [&]()
                       { return _loop != nullptr; });
            loop = _loop;
        }

        return loop;
    }

private:
    // 实例化 EventLoop 对象, 唤醒 _cond 上可能阻塞的线程, 开始运行 EventLoop 模块
    // 用于实现 _loop 获取的同步关系, 避免线程创建好但 _loop 没有实例化前获取 _loop
    void ThreadEntry()
    {
        EventLoop loop;

        {
            std::unique_lock<std::mutex> lock(_mtx);
            _loop = &loop;
            _cond.notify_all();
        }

        _loop->Start();
    }

private:
    std::mutex _mtx;
    std::condition_variable _cond;
    EventLoop *_loop;    // EventLoop 指针变量, 必须在线程内实例化
    std::thread _thread; // EventLoop 对应的线程
};

/*----------------------------------------------------------------------------------------------------------------*/

class LoopThreadPool
{
public:
    LoopThreadPool(EventLoop *baseloop)
        : _thread_count(0),
          _next_id(0),
          _baseloop(baseloop)
    {
    }

    // 设置子线程数量
    void SetThreadCount(int count)
    {
        _thread_count = count;
    }

    // 开始运行
    void Start()
    {
        if (_thread_count > 0)
        {
            _threads.resize(_thread_count);
            _loops.resize(_thread_count);
            for (size_t i = 0; i < _thread_count; ++i)
            {
                _threads[i] = new LoopThread();
                _loops[i] = _threads[i]->GetLoop();
            }
        }
    }

    // 返回进行工作的 EventLoop 的指针
    EventLoop *WorkLoop()
    {
        if (_thread_count == 0)
        {
            return _baseloop;
        }
        else
        {
            _next_id = (_next_id + 1) % _thread_count;
            return _loops[_next_id];
        }
    }

private:
    int _thread_count;                  // 线程数量计数
    int _next_id;                       // 下一个线程的 _conn_id(_timer_id)
    EventLoop *_baseloop;               // 主线程的 EventLoop
    std::vector<LoopThread *> _threads; // 记录子线程的指针
    std::vector<EventLoop *> _loops;    // 记录子线程的 EventLoop 的指针
};

/*----------------------------------------------------------------------------------------------------------------*/

class TcpServer
{
private:
    // 这四个回调函数是由服务器模块设置 -- 本质是使用者设置
    using ConnectionCallback = std::function<void(const std::shared_ptr<Connection> &)>;
    using MessageCallback = std::function<void(const std::shared_ptr<Connection> &, Buffer *)>;
    using ClosedCallback = std::function<void(const std::shared_ptr<Connection> &)>;
    using AnyCallback = std::function<void(const std::shared_ptr<Connection> &)>;
    using Functor = std::function<void()>;

private:
    // 为新连接构造一个 Connection 进行管理
    void NewConnection(int fd)
    {
        ++_next_id;
        std::shared_ptr<Connection> conn = std::make_shared<Connection>(_pool.WorkLoop(), _next_id, fd);
        conn->SetMessageCallback(_msg_cb);
        conn->SetClosedCallback(_closed_cb);
        conn->SetConnectedCallback(_conn_cb);
        conn->SetAnyCallback(_any_cb);
        conn->SetSrvClosedCallback(std::bind(&TcpServer::RemoveConnection, this, std::placeholders::_1));
        // 启动非活跃超时销毁
        if (_enable_inactive_release == true)
        {
            conn->EnableInactiveRelease(_timeout);
        }
        // 就绪初始化
        conn->Established();
        _conns.insert(std::make_pair(_next_id, conn));
    }

    // 服务器组件内部关闭连接要进行的回调函数
    bool RemoveConnectionInLoop(const std::shared_ptr<Connection> &conn)
    {
        int id = conn->Id();
        std::unordered_map<uint64_t, std::shared_ptr<Connection>>::iterator it = _conns.find(id);
        if (it != _conns.end())
        {
            _conns.erase(it);
        }
        return true;
    }

    // 从管理 Connection 的 _conns 中移除连接信息
    void RemoveConnection(const std::shared_ptr<Connection> &conn)
    {
        _baseloop.RunInLoop(std::bind(&TcpServer::RemoveConnectionInLoop, this, conn));
    }

    // 添加定时任务
    bool TimerAddInLoop(const Functor &task, int delayTime)
    {
        ++_next_id;
        _baseloop.TimerAdd(_next_id, delayTime, task);
        return true;
    }

public:
    TcpServer(int port)
        : _port(port),
          _next_id(0),
          _enable_inactive_release(false),
          _acceptor(&_baseloop, port),
          _pool(&_baseloop)
    {
        _acceptor.SetAcceptCallback(std::bind(&TcpServer::NewConnection, this, std::placeholders::_1));
        // 用 _baseloop 监控监听套接字
        _acceptor.Listen();
    }

    // 设置子线程数量
    void SetThreadCount(int count)
    {
        _pool.SetThreadCount(count);
    }

    // 设置回调函数
    bool SetConnectedCallback(const ConnectionCallback &cb)
    {
        _conn_cb = cb;
        return true;
    }
    bool SetMessageCallback(const MessageCallback &cb)
    {
        _msg_cb = cb;
        return true;
    }
    bool SetClosedCallback(const ClosedCallback &cb)
    {
        _closed_cb = cb;
        return true;
    }
    bool SetAnyCallback(const AnyCallback &cb)
    {
        _any_cb = cb;
        return true;
    }

    // 启动非活跃销毁, 并定义时长, 添加任务
    void EnableInactiveRelease(int timeout)
    {
        _timeout = timeout;
        _enable_inactive_release = true;
    }

    // 添加定时任务
    void TimerAdd(const Functor &task, int delayTime)
    {
        _baseloop.RunInLoop(std::bind(&TcpServer::TimerAddInLoop, this, task, delayTime));
    }

    void Start()
    {
        _pool.Start();
        _baseloop.Start();
    }

private:
    uint64_t _next_id;                                                // 递增的连接 ID
    int _port;                                                        // 端口号
    int _timeout;                                                     // 非活跃连接的最大时长
    bool _enable_inactive_release;                                    // 连接是否启动非活跃销毁, 默认 false 不启用
    EventLoop _baseloop;                                              // 主线程 EventLoop 对象, 处理监听事件
    Acceptor _acceptor;                                               // 监听套接字的管理对象
    LoopThreadPool _pool;                                             // 从属的 EventLoop 线程池
    std::unordered_map<uint64_t, std::shared_ptr<Connection>> _conns; // 保存管理的所有连接对应的 shared_ptr 对象

    ConnectionCallback _conn_cb;
    MessageCallback _msg_cb;
    ClosedCallback _closed_cb;
    AnyCallback _any_cb;
};

/*----------------------------------------------------------------------------------------------------------------*/

const std::string nullfile = "/dev/null";
// 忽略 SIGPIPE 信号(对端关闭、本端依旧发送数据会收到这个信号), 防止进程崩溃
class NetWork
{
public:
    NetWork()
    {
        signal(SIGPIPE, SIG_IGN);
        Daemon();
    }
    void Daemon (const std::string& cwd = ""){
        signal(SIGCLD, SIG_IGN);
        signal(SIGPIPE, SIG_IGN);
        signal(SIGSTOP, SIG_IGN);
    
        if(fork() > 0){
            exit(0);
        }
        setsid();
    
        if(!cwd.empty()){
            chdir(cwd.c_str());
        }
    
        int fd = open(nullfile.c_str(), O_RDWR);
        if(fd > 0){
            dup2(fd, 0);
            dup2(fd, 1);
            dup2(fd, 2);
            close(fd);
        }
    }
};

/*----------------------------------------------------------------------------------------------------------------*/

// Channel 移除监控
bool Channel::Remove()
{
    return _loop->RemoveEvent(this);
}
// Channel 创建/更新监控
bool Channel::CreateAndUpdate()
{
    return _loop->CreateAndUpdateEvent(this);
}

// TimerWheel 添加定时任务
void TimerWheel::TimerAdd(uint64_t id, uint32_t delayTime, const TaskFunc &task_cb)
{
    _loop->RunInLoop(std::bind(&TimerWheel::TimerAddInLoop, this, id, delayTime, task_cb));
}

// TimerWheel 刷新/延迟定时任务
void TimerWheel::TimerRefresh(uint64_t id)
{
    _loop->RunInLoop(std::bind(&TimerWheel::TimerRefreshInLoop, this, id));
}

// TimerWheel 取消定时任务
void TimerWheel::TimerCancel(uint64_t id)
{
    _loop->RunInLoop(std::bind(&TimerWheel::TimerCancelInLoop, this, id));
}

/*----------------------------------------------------------------------------------------------------------------*/

static NetWork nw;