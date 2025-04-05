#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>

#define LEFT "["
#define RIGHT "]"
#define LABLE "#"
#define DELIM " \t"
#define LINE_SIZE 1024
#define ARGC_SIZE 32
#define EXIT_CODE 44
#define NONE -1
#define IN_RDIR 0
#define OUT_RDIR 1
#define APPRND_RDIR 2
#define FILE_MODE 0666

int lastcode = 0;
int quit = 0;
char commandline[LINE_SIZE] = {NULL};
char* argv[ARGC_SIZE];
//extern char** environ;
char pwd[LINE_SIZE]; // 获取当前目录的缓冲区
char myenv[LINE_SIZE]; // 自定义环境变量表
// 自定义本地变量表
char *rdirfilename = NULL;
int rdir = NONE;

const char* GetUserName(){
  return getenv("USER");
}

const char* GetHostName(){
  return getenv("HOSTNAME");
}

void GetPwd(){
  getcwd(pwd, sizeof(pwd));
}

void Check_Redir(char* cmd){
  char* pos = cmd;
  while(*pos){
    if(*pos == '>'){
      if(*(pos + 1) == '>'){
        *pos++ = '\0';
        *pos++ = '\0';

        while(isspace(*pos)) pos++;

        rdirfilename = pos;
        rdir = APPRND_RDIR;
        break;
      }
      else{
        *pos++ = '\0';


        while(isspace(*pos)) pos++;

        rdirfilename = pos;
        rdir = OUT_RDIR;
        break;
      }
    }
    else if(*pos == '<'){
      *pos++ = '\0';
      
      while(isspace(*pos)) pos++;

      rdirfilename = pos;
      rdir = IN_RDIR;
      break;
    }
    pos++;
  }
}

void Interact(char* cline, int size){
  GetPwd();
  printf(LEFT"%s@%s %s"RIGHT""LABLE" ", GetUserName(), GetHostName(), pwd);

  char* str = fgets(cline, size, stdin);
  // 输入换行也有内容，读取不会出问题
  assert(str);
  // 编译器会对定义了但是未使用的变量报warning甚至error，release下assert无效，后续用不上str，这里用一下防止编译报错
  (void)str;
  // 不要回车\n
  cline[strlen(cline) - 1] = '\0';
  Cheeck_Redir(cline);
}

int SplitString(char cline[], char* _argv[]){
  int i = 0;
  _argv[i++] = strtok(cline, DELIM);
  while(argv[i++] = strtok(NULL, DELIM));
  return i - 1;
}

void NormalExcute(char* _argv[]){
    pid_t id = fork();
    if(id < 0){
      perror("fork");
    }
    else if(id == 0){
      int fd = 0;
      if(rdir == IN_RDIR){
        fd = open(rdirfilename, O_RDONLY);
        dup2(fd, 0);
      }
      else if(rdir == OUT_RDIR){
        fd = open(rdirfilename, O_CREAT|O_WRONLY|O_TRUNC, FILE_MODE);
        dup2(fd, 0);
      }
      else if(rdir == APPRND_RDIR){
        fd = open(rdirfilename, O_CREAT|O_WRONLY|O_APPEND, FILE_MODE);
        dup2(fd, 0);
      }

      //execvpe(_argv[0], _argv, environ);   
      execvp(_argv[0], _argv);
      exit(EXIT_CODE);
    }
    else{
      int status = 0;
      pid_t rid = waitpid(id, &status, 0);
      if(id == rid){
        lastcode = WEXITSTATUS(status);
      }
    }
}


int BuildCommand(char* _argv[], int _argc){
    if(strcmp(_argv[0], "ls") == 0){
      _argv[_argc++] = "--color";
      _argv[_argc] == NULL;
      return EXIT_CODE;
    }
    // 可以再实现ll是ls -l的别名

    if(_argc == 2 && strcmp(_argv[0], "cd") == 0){
      chdir(argv[1]);
      GetPwd();
      sprintf(getenv("PWD"), "%s", pwd);
      lastcode = 0;
      return 0;
    }
    else if(_argc == 2 && strcmp(_argv[0], "export") == 0){
      // 导入环境变量不是把字符串深拷贝到环境变量存储的地方，仅仅是把新增环境变量字符串的地址加入到存储环境变量的表中（在其中找一个没有被使用的位置）（指针数组）（类似于一种浅拷贝），因此不能随意修改新增环境变量存储的数组
      strcpy(myenv, _argv[1]); // 但是再导入一个旧的就没有了，这里不再更改，可以malloc一段空间拷贝进去，建立一个myenv的指针数组 -- 二维数组（要记录myenv的size -- 可以考虑struct封装）
      putenv(myenv);
      lastcode = 0;
      return 0;
    }
    else if(_argc == 2 && strcmp(_argv[0], "echo") == 0){
      if(strcmp(_argv[1], "$?") == 0){
        printf("%d\n", lastcode);
        lastcode = 0;
        return 0;
      }
      else if(*_argv[1] == '$'){
        char* ret = getenv(_argv[1] + 1);
        if(ret){
          printf("%s\n", ret);
        }
      }
      else{ // 不写检查""了，尾部处理类似\n，头部直接跳过
        printf("%s\n", _argv[1]);
      }
      lastcode = 0;
      return 0;
    }

    return EXIT_CODE;
}

int main(){
  while(!quit){
    // 1.初始化
    rdirfilename = NULL;
    rdir = NONE;

    // 2.交互、获取命令行
    Interact(commandline, sizeof(commandline));
    
    // 3.子串分割、解析命令行
    int argc = SplitString(commandline, argv);
    if(argc == 0) continue;

    // 4.指令判断
    // 4.1内建命令 -- shell内部直接调用函数 -- 已实现
    // 4.2管道 -- 未实现
    // 4.2.1.分析字符串有多少个 | 管道需要被实现，同时将命令拆分成多个子命令
    // 4.2.2.malloc一段空间保存pipe申请管道的fd，pipe申请多个管道
    // 4.2.3.循环创建多个子进程，更改每个子进程的重定向情况（第一个命令进程标准输出fd更改为第一个管道的写端，
    //       第二个命令进程标准输入fd更改为第一个管道的读端、标准输出fd改为第二个管道的写端（如果有）...依此类推）
    // 4.2.4.利用程序替换exec*系列函数让不同子进程执行不同代码（exec*系列函数仅进行代码数据的替换，不会更改文件描述符表、
    //       不改变原先修改好的重定向）
    int com = BuildCommand(argv, argc);
    
    // 5.普通命令的执行
    if(com){
      NormalExcute(argv);
    }
  }
  
  return 0;
}