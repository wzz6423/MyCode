#ifndef CHAIN_OF_RESPONSIBILITY_HPP
#define CHAIN_OF_RESPONSIBILITY_HPP

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <filesystem> // C++17
#include <fstream>
#include <ctime>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

// 责任链基类
class HandlerText
{
public:
    virtual void Excute(const std::string &text) = 0;
    void SetNext(std::shared_ptr<HandlerText> next)
    {
        _next = next;
    }
    void Enable()
    {
        _enable = true;
    }
    void Disable()
    {
        _enable = false;
    }
    virtual ~HandlerText()
    {
    }

protected:                              // protected需要被子类继承
    std::shared_ptr<HandlerText> _next; // 下一个责任链节点
    bool _enable = true;                // 是否启用该节点
};

// 对文本进行格式化处理的
class HandlerTextFormat : public HandlerText
{
public:
    void Excute(const std::string &text) override
    {
        std::string format_result = text + "\n";
        if (_enable)
        {
            // 该节点被开启，对文本进行格式化处理
            std::stringstream ss;
            ss << time(nullptr) << "-" << getpid() << "-" << text << "\n";
            format_result = ss.str();
            std::cout << "step 1: 格式化消息: " << text << " 结果: " << format_result << std::endl;
        }
        if (_next)
        {
            _next->Excute(format_result); // 将处理结果，表现在text内部，传递给下一个节点
        }
        else
        {
            std::cout << "到达责任链处理结尾,完成责任链处理" << std::endl;
        }
    }
};

// 文件的基本信息: 文件路径，文件名称
std::string defaultfilepath = "./tmp/";
std::string defaultfilename = "test.log";

// 对文本进行文件保存
class HandlerTextSaveFile : public HandlerText
{
public:
    HandlerTextSaveFile(const std::string &filepath = defaultfilepath,
                        const std::string &filename = defaultfilename)
        : _filepath(filepath), _filename(filename)
    {
        // 形成默认的目录名, filesystem
        if (std::filesystem::exists(_filepath))
            return;
        try
        {
            std::filesystem::create_directories(_filepath);
        }
        catch (std::filesystem::filesystem_error const &e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    void Excute(const std::string &text) override
    {
        if (_enable)
        {
            // 保存到文件中
            std::string file = _filepath + _filename;
            std::ofstream ofs(file, std::ios::app);
            if (!ofs.is_open())
            {
                std::cerr << "open file error: " << file << std::endl;
                return;
            }
            ofs << text;
            ofs.close();
            std::cout << "step 2: 保存消息: " << text << " 到文件: " << file << std::endl;
        }
        if (_next)
        {
            _next->Excute(text); // 将处理结果，表现在text内部，传递给下一个节点
        }
        else
        {
            std::cout << "到达责任链处理结尾,完成责任链处理" << std::endl;
        }
    }

private:
    std::string _filepath;
    std::string _filename;
};

const int defaultmaxline = 5; // 最大行数, 这里比较短，方便测试

// 对文件内容长度进行检查，如果长度过长，对文件内容进行打包备份
class HandlerTextBackup : public HandlerText
{
public:
    HandlerTextBackup(const std::string &filepath = defaultfilepath,
                      const std::string &filename = defaultfilename,
                      const int &maxline = defaultmaxline)
        : _filepath(filepath), _filename(filename), _maxline(maxline)
    {
    }
    void Excute(const std::string &text) override
    {
        if (_enable)
        {
            // 该节点被开启，对文件进行检查，如果超范围，我们就要切片，并且进行打包备份
            std::string file = _filepath + _filename;
            std::cout << "Step 3: 检查文件: " << file << " 大小是否超范围" << std::endl;

            if (IsOutOfRange(file))
            {
                // 如果超了范围，进行切片备份
                std::cout << "目标文件超范围，进行切片备份" << file << std::endl;
                Backup(file);
            }
        }
        if (_next)
        {
            _next->Excute(text); // 将处理结果，表现在text内部，传递给下一个节点
        }
        else
        {
            std::cout << "到达责任链处理结尾,完成责任链处理" << std::endl;
        }
    }

private:
    bool IsOutOfRange(const std::string &file)
    {
        std::ifstream ifs(file);
        if (!ifs.is_open())
        {
            std::cerr << "open file eeor: " << file << std::endl;
            return false;
        }
        int lines = 0;
        std::string line;
        while (std::getline(ifs, line))
        {
            lines++;
        }
        ifs.close();

        return lines > _maxline;
    }

    void Backup(const std::string &file)
    {
        // 1589234234
        std::string suffix = std::to_string(time(nullptr));
        // "./tmp/test.txt" -> "./tmp/test.txt.1589234234"
        std::string backup_file = file + "." + suffix; // 备份文件名
        // 只需要文件名, 不需要路径, test.txt.1589234234
        std::string src_file = _filename + "." + suffix;
        // test.txt.1589234234.tgz
        std::string tar_file = src_file + ".tgz";

        // 切片备份并打包
        pid_t pid = fork();
        if (pid == 0)
        {
            // child
            // 1. 先对文件进行重名，Linux上，对文件名进行重命名是原子的.
            // "./tmp/test.txt" -> "./tmp/test.txt.1589234234"
            // 2. 我们想让子进程进行数据备份
            std::filesystem::rename(file, backup_file);
            std::cout << "step 4: 备份文件: " << file << " 到文件: " << backup_file << std::endl;
            // 3. 对备份文件进行打包，打包成为.tgz, 需要使用exec*系统调用
            // 3.1 对备份文件进行打包.tgz
            // "./tmp/test.txt" -> "./tmp/test.txt.1589234234" -> "./tmp/test.txt.1589234234.tgz"
            // 3.1.1 更改工作路径
            std::filesystem::current_path(_filepath);
            // 3.1.2 调用tar命令进行打包
            execlp("tar", "tar", "-czf", tar_file.c_str(), src_file.c_str(), nullptr);
            exit(1); // exec*系统调用失败，返回1
        }

        // parent
        int status;
        pid_t rid = waitpid(pid, &status, 0);
        if (rid > 0)
        {
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
            {
                // 打包成功，删除源文件
                std::filesystem::remove(backup_file);
                std::cout << "step 5: 删除备份文件: " << backup_file << std::endl;
            }
        }
    }

private:
    std::string _filepath;
    std::string _filename;
    int _maxline; // 最大行数
};

// 责任链入口类
class HandlerEntry
{
public:
    HandlerEntry()
    {
        // 构造责任链节点
        _format = std::make_shared<HandlerTextFormat>();
        _save = std::make_shared<HandlerTextSaveFile>();
        _backup = std::make_shared<HandlerTextBackup>();

        // 设置责任链节点处理顺序
        _format->SetNext(_save);
        _save->SetNext(_backup);
    }
    void EnableHandler(bool isformat, bool issave, bool isbackup)
    {
        isformat ? _format->Enable() : _format->Disable();
        issave ? _save->Enable() : _save->Disable();
        isbackup ? _backup->Enable() : _backup->Disable();
    }
    void Run(const std::string &text)
    {
        _format->Excute(text);
    }
    ~HandlerEntry()
    {
    }

private:
    std::shared_ptr<HandlerText> _format;
    std::shared_ptr<HandlerText> _save;
    std::shared_ptr<HandlerText> _backup;
};

#endif // CHAIN_OF_RESPONSIBILITY_HPP