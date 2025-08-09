#include "sem.hpp"
#include <unistd.h>
#include <ctime>
#include <cstdio>

int main()
{
    // 基于抽象接口类的具体建造者
    std::shared_ptr<SemaphoreBuilder> builder = std::make_shared<ConcreteSemaphoreBuilder>();
    // 指挥者对象
    std::shared_ptr<Director> director = std::make_shared<Director>();

    // 在指挥者的指导下，完成建造过程
    director->Construct(builder, BUILD_SEM, 0600, 3, {1, 2, 3});

    // 完成了对象的创建的过程，获取对象
    auto fsem = builder->GetSem();

    // sleep(10);

    // SemaphoreBuilder sb;
    // auto fsem = sb.SetVar(1).build(BUILD_SEM, 1);
    srand(time(0) ^ getpid());
    pid_t pid = fork();

    // 期望的是，父子进行打印的时候，C或者F必须成对出现！保证打印是原子的.
    if (pid == 0)
    {
        director->Construct(builder, GET_SEM);
        auto csem = builder->GetSem();
        while (true)
        {
            // csem->P(0);
            printf("C");
            usleep(rand() % 95270);
            fflush(stdout);
            printf("C");
            usleep(rand() % 43990);
            fflush(stdout);
            // csem->V(0);
        }
    }
    while (true)
    {
        // fsem->P(0);
        printf("F");
        usleep(rand() % 95270);
        fflush(stdout);
        printf("F");
        usleep(rand() % 43990);
        fflush(stdout);
        // fsem->V(0);
    }

    return 0;
}