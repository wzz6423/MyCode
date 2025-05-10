/*
    实现抽象格式化子项基类
    实现派生格式化子项子类  -- 时间, 日志等级, 源码文件名, 行号, 线程ID, 日志器名称, 日志内容 -- 有效载荷, 制表符, 换行符, 其它
*/

#pragma once

// C++
#include <iostream>
#include <string>
#include <vector>
#include <memory>
// C
#include <ctime>
#include <cassert>
// Other
#include "util.hpp"
#include "message.hpp"

namespace Log
{
    // 抽象格式化子项基类
    class FormatItem
    {
    public:
        using formatPtr = std::shared_ptr<FormatItem>;
        virtual void Format(std::ostream &out, const LogMessage &msg) = 0;
    };

    // 派生格式化子项子类
    // 时间
    class TimeFormatItem : public FormatItem
    {
    public:
        TimeFormatItem(const std::string time_fmt = "%Y-%m-%d %H:%M:S")
            : _time_fmt(time_fmt)
        {
        }

        void Format(std::ostream &out, const LogMessage &msg) override
        {
            // 1.直接转换
            // struct tm t;
            // localtime_r(&msg._ctime, &t);
            // char fmtTime[32] = {0};
            // strftime(fmtTime, sizeof(fmtTime) - 1, _time_fmt.c_str(), &t);

            // 2.使用 util 提供的方法
            std::string fmtTime = Util::Date::FormatExistTime(msg._ctime);

            out << fmtTime;
        }

    private:
        std::string _time_fmt; // %Y-%m-%d %H:%M:S
    };

    // 日志等级
    class LevelFormatItem : public FormatItem
    {
    public:
        void Format(std::ostream &out, const LogMessage &msg) override
        {
            std::optional<std::string> msgstr = LogLevel::LevelToStdString(msg._level);
            if (msgstr)
            {
                out << msgstr.value();
            }
        }
    };

    // 源码文件名
    class FilenameFormatItem : public FormatItem
    {
    public:
        void Format(std::ostream &out, const LogMessage &msg) override
        {
            out << msg._filename;
        }
    };

    // 行号
    class LineFormatItem : public FormatItem
    {
    public:
        void Format(std::ostream &out, const LogMessage &msg) override
        {
            out << msg._line;
        }
    };

    // 线程ID
    class ThreadIdFormatItem : public FormatItem
    {
    public:
        void Format(std::ostream &out, const LogMessage &msg) override
        {
            out << msg._id;
        }
    };

    // 日志器名称
    class LoggerFormatItem : public FormatItem
    {
    public:
        void Format(std::ostream &out, const LogMessage &msg) override
        {
            out << msg._logger;
        }
    };

    // 日志内容 -- 有效载荷
    class MsgFormatItem : public FormatItem
    {
    public:
        void Format(std::ostream &out, const LogMessage &msg) override
        {
            out << msg._payload;
        }
    };

    // 制表符
    class TableFormatItem : public FormatItem
    {
    public:
        void Format(std::ostream &out, const LogMessage &msg) override
        {
            out << "\t";
        }
    };

    // 换行符
    class NewLineFormatItem : public FormatItem
    {
    public:
        void Format(std::ostream &out, const LogMessage &msg) override
        {
            out << "\n";
        }
    };

    // 其它
    class OtherFormatItem : public FormatItem
    {
    public:
        OtherFormatItem(const std::string &tag)
            : _tag(tag)
        {
        }

        void Format(std::ostream &out, const LogMessage &msg) override
        {
            out << _tag;
        }

    private:
        std::string _tag;
    };

    // 组织格式化日志 -- [%D{%Y-%m-%d %H:%M:S}][%t][%c][%f:%l][%p]%T%m%n
    // %D: 日期 -- %Y-%m-%d %H:%M:S+
    // %t: 线程Id
    // %c: 日志器名称
    // %f: 源码文件名
    // %l: 行号
    // %p: 日志级别
    // %T: 制表符
    // %m: 日志内容 -- 有效载荷
    // %n: 换行符
    class Formatter
    {
    public:
        using formatterPtr = std::shared_ptr<Formatter>;
        Formatter(const std::string& pattern = "[%D{%Y-%m-%d %H:%M:S}][%t][%c][%f:%l][%p]%T%m%n")
            : _pattern(pattern)
        {
            bool ret = ParsePattern();
            assert(ret);
        }

