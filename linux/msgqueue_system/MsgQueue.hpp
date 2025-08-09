#ifndef MSGQUEUE_HPP
#define MSGQUEUE_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define PATHNAME "/tmp"
#define PROJID 0x123

const int default_fd = -1;
const int defualt_size = 1024;

#define GET_MSGQUEUE (IPC_CREAT)
#define CREATE_MSGQUEUE (IPC_CREAT | IPC_EXCL | 0666)

class MsgQueue
{
    // 有类型数据块
    struct msgbuf
    {
        long mtype;
        char mtext[defualt_size];
    };

public:
    MsgQueue() : _msgfd(default_fd) {}
    // 创建消息队列
    void Create(int flag)
    {
        // 获取唯一的键值
        key_t key = ftok(PATHNAME, PROJID);
        if (key == -1)
        {
            std::cerr << "ftok error" << std::endl;
            exit(1);
        }
        // 按照16进制打印key
        std::cout << "key: " << std::hex << key << std::endl;
        // 创建消息队列 --- 暂定
        // IPC_CREAT:创建消息队列，如果不存在，创建之，否则，获取它
        // IPC_CREAT | IPC_EXCL(不单独使用): 若消息队列已存在，则返回错误，否则创建之。新的消息队列
        _msgfd = msgget(key, flag);
        if (_msgfd == -1)
        {
            std::cerr << "msgget error" << std::endl;
            exit(2);
        }
        std::cout << "msgqueue created: " << _msgfd << std::endl;
    }
    // 发送消息
    void Send(int type, const std::string &text)
    {
        struct msgbuf msg;
        memset(&msg, 0, sizeof(msg));
        msg.mtype = type;
        memcpy(msg.mtext, text.c_str(), text.size());
        // 问题：不能填写成为sizeof(msg)
        int n = msgsnd(_msgfd, &msg, sizeof(msg.mtext), 0);
        if (n == -1)
        {
            std::cerr << "msgsnd error" << std::endl;
            return;
        }
    }
    // 接受消息，参数设置成输出型参数
    void Recv(int type, std::string &text)
    {
        struct msgbuf msg;
        int n = msgrcv(_msgfd, &msg, sizeof(msg.mtext), type, 0);
        if (n == -1)
        {
            std::cerr << "msgrcv error" << std::endl;
            return;
        }
        msg.mtext[n] = '\0';
        text = msg.mtext;
    }
    // 获取消息队列中的属性
    void GetAttr()
    {
        struct msqid_ds outbuffer;
        int n = msgctl(_msgfd, IPC_STAT, &outbuffer);
        if (n == -1)
        {
            std::cerr << "msgctl error" << std::endl;
            return;
        }
        std::cout << "outbuffer.msg_perm.__key: " << std::hex << outbuffer.msg_perm.__key << std::endl;
    }
    // 删除消息队列
    void Destroy()
    {
        int n = msgctl(_msgfd, IPC_RMID, 0);
        if (n == -1)
        {
            std::cerr << "msgctl error" << std::endl;
            return;
        }
        std::cout << "msgqueue destroyed" << std::endl;
    }
    ~MsgQueue() {}

private:
    int _msgfd;
};

// 我们需要定义消息类型
#define MSG_TYPE_CLIENT 1
#define MSG_TYPE_SERVER 2

class Server : public MsgQueue
{
public:
    Server()
    {
        MsgQueue::Create(CREATE_MSGQUEUE);
        std::cout << "server create msgqueue done" << std::endl;
        MsgQueue::GetAttr();
    }
    ~Server()
    {
        MsgQueue::Destroy();
    }
};

class Client : public MsgQueue
{
public:
    Client()
    {
        MsgQueue::Create(GET_MSGQUEUE);
        std::cout << "client get msgqueue done" << std::endl;
    }
    ~Client()
    {
    }
};

#endif // MSGQUEUE_HPP