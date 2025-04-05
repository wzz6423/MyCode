// 函数形参编码规范
// 输入：const &
// 输出：*
// 输入输出：&

#include "Task.hpp"

// child process num
const int processnum = 5;
// error enum
enum
{
    PIPE_CREATE_ERR = 1,
    WAIT_ERR
};
// tasks queue
std::vector<task_t> tasks;

// 描述子进程池
class channel
{
public:
    channel(int cmdfd, pid_t slaverid, const std::string &processname)
        : _cmdfd(cmdfd), _slaverid(slaverid), _processname(processname)
    {
    }

public:
    int _cmdfd = 0;                           // 发送任务的文件描述符
    pid_t _slaverid = 0;                      // 子进程的pid // slaver -> worker也行
    std::string _processname = std::string(); // 子进程的名字 -- 日志
};

// 子进程任务
void slaver()
{
    while (true)
    {
        int cmdcode = 0;
        int n = read(0, &cmdcode, sizeof(int));
        if (n == sizeof(int))
        {
            std::cout << "Slaver say@ get a command: " << getpid() << " : cmdcode : " << cmdcode << std::endl;
            if (cmdcode > 0 && cmdcode < tasks.size())
            {
                tasks[cmdcode - 1]();
            }
        }
        else if (n == 0)
        {
            break;
        }
    }
}

// 初始化子进程
void InitProcessPool(std::vector<channel> *channels)
{
    std::vector<int> oldfds;
    for (int i = 0; i < processnum; ++i)
    {
        int pipefd[2];
        int n = pipe(pipefd);
        if (n < 0)
        {
            exit(PIPE_CREATE_ERR);
        }

        pid_t id = fork();
        if (id == 0)
        {
            // child
            std::cout << "child : " << getpid() << " close history pipe_fd";
            for (auto &fd : oldfds)
            {
                std::cout << fd << " ";
                close(fd);
            }
            std::cout << std::endl;

            close(pipefd[1]);
            dup2(pipefd[0], 0);
            close(pipefd[0]);
            slaver();

            std::cout << "Child process quit :" << getpid() << std::endl;

            exit(0);
        }
        else if (id < 0)
        {
            std::cerr << "Child process create faile..." << std::endl;
        }

        // parent
        close(pipefd[0]);
        
        std::string name = "process-" + std::to_string(i);
        channels->push_back(channel(pipefd[1], id, name));
        oldfds.push_back(pipefd[1]);

        sleep(1);
    }
}

void CtrlSlaver(const std::vector<channel> &channels)
{
    int which = 5;
    while(which--){
        int cmdcode = rand() % tasks.size();
        int process = rand() % channels.size();

        write(channels[process]._cmdfd, &cmdcode, sizeof(cmdcode));
        sleep(1);
    }
}

void QuitProcess(const std::vector<channel> &channels)
{
    for(auto& prc : channels){
        close(prc._cmdfd);
        waitpid(prc._slaverid, nullptr, 0);
    }
}

int main()
{
    LoadTask(&tasks);

    srand(time(nullptr) ^ getpid() ^ 1024);

    // 组织子进程池
    std::vector<channel> channels;

    // 初始化
    InitProcessPool(&channels);

    // 控制
    CtrlSlaver(channels);

    // 清理
    QuitProcess(channels);

    return 0;
}