        // 格式化 msg
        void Format(std::ostream &out, const LogMessage &msg)
        {
            for (auto &item : _items)
            {
                item->Format(out, msg);
            }
        }

        std::string Format(const LogMessage &msg)
        {
            std::stringstream ss;
            Format(ss, msg);
            return ss.str();
        }

    private:
        // 解析 pattern 字符串的格式化子项
        bool ParsePattern()
        {
            // 解析格式化规则字符串
            std::vector<std::pair<std::string, std::string>> fmtOrder;
            size_t pos = 0;
            std::string key;
            std::string val;
            while (pos < _pattern.size())
            {
                // 处理原始字符串 -- 是否为 % , 不是则为原始字符
                if (_pattern[pos] != '%')
                {
                    val.push_back(_pattern[pos++]);
                    continue;
                }

                // 是 % , 如果为 %% 则为一个 % 原始字符
                if (pos + 1 < _pattern.size() && _pattern[pos + 1] == '%')
                {
                    val.push_back('%');
                    pos += 2;
                    continue;
                }

                // val 不为空, % 后是格式化字符, 原始字符串处理完毕
                if (!val.empty())
                {
                    fmtOrder.push_back(std::make_pair("", val));
                    val.clear();
                }

                // 格式化字符处理
                if (++pos == _pattern.size())
                {
                    std::cerr << "% 后无对应格式化字符..." << std::endl;
                    return false;
                }
                key = _pattern[pos];

                // pos 指向格式化字符串后的位置
                ++pos;

                // 子规则处理
                if (pos < _pattern.size() && _pattern[pos] == '{')
                {
                    // pos 指向子规则的起始位置
                    ++pos;

                    while (pos < _pattern.size() && _pattern[pos] != '}')
                    {
                        val.push_back(_pattern[pos++]);
                    }

                    // 直到结尾也没有找到 } -- 格式错误
                    if (pos == _pattern.size() || _pattern[pos] != '}')
                    {
                        std::cerr << "子规则 {} 匹配错误..." << std::endl;
                        return false;
                    }

                    // pos 走到 } 的下一个位置进行新一轮的循环处理
                    ++pos;
                }

                fmtOrder.push_back(std::make_pair(key, val));
                key.clear();
                val.clear();
            }

            // 根据解析得到的数据初始化格式化子项数组成员
            for (auto &e : fmtOrder)
            {
                _items.push_back(CreateItem(e.first, e.second));
            }

            return true;
        }

        // 根据不同的格式化字符串创建不同的格式化子项对象
        FormatItem::formatPtr CreateItem(const std::string &key, const std::string &val)
        {
            if (key == "D")
            {
                return std::make_shared<TimeFormatItem>(val);
            }
            else if (key == "t")
            {
                return std::make_shared<ThreadIdFormatItem>();
            }
            else if (key == "c")
            {
                return std::make_shared<LoggerFormatItem>();
            }
            else if (key == "f")
            {
                return std::make_shared<FilenameFormatItem>();
            }
            else if (key == "l")
            {
                return std::make_shared<LineFormatItem>();
            }
            else if (key == "p")
            {
                return std::make_shared<LevelFormatItem>();
            }
            else if (key == "T")
            {
                return std::make_shared<TableFormatItem>();
            }
            else if (key == "m")
            {
                return std::make_shared<MsgFormatItem>();
            }
            else if (key == "n")
            {
                return std::make_shared<NewLineFormatItem>();
            }
            else if (key.empty() || key == "")
            {
                return std::make_shared<OtherFormatItem>(val);
            }
            else
            {
                std::cerr << "不存在对应的 % 格式化处理方法..." << std::endl;
                assert(false);
            }
        }

    private:
        std::string _pattern;                      // 格式化规则字符串 -- [%D{%Y-%m-%d %H:%M:S}][%t][%c][%f:%l][%p]%T%m%n
        std::vector<FormatItem::formatPtr> _items; // 根据不同的格式化字符串创建的不同的格式化子项对象
    };
}