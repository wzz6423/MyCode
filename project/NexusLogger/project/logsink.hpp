/*
    日志落地模块的实现:
        1.实现抽象落地基类
        2.派生子类(根据不同落地方向进行派生)
        3.使用工厂模式进行创建与表示的分离
*/

#pragma once

// C++
#include <iostream>
#include <sstream>
#include <memory>
#include <filesystem>
// C
#include <cassert>
#include <cstring>
#include <cerrno>
// Other
#include "util.hpp"

namespace Log
{
    // 抽象落地基类
    class LogSink
    {
    public:
        using sinkPtr = std::shared_ptr<LogSink>;

        // 落地函数
        virtual void Log(const char *data, size_t len) = 0;

        // 析构虚函数 -- 使用者要创造更多落地子类可能会用到析构函数
        virtual ~LogSink() {}
    };

    // 落地标准输出
    class StdoutSink : public LogSink
    {
    public:
        // 将日志消息写入到标准输出
        void Log(const char *data, size_t len)
        {
            std::cout.write(data, len);
        }
    };

    // 落地指定文件
    class FileSink : public LogSink
    {
    public:
        // 构造时传入文件名并打开文件, 管理文件操作符/句柄
        FileSink(const std::string &path)
            : _path(path)
        {
            int cnt = 6;
            bool flag = false;
            while (--cnt && !flag)
            {
                // 检查目录和文件是否存在, 如果不存在则创建
                if (!Util::File::IsFileExists(Util::File::ExtractFilename(_path), Util::File::ExtractDirectory(_path)))
                {
                    if (!Util::File::CreateFile(_path))
                    {
                        if (cnt)
                        {
                            continue;
                        }
                        else
                        {
                            std::cerr << "文件创建失败..." << std::endl;
                            assert(false);
                        }
                    }
                }

                // 打开文件并将文件操作句柄赋给 _ofs
                _ofs.open(_path, std::ios::binary | std::ios::app);
                if (!_ofs.is_open())
                {
                    if (cnt)
                    {
                        continue;
                    }
                    else
                    {
                        std::cerr << "文件打开失败..." << std::endl;
                        assert(false);
                    }
                }

                flag = true;
            }
        }

        // 将日志消息写入到指定文件
        void Log(const char *data, size_t len)
        {
            _ofs.write(data, len);
            assert(_ofs.good());
        }

    private:
        std::string _path;  // 文件存储路径 & 文件名
        std::ofstream _ofs; // 文件操作句柄
    };

    enum class FileSize
    {
        KB,
        MB,
        HALF_GB,
        GB
    };

    // 落地滚动文件 -- 大小
    class RollBySizeFileSink : public LogSink
    {
    public:
        // 构造时传入文件名并打开文件, 管理文件操作符/句柄
        // 默认文件最大大小: 1GB
        RollBySizeFileSink(const std::string &basename, FileSize filesize = FileSize::GB)
            : _basename(basename)
        {
            switch (filesize)
            {
            case FileSize::KB:
                _max_file_size = 1024;
                break;
            case FileSize::MB:
                _max_file_size = 1024 * 1024;
                break;
            case FileSize::HALF_GB:
                _max_file_size = 512 * 1024 * 1024;
                break;
            case FileSize::GB:
                _max_file_size = 1024 * 1024 * 1024;
                break;
            }

            bool ret = SwitchNewFile();
            if (!ret)
            {
                assert(false);
            }
        }

        // 将日志消息写入到指定滚动文件 -- 写入前判断文件大小, 超过最大大小就切换文件
        void Log(const char *data, size_t len)
        {
            if (_cur_file_size + len >= _max_file_size)
            {
                bool ret = SwitchNewFile();
                if (!ret)
                {
                    assert(false);
                }
            }
            _ofs.write(data, len);
            _cur_file_size += len;
            assert(_ofs.good());
        }

    private:
        // 创建文件并将文件句柄切换到新文件
        bool SwitchNewFile()
        {
            int cnt = 6;
            bool flag = false;
            while (--cnt && !flag)
            {
                // 获取格式化的当前时间
                std::string nowTime = Util::Date::FormatTime("%Y_%m_%d_%H_%M_%S");

                // 得到实际文件名
                std::string filename;
                filename += _basename;
                filename += nowTime;
                filename += "_" + std::to_string(_name_count);
                filename += ".log";

                // 检查目录和文件是否存在, 如果不存在则创建
                if (!Util::File::IsFileExists(Util::File::ExtractFilename(filename), Util::File::ExtractDirectory(filename)))
                {
                    if (!Util::File::CreateFile(filename))
                    {
                        if (cnt)
                        {
                            continue;
                        }
                        else
                        {
                            std::cerr << "文件创建失败..." << std::endl;
                            return false;
                        }
                    }
                }

                // 检查 _ofs 是否已经打开过文件, 打开过就进行关闭
                if (_ofs.is_open())
                {
                    _ofs.close();
                }

                // 打开文件并将文件操作句柄赋给 _ofs
                _ofs.open(filename, std::ios::binary | std::ios::app);
                if (!_ofs.is_open())
                {
                    if (cnt)
                    {
                        continue;
                    }
                    else
                    {
                        std::cerr << "文件打开失败..." << std::endl;
                        return false;
                    }
                }

                flag = true;
            }

            ++_name_count;
            _cur_file_size = 0;
            return true;
        }

    private:
        std::string _basename;     // 文件存储路径 & 基础文件名 -- 基础文件名 + 扩展文件名(时间) = 实际输出文件名
        std::ofstream _ofs;        // 文件操作句柄
        size_t _max_file_size;     // 文件最大大小
        size_t _cur_file_size = 0; // 文件当前大小
        size_t _name_count = 0;    // 文件名计数器, 如果一秒内写入过多内容则会导致文件名冲突, 故加入一个文件名计数器
    };

    enum class TimeGap
    {
        SECOND,
        MINUTE,
        HOUR,
        DAY,
        WEEK,
        MOUTH
    };

    // 落地滚动文件 -- 时间
    // 实现思想一: 传入时间段, 与保存的上一次创建文件的时间相加, 大于当前时间就创建新文件
    // 实现思想二: 传入时间段, 保存上一次创建文件时的时间对该时间段的取模的值, 再次取模时如果值发生变化就创建新文件 -- 注意如果是秒时任何数取模1都是0
    class RollByTimeFileSink : public LogSink
    {
    public:
        // 构造时传入文件名并打开文件, 管理文件操作符/句柄
        // 默认切换时间: 1天
        // 默认文件最大大小: 1GB
        RollByTimeFileSink(const std::string &basename, TimeGap timegap = TimeGap::DAY, FileSize filesize = FileSize::GB)
            : _basename(basename)
        {
            switch (timegap)
            {
            case TimeGap::SECOND:
                _switch_time = 1;
                break;
            case TimeGap::MINUTE:
                _switch_time = 60;
                break;
            case TimeGap::HOUR:
                _switch_time = 60 * 60;
                break;
            case TimeGap::DAY:
                _switch_time = 24 * 60 * 60;
                break;
            case TimeGap::WEEK:
                _switch_time = 7 * 24 * 60 * 60;
                break;
            case TimeGap::MOUTH:
                _switch_time = 30 * 7 * 24 * 60 * 60;
                break;
            }

            switch (filesize)
            {
            case FileSize::KB:
                _max_file_size = 1024;
                break;
            case FileSize::MB:
                _max_file_size = 1024 * 1024;
                break;
            case FileSize::HALF_GB:
                _max_file_size = 512 * 1024 * 1024;
                break;
            case FileSize::GB:
                _max_file_size = 1024 * 1024 * 1024;
                break;
            }

            bool ret = SwitchNewFile();
            if (!ret)
            {
                assert(false);
            }
        }

        // 将日志消息写入到指定滚动文件
        // 写入前判断文件大小, 超过最大大小就切换文件
        // 写入前判断是否超时, 超时就切换文件
        void Log(const char *data, size_t len)
        {
            if ((_cur_file_size + len >= _max_file_size) || (_last_creat_time + _switch_time <= Util::Date::NowTime()))
            {
                bool ret = SwitchNewFile();
                if (!ret)
                {
                    assert(false);
                }
            }
            _ofs.write(data, len);
            _cur_file_size += len;
            assert(_ofs.good());
        }

    private:
        // 创建文件并将文件句柄切换到新文件
        bool SwitchNewFile()
        {
            int cnt = 6;
            bool flag = false;
            while (--cnt && !flag)
            {
                // 获取格式化的当前时间
                std::string nowTime = Util::Date::FormatTime("%Y_%m_%d_%H_%M_%S");

                // 得到实际文件名
                std::string filename;
                filename += _basename;
                filename += nowTime;
                filename += "_" + std::to_string(_name_count);
                filename += ".log";

                // 检查目录和文件是否存在, 如果不存在则创建
                if (!Util::File::IsFileExists(Util::File::ExtractFilename(filename), Util::File::ExtractDirectory(filename)))
                {
                    if (!Util::File::CreateFile(filename))
                    {
                        if (cnt)
                        {
                            continue;
                        }
                        else
                        {
                            std::cerr << "文件创建失败..." << std::endl;
                            return false;
                        }
                    }
                }

                // 检查 _ofs 是否已经打开过文件, 打开过就进行关闭
                if (_ofs.is_open())
                {
                    _ofs.close();
                }

                // 打开文件并将文件操作句柄赋给 _ofs
                _ofs.open(filename, std::ios::binary | std::ios::app);
                if (!_ofs.is_open())
                {
                    if (cnt)
                    {
                        continue;
                    }
                    else
                    {
                        std::cerr << "文件打开失败..." << std::endl;
                        return false;
                    }
                }

                flag = true;
            }

            ++_name_count;
            _cur_file_size = 0;
            _last_creat_time = Util::Date::NowTime();
            return true;
        }

    private:
        std::string _basename;     // 文件存储路径 & 基础文件名 -- 基础文件名 + 扩展文件名(时间) = 实际输出文件名
        std::ofstream _ofs;        // 文件操作句柄
        size_t _max_file_size;     // 文件最大大小
        size_t _cur_file_size = 0; // 文件当前大小
        size_t _last_creat_time;   // 记录上一次创建文件的时间
        size_t _switch_time;       // 经过多长时间切换文件
        size_t _name_count = 0;    // 文件名计数器, 如果一秒内写入过多内容则会导致文件名冲突, 故加入一个文件名计数器
    };

    // 日志落地工厂 -- 简单工厂模式
    class SinkFactory
    {
    public:
        // 如果定义 enum class 使用 switch 来区分使用哪种落地方式, 在后续进行扩展时要修改此处源码, 违反开闭原则, 故使用模板
        // 不同落地方式构造所需要的参数数量不同, 故使用可变参数包
        template <typename SinkType, typename... Args>
        static LogSink::sinkPtr Create(Args &&...args)
        {
            return std::make_shared<SinkType>(std::forward<Args>(args)...);
        }
    };
}