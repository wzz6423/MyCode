#ifndef SEM_HPP
#define SEM_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

const std::string SEM_PATH = "/tmp";
const int SEM_PROJ_ID = 0x77;
const int defaultnum = 1;
#define GET_SEM (IPC_CREAT)
#define BUILD_SEM (IPC_CREAT | IPC_EXCL)

// 一个把整数转十六进制的函数
std::string intToHex(int num)
{
    char hex[64];
    snprintf(hex, sizeof(hex), "0x%x", num);
    return std::string(hex);
}

// 产品类, 只需要关心自己的使用即可(删除)
// 这里的Semaphore不是一个信号量, 而是一个信号量集合, 要指明要PV操作哪一个信号量
class Semaphore
{
private:
    void PV(int who, int data)
    {
        struct sembuf sem_buf;
        sem_buf.sem_num = who;      // 信号量编号,从0开始
        sem_buf.sem_op = data;      // S + sem_buf.sem_op
        sem_buf.sem_flg = SEM_UNDO; // 不关心
        int n = semop(_semid, &sem_buf, 1);
        if (n < 0)
        {
            std::cerr << "semop PV failed" << std::endl;
        }
    }

public:
    Semaphore(int semid) : _semid(semid)
    {
    }
    int Id() const
    {
        return _semid;
    }
    void P(int who)
    {
        PV(who, -1);
    }
    void V(int who)
    {
        PV(who, 1);
    }
    ~Semaphore()
    {
        if (_semid >= 0)
        {
            int n = semctl(_semid, 0, IPC_RMID);
            if (n < 0)
            {
                std::cerr << "semctl IPC_RMID failed" << std::endl;
            }
            std::cout << "Semaphore " << _semid << " removed" << std::endl;
        }
    }

private:
    int _semid;
    // key_t _key; // 信号量集合的键值
    // int _perm;  // 权限
    // int _num;   // 信号量集合的个数
};

// 建造者接口
class SemaphoreBuilder
{
public:
    virtual ~SemaphoreBuilder() = default;
    virtual void BuildKey() = 0;
    virtual void SetPermission(int perm) = 0;
    virtual void SetSemNum(int num) = 0;
    virtual void SetInitVal(std::vector<int> initVal) = 0;
    virtual void Build(int flag) = 0;
    virtual void InitSem() = 0;
    virtual std::shared_ptr<Semaphore> GetSem() = 0;
};

// 具体建造者类
class ConcreteSemaphoreBuilder : public SemaphoreBuilder
{
public:
    ConcreteSemaphoreBuilder() {}
    virtual void BuildKey() override
    {
        // 1. 构建键值
        std::cout << "Building a semaphore" << std::endl;
        _key = ftok(SEM_PATH.c_str(), SEM_PROJ_ID);
        if (_key < 0)
        {
            std::cerr << "ftok failed" << std::endl;
            exit(1);
        }
        std::cout << "Got key: " << intToHex(_key) << std::endl;
    }
    virtual void SetPermission(int perm) override
    {
        _perm = perm;
    }
    virtual void SetSemNum(int num) override
    {
        _num = num;
    }
    virtual void SetInitVal(std::vector<int> initVal) override
    {
        _initVal = initVal;
    }
    virtual void Build(int flag) override
    {
        // 2. 创建信号量集合
        int semid = semget(_key, _num, flag | _perm);
        if (semid < 0)
        {
            std::cerr << "semget failed" << std::endl;
            exit(2);
        }
        std::cout << "Got semaphore id: " << semid << std::endl;
        _sem = std::make_shared<Semaphore>(semid);
    }
    virtual void InitSem() override
    {
        if (_num > 0 && _initVal.size() == _num)
        {
            // 3. 初始化信号量集合
            for (int i = 0; i < _num; i++)
            {
                if (!Init(_sem->Id(), i, _initVal[i]))
                {
                    std::cerr << "Init failed" << std::endl;
                    exit(3);
                }
            }
        }
    }
    virtual std::shared_ptr<Semaphore> GetSem() override
    {
        return _sem;
    }

private:
    bool Init(int semid, int num, int val)
    {
        union semun
        {
            int val;               /* Value for SETVAL */
            struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
            unsigned short *array; /* Array for GETALL, SETALL */
            struct seminfo *__buf; /* Buffer for IPC_INFO
                                      (Linux-specific) */
        } un;
        un.val = val;
        int n = semctl(semid, num, SETVAL, un);
        if (n < 0)
        {
            std::cerr << "semctl SETVAL failed" << std::endl;
            return false;
        }
        return true;
    }

private:
    key_t _key;                      // 信号量集合的键值
    int _perm;                       // 权限
    int _num;                        // 信号量集合的个数
    std::vector<int> _initVal;       // 初始值
    std::shared_ptr<Semaphore> _sem; // 要创建的具体产品
};

// 指挥者类
class Director
{
public:
    void Construct(std::shared_ptr<SemaphoreBuilder> builder, int flag, int perm = 0666, int num = defaultnum, std::vector<int> initVal = {1})
    {
        builder->BuildKey();
        builder->SetPermission(perm);
        builder->SetSemNum(num);
        builder->SetInitVal(initVal);
        builder->Build(flag);
        if (flag == BUILD_SEM)
        {
            builder->InitSem();
        }
    }
};

#endif // SEM_HPP