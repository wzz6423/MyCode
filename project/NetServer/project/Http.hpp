#pragma once

/*----------------------------------------------------------------------------------------------------------------*/

// c++
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <optional>
#include <vector>
#include <regex>

// C
#include <cstdio>

// system call
#include <sys/stat.h>

// other
#include "Server.hpp"

/*----------------------------------------------------------------------------------------------------------------*/

std::unordered_map<int, std::string> _statu_mag = {
    {100, "Continue"},
    {101, "Switching Protocol"},
    {102, "Processing"},
    {103, "Early Hints"},
    {200, "OK"},
    {201, "Created"},
    {202, "Accepted"},
    {203, "Non-Authoritative Information"},
    {204, "No Content"},
    {205, "Reset Content"},
    {206, "Partial Content"},
    {207, "Multi-Status"},
    {208, "Already Reported"},
    {226, "IM Used"},
    {300, "Multiple Choice"},
    {301, "Moved Permanently"},
    {302, "Found"},
    {303, "See Other"},
    {304, "Not Modified"},
    {305, "Use Proxy"},
    {306, "unused"},
    {307, "Temporary Redirect"},
    {308, "Permanent Redirect"},
    {400, "Bad Request"},
    {401, "Unauthorized"},
    {402, "Payment Required"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {405, "Method Not Allowed"},
    {406, "Not Acceptable"},
    {407, "Proxy Authentication Required"},
    {408, "Request Timeout"},
    {409, "Conflict"},
    {410, "Gone"},
    {411, "Length Required"},
    {412, "Precondition Failed"},
    {413, "Payload Too Large"},
    {414, "URI Too Long"},
    {415, "Unsupported Media Type"},
    {416, "Range Not Satisfiable"},
    {417, "Expectation Failed"},
    {418, "I'm a teapot"},
    {421, "Misdirected Request"},
    {422, "Unprocessable Entity"},
    {423, "Locked"},
    {424, "Failed Dependency"},
    {425, "Too Early"},
    {426, "Upgrade Required"},
    {428, "Precondition Required"},
    {429, "Too Many Requests"},
    {431, "Request Header Fields Too Large"},
    {451, "Unavailable For Legal Reasons"},
    {501, "Not Implemented"},
    {502, "Bad Gateway"},
    {503, "Service Unavailable"},
    {504, "Gateway Timeout"},
    {505, "HTTP Version Not Supported"},
    {506, "Variant Also Negotiates"},
    {507, "Insufficient Storage"},
    {508, "Loop Detected"},
    {510, "Not Extended"},
    {511, "Network Authentication Required"}};

std::unordered_map<std::string, std::string> _mime_msg = {
    {".aac", "audio/aac"},
    {".abw", "application/x-abiword"},
    {".arc", "application/x-freearc"},
    {".avi", "video/x-msvideo"},
    {".azw", "application/vnd.amazon.ebook"},
    {".bin", "application/octet-stream"},
    {".bmp", "image/bmp"},
    {".bz", "application/x-bzip"},
    {".bz2", "application/x-bzip2"},
    {".csh", "application/x-csh"},
    {".css", "text/css"},
    {".csv", "text/csv"},
    {".doc", "application/msword"},
    {".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
    {".eot", "application/vnd.ms-fontobject"},
    {".epub", "application/epub+zip"},
    {".gif", "image/gif"},
    {".htm", "text/html"},
    {".html", "text/html"},
    {".ico", "image/vnd.microsoft.icon"},
    {".ics", "text/calendar"},
    {".jar", "application/java-archive"},
    {".jpeg", "image/jpeg"},
    {".jpg", "image/jpeg"},
    {".js", "text/javascript"},
    {".json", "application/json"},
    {".jsonld", "application/ld+json"},
    {".mid", "audio/midi"},
    {".midi", "audio/x-midi"},
    {".mjs", "text/javascript"},
    {".mp3", "audio/mpeg"},
    {".mpeg", "video/mpeg"},
    {".mpkg", "application/vnd.apple.installer+xml"},
    {".odp", "application/vnd.oasis.opendocument.presentation"},
    {".ods", "application/vnd.oasis.opendocument.spreadsheet"},
    {".odt", "application/vnd.oasis.opendocument.text"},
    {".oga", "audio/ogg"},
    {".ogv", "video/ogg"},
    {".ogx", "application/ogg"},
    {".otf", "font/otf"},
    {".png", "image/png"},
    {".pdf", "application/pdf"},
    {".ppt", "application/vnd.ms-powerpoint"},
    {".pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
    {".rar", "application/x-rar-compressed"},
    {".rtf", "application/rtf"},
    {".sh", "application/x-sh"},
    {".svg", "image/svg+xml"},
    {".swf", "application/x-shockwave-flash"},
    {".tar", "application/x-tar"},
    {".tif", "image/tiff"},
    {".tiff", "image/tiff"},
    {".ttf", "font/ttf"},
    {".txt", "text/plain"},
    {".vsd", "application/vnd.visio"},
    {".wav", "audio/wav"},
    {".weba", "audio/webm"},
    {".webm", "video/webm"},
    {".webp", "image/webp"},
    {".woff", "font/woff"},
    {".woff2", "font/woff2"},
    {".xhtml", "application/xhtml+xml"},
    {".xls", "application/vnd.ms-excel"},
    {".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
    {".xml", "application/xml"},
    {".xul", "application/vnd.mozilla.xul+xml"},
    {".zip", "application/zip"},
    {".3gp", "video/3gpp"},
    {".3g2", "video/3gpp2"},
    {".7z", "application/x-7z-compressed"}};

/*----------------------------------------------------------------------------------------------------------------*/

class Util
{
public:
    // 字符串分割函数, 将 src 字符串按照 sep 字符进行分割, 得到的各个子串放入 array 中, 最终返回切分出来的子串的数量
    static size_t Split(const std::string &src, const std::string &sep, std::vector<std::string> *array)
    {
        size_t offset = 0;

        while (offset < src.size())
        {
            size_t pos = src.find(sep, offset);
            if (pos == std::string::npos)
            {
                // 将剩余部分作为一个子串尾插
                array->push_back(src.substr(offset));
                return array->size();
            }
            // 当前子串为空, 仅有被用来进行分割的字符, 没有有效内容
            if (pos == offset)
            {
                offset = pos + sep.size();
                continue;
            }

            // 正常情况, 切分子串并尾插
            array->push_back(src.substr(offset, pos - offset));
            offset = pos + sep.size();
        }
        return array->size();
    }

    // 读取文件所有内容并放在一个 string 内
    static bool ReadFile(const std::string &filename, std::string *buf)
    {
        std::ifstream ifs(filename, std::ios::binary); // 二进制方式 -- 保证数据完全原始
        if (ifs.is_open() == false)
        {
            Error_Log("open %s file error!\n", filename.c_str());
            return false;
        }
        else
        {
            size_t fsize = 0;
            ifs.seekg(0, ifs.end); // 跳转读写位置到文件末尾处
            fsize = ifs.tellg();   // 获取当前读写位置相对于起始位置的偏移量, 此处的偏移量就是文件的大小
            ifs.seekg(0, ifs.beg); // 跳转回文件起始位置进行读取
            buf->resize(fsize);    // 开辟文件大小的空间用于缓冲区使用
            ifs.read(&(*buf)[0], fsize);
            if (ifs.good() == false)
            {
                Error_Log("read %s file error!\n", filename.c_str());
                ifs.close();
                return false;
            }
            else
            {
                ifs.close();
                return true;
            }
        }
    }

    // 从一个 string 的缓冲区中读取数据并写入文件
    static bool WriteFile(const std::string &filename, const std::string &buf)
    {
        std::ofstream ofs(filename, std::ios::binary | std::ios::trunc); // 二进制 + 清空原有内容
        if (ofs.is_open() == false)
        {
            Error_Log("open %s file erroe!\n", filename.c_str());
            return false;
        }
        else
        {
            ofs.write(buf.c_str(), buf.size());
            if (ofs.good() == false)
            {
                Error_Log("write %s error!\n", filename.c_str());
                ofs.close();
                return false;
            }
            else
            {
                ofs.close();
                return true;
            }
        }
    }

    // url 编码, 避免 url 中资源路径和查询字符串中的特殊字符与 HTTP 请求中特殊字符产生歧义
    // 编码格式: 将特殊字符的 ASCII 码值转换为两个 16 进制字符, 加上前缀 % : 如 C++ -> C%2B%2B
    // 不编码的特殊字符, RFC3986 文档规定: 【 . _ - ~ 字母、 数字】 属于绝对不编码字符
    // W3C 文档中规定: 查询字符串中的空格需要编码为 + , 解码是 + 转空格
    static std::string UrlEncode(const std::string url, bool convert_space_to_plus = false)
    {
        std::string res;
        for (auto &c : url)
        {
            if (c == '.' || c == '-' || c == '_' || c == '~')
            {
                res += c;
                continue;
            }
            else if (c == ' ' && convert_space_to_plus == true)
            {
                res += '+';
                continue;
            }
            else
            {
                // 其余字符均要被编码为 %HH 格式
                char tmp[4] = {0};
                // snprintf 与 printf 类似, 均为格式化字符串, printf 是打印到标准输出, snprintf 可以指定打印目标
                snprintf(tmp, 4, "%%%02X", c);
                res += tmp;
            }
        }
        return res;
    }

    // 16 进制转 10 进制
    static std::optional<char> HEXtoI(char c)
    {
        if (c >= '0' && c <= '9')
        {
            return c - '0';
        }
        else if (c >= 'a' && c <= 'z')
        {
            return c - 'a' + 10;
        }
        else if (c >= 'A' && c <= 'Z')
        {
            return c - 'A' + 10;
        }
        else
        {
            return std::nullopt;
        }
    }

    // url 解码
    static std::optional<std::string> UrlDecode(const std::string &url, bool convert_space_to_plus = false)
    {
        // 遇见 % 则将其后两个字符转换为数字, 第 1 个数字左移 4 位, 加上第 2 个数字
        std::string res;
        for (size_t i = 0; i < url.size(); ++i)
        {
            if (url[i] == '+' && convert_space_to_plus == true)
            {
                res += ' ';
                continue;
            }
            else if (url[i] == '%' && (i + 2) < url.size())
            {
                std::optional<char> c1 = HEXtoI(url[i + 1]);
                std::optional<char> c2 = HEXtoI(url[i + 2]);
                if (c1 && c2)
                {
                    char ch = c1.value() << 4 + c2.value();
                    res += ch;
                    i += 2;
                    continue;
                }
                else
                {
                    return std::nullopt;
                }
            }
            else
            {
                res += url[i];
            }
        }
        return res;
    }

    // 响应状态码的对应描述信息获取
    static std::string StatuToDesc(int statu)
    {
        std::unordered_map<int, std::string>::iterator it = _statu_mag.find(statu);
        if (it == _statu_mag.end())
        {
            return "Unknow";
        }
        else
        {
            return it->second;
        }
    }

    // 根据文件后缀获取文件 mime
    static std::string ExtMime(const std::string &filename)
    {
        // 先获取文件后缀
        size_t pos = filename.find_last_of('.');
        if (pos == std::string::npos)
        {
            // 二进制流
            return "application/octet-stream";
        }
        else
        {
            // 根据后缀获取 mime
            std::string ext = filename.substr(pos);
            std::unordered_map<std::string, std::string>::iterator it = _mime_msg.find(ext);
            if (it == _mime_msg.end())
            {
                // 二进制流
                return "application/octet-stream";
            }
            else
            {
                return it->second;
            }
        }
    }

    // 判断一个文件是否是目录
    static bool IsDirectory(const std::string &isDir)
    {
        struct stat st;
        int ret = stat(isDir.c_str(), &st);
        if (ret < 0)
        {
            return false;
        }
        else
        {
            return S_ISDIR(st.st_mode);
        }
    }

    // 判断一个文件是否是普通文件
    static bool IsFile(const std::string &isFile)
    {
        struct stat st;
        int ret = stat(isFile.c_str(), &st);
        if (ret < 0)
        {
            return false;
        }
        else
        {
            return S_ISREG(st.st_mode);
        }
    }

    // HTTP 请求资源路径有效性判断
    // /index.html 的 / 叫作相对根目录, 映射的是某台服务器上的某个子目录
    // 不能访问没有权限访问的目录及文件如 /../../xxx
    static bool ValidPath(const std::string &path)
    {
        // 按照 / 进行路径分割, 根据路径计算目录深度, 遇到 .. 就--, 否则 ++ , 合理路径不应小于 0
        std::vector<std::string> subPath;
        Split(path, "/", &subPath);
        int level = 0;
        for (auto &p : subPath)
        {
            if (p == "..")
            {
                --level; // 任意一层走回上级目录就认为是有问题的
                if (level < 0)
                {
                    return false;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                ++level;
            }
        }
        return true;
    }
};

/*----------------------------------------------------------------------------------------------------------------*/

class HttpRequest
{
public:
    HttpRequest()
        : _version("HTTP/1.1")
    {
    }

    // 清空所有数据
    void ReSet()
    {
        _method.clear();
        _path.clear();
        _version = "HTTP/1.1";
        _body.clear();
        std::smatch tmpMatches;
        _matches.swap(tmpMatches);
        _headers.clear();
        _params.clear();
    }

    // 插入头部字段
    void SetHeader(const std::string &key, const std::string &val)
    {
        _headers.insert(std::make_pair(key, val));
    }

    // 判断是否存在指定头部字段
    bool HasHeader(const std::string &key) const
    {
        std::unordered_map<std::string, std::string>::const_iterator it = _headers.find(key);
        if (it == _headers.end())
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    // 获取指定头部字段的值
    std::optional<std::string> GetHeaderVal(const std::string &key) const
    {
        std::unordered_map<std::string, std::string>::const_iterator it = _headers.find(key);
        if (it == _headers.end())
        {
            return std::nullopt;
        }
        else
        {
            return it->second;
        }
    }

    // 插入查询字符串
    void SetParam(const std::string &key, const std::string &val)
    {
        _params.insert(std::make_pair(key, val));
    }

    // 判断是否有某个指定的查询字符串
    bool HasParam(const std::string &key) const
    {
        std::unordered_map<std::string, std::string>::const_iterator it = _params.find(key);
        if (it == _params.end())
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    // 获取指定的查询字符串
    std::optional<std::string> GetParamVal(const std::string &key) const
    {
        std::unordered_map<std::string, std::string>::const_iterator it = _params.find(key);
        if (it == _params.end())
        {
            return std::nullopt;
        }
        else
        {
            return it->second;
        }
    }

    // 获取正文长度
    size_t ContentLength() const
    {
        bool ret = HasHeader("Content-Length");
        if (ret == false)
        {
            return 0;
        }
        else
        {
            std::optional<std::string> clen = GetHeaderVal("Content-Length");
            if (clen)
            {
                return std::stoi(clen.value());
            }
            else
            {
                return false;
            }
        }
    }

    // 判断是否是短连接
    std::optional<bool> Close() const
    {
        // 没有 Connection 字段或者值为 close , 则为短连接, 否则是长连接
        std::optional<std::string> isClose = GetHeaderVal("Connection");
        if (isClose)
        {
            if (HasHeader("Connection") == true && isClose.value() == "keep-alive")
            {
                return false;
            }
            else
            {
                return std::nullopt;
            }
        }
        else
        {
            return true;
        }
    }

public:
    std::string _method;                                   // 请求方法
    std::string _path;                                     // 请求路径
    std::string _version;                                  // Http 协议版本
    std::string _body;                                     // 请求正文
    std::smatch _matches;                                  // 资源路径的正则提取数据
    std::unordered_map<std::string, std::string> _headers; // 头部字段
    std::unordered_map<std::string, std::string> _params;  // 查询字符串
};

/*----------------------------------------------------------------------------------------------------------------*/

class HttpResponse
{
public:
    HttpResponse()
        : _redirect_flag(false),
          _statu(200) // OK
    {
    }

    HttpResponse(int statu)
        : _redirect_flag(false),
          _statu(statu)
    {
    }

    // 清空内容
    void ReSet()
    {
        _statu = 200;
        _redirect_flag = false;
        _body.clear();
        _redirect_url.clear();
        _headers.clear();
    }

    // 插入头部字段
    void SetHeader(const std::string &key, const std::string &val)
    {
        _headers.insert(std::make_pair(key, val));
    }

    // 判断是否存在指定头部字段
    bool HasHeader(const std::string &key) const
    {
        std::unordered_map<std::string, std::string>::const_iterator it = _headers.find(key);
        if (it == _headers.end())
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    // 获取指定头部字段的值
    std::optional<std::string> GetHeaderVal(const std::string &key) const
    {
        std::unordered_map<std::string, std::string>::const_iterator it = _headers.find(key);
        if (it == _headers.end())
        {
            return std::nullopt;
        }
        else
        {
            return it->second;
        }
    }

    // 设置正文
    void SetContent(const std::string &body, const std::string &type = "text/html")
    {
        _body = body;
        SetHeader("Content-Type", type);
    }

    // 设置重定向 url
    void SetRedirect(const std::string &url, int statu = 302) // 临时重定向状态码 -- 提供这个形参可以设置为永久重定向
    {
        _statu = statu;
        _redirect_flag = true;
        _redirect_url = url;
    }

    // 判断是否是短连接
    std::optional<bool> Close() const
    {
        // 没有 Connection 字段或者值为 close , 则为短连接, 否则是长连接
        std::optional<std::string> isClose = GetHeaderVal("Connection");
        if (isClose)
        {
            if (HasHeader("Connection") == true && isClose.value() == "keep-alive")
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            return true;
        }
    }

public:
    int _statu;                                            // 状态码
    bool _redirect_flag;                                   // 是否进行重定向
    std::string _body;                                     // 正文
    std::string _redirect_url;                             // 重定向 url (若有)
    std::unordered_map<std::string, std::string> _headers; // 头部
};

/*----------------------------------------------------------------------------------------------------------------*/

using HttpRecvStatu = enum {
    RECV_HTTP_ERROR = 0,
    RECV_HTTP_LINE,
    RECV_HTTP_HEAD,
    RECV_HTTP_BODY,
    RECV_HTTP_OVER
};

#define MAX_LINE 8192
class HttpContext
{
public:
    HttpContext()
        : _resp_statu(200), // OK
          _recv_statu(RECV_HTTP_LINE)
    {
    }

    // 清空数据
    void ReSet()
    {
        _resp_statu = 200;
        _recv_statu = RECV_HTTP_LINE;
        _request.ReSet();
    }

    // 获取当前 Http 响应状态码
    int RespStatu()
    {
        return _resp_statu;
    }

    // 获取当前 Http 接收及解析的阶段状态标记
    HttpRecvStatu RecvStatu()
    {
        return _recv_statu;
    }

    // 获取当前已经解析得到的请求信息
    HttpRequest &Request()
    {
        return _request;
    }

    // 接收并解析 Http 请求
    void RecvHttpRequest(Buffer *buf)
    {
        // 带上 while(_recv_statu != RECV_HTTP_OVER){} ?
        // 不同的状态做不同的事情但不需要 break , 因为是按照顺序排的, 处理完上一项应当继续向后处理请求而非直接退出等待新数据
        switch (_recv_statu)
        {
        case RECV_HTTP_LINE:
            RecvHttpLine(buf);
        case RECV_HTTP_HEAD:
            RecvHttpHead(buf);
        case RECV_HTTP_BODY:
            RecvHttpBody(buf);
        }
    }

private:
    // 接收 Http 请求行
    bool RecvHttpLine(Buffer *buf)
    {
        if (_recv_statu != RECV_HTTP_LINE)
        {
            return false;
        }
        else
        {
            // 获取一行数据, 带有末尾换行
            std::optional<std::string> line = buf->GetLineAndPop();
            if (!line)
            {
                return false;
            }
            else
            {
                std::string rline = line.value();

                // 缓冲区内容不足一行或一行数据过大
                if (rline.size() == 0)
                {
                    // 缓冲区数据不足一行 --> 判读缓冲区可读数据长度, 很长也不足一行 -- 错误
                    if (buf->ReadAbleSize() > MAX_LINE)
                    {
                        _recv_statu = RECV_HTTP_ERROR;
                        _resp_statu = 414; // URL TOO LONG
                        return false;
                    }
                    else
                    {
                        // 缓冲区内数据不足一行, 等新数据到来
                        return true;
                    }
                }
                // 读取到的数据过长 -- 错误
                else if (rline.size() > MAX_LINE)
                {
                    _recv_statu = RECV_HTTP_ERROR;
                    _resp_statu = 414; // URL TOO LONG
                    return false;
                }
                else
                {
                    bool ret = ParseHttpLine(rline);
                    if (ret == false)
                    {
                        return false;
                    }
                    else
                    {
                        // 首行处理完毕, 进入头部获取阶段
                        _recv_statu = RECV_HTTP_HEAD;
                        return true;
                    }
                }
            }
        }
    }

    // 利用正则表达式解析 Http 请求行
    bool ParseHttpLine(const std::string &line)
    {
        std::smatch matches;
        std::string url = line;
        url.erase(std::remove(url.begin(), url.end(), '\r'), url.end());
        url.erase(std::remove(url.begin(), url.end(), '\n'), url.end());
        std::regex urlEncodeRule(R"((GET|HEAD|POST|PUT|DELETE)\s+([^?\s]+)(?:\?([^\s]*))?\s+(HTTP\/1\.[01]))");
        // R"((GET|HEAD|POST|PUT|DELETE) ([^\?]*)(?:\?(.*))? (HTTP\/1\.[01])(?:\n|\r\n)?)"
        bool ret = std::regex_match(url, matches, urlEncodeRule);
        if (ret == false)
        {
            _recv_statu = RECV_HTTP_ERROR;
            _resp_statu = 400;          // BAD REQUEST
            assert(false);
            return false;
        }
        else
        {
            // 解析出来的格式
            // 0 : 全部 url 请求行内容
            // 1 : 请求方法
            // 2 : url 请求资源路径
            // 3 : 查询参数
            // 4 : 协议版本

            // 请求方法获取
            _request._method = matches[1];
            std::transform(_request._method.begin(), _request._method.end(), _request._method.begin(), ::toupper);

            // 资源路径的获取 -- 要进行 url 解码, 不需要 + 转空格
            std::optional<std::string> ret = Util::UrlDecode(matches[2], false);
            if (ret)
            {
                if (ret.value() == "/favicon.ico")
                {
                    _resp_statu = 204;
                    return true;
                }
                else
                {
                    _request._path = ret.value();
                }
            }
            else
            {
                _recv_statu = RECV_HTTP_ERROR;
                _resp_statu = 400; // BAD REQUEST
                return false;
            }

            // 查询资源路径的字符串获取与处理
            std::vector<std::string> query_string_array;
            std::string query_string = matches[3];
            // 查询字符串的格式: key=value&key=val...
            // 先以 & 进行分割, 得到各个子串
            Util::Split(query_string, "&", &query_string_array);
            // 针对各个子串以 = 进行分割也需要进行 url 解码得到 key 和 val
            for (auto &str : query_string_array)
            {
                size_t pos = str.find("=");
                if (pos == std::string::npos)
                {
                    _recv_statu = RECV_HTTP_ERROR;
                    _resp_statu = 400; // BAD REQUEST
                    return false;
                }
                else
                {
                    std::optional<std::string> key = Util::UrlDecode(str.substr(0, pos), true);
                    std::optional<std::string> val = Util::UrlDecode(str.substr(pos + 1), true);
                    if (key && val)
                    {
                        _request.SetParam(key.value(), val.value());
                    }
                    else
                    {
                        _recv_statu = RECV_HTTP_ERROR;
                        _resp_statu = 400; // BAD REQUEST
                        return false;
                    }
                }
            }

            // 获取协议版本
            _request._version = matches[4];

            return true;
        }
    }

    // 接收 Http 请求头
    bool RecvHttpHead(Buffer *buf)
    {
        if (_recv_statu != RECV_HTTP_HEAD)
        {
            return false;
        }
        else
        {
            // 一行一行取出数据, 直到遇到空行 : 头部的格式为 key: val\r\nkey: val\r\n...
            while (true)
            {
                std::optional<std::string> line = buf->GetLineAndPop();
                if (!line)
                {
                    return false;
                }
                else
                {
                    std::string rline = line.value();

                    // 缓冲区内容不足一行或一行数据过大
                    if (rline.size() == 0)
                    {
                        // 缓冲区数据不足一行 --> 判读缓冲区可读数据长度, 很长也不足一行 -- 错误
                        if (buf->ReadAbleSize() > MAX_LINE)
                        {
                            _recv_statu = RECV_HTTP_ERROR;
                            _resp_statu = 414; // URL TOO LONG
                            return false;
                        }
                        else
                        {
                            // 缓冲区内数据不足一行, 等新数据到来
                            return true;
                        }
                    }
                    // 读取到的数据过长 -- 错误
                    else if (rline.size() > MAX_LINE)
                    {
                        _recv_statu = RECV_HTTP_ERROR;
                        _resp_statu = 414; // URL TOO LONG
                        return false;
                    }
                    else
                    {
                        if (rline == "\n" || rline == "\r\n")
                        {
                            break;
                        }
                        else
                        {
                            bool ret = ParseHttpHead(rline);
                            if (ret == false)
                            {
                                return false;
                            }
                        }
                    }
                }
            }
            _recv_statu = RECV_HTTP_BODY;
            return true;
        }
    }

    // 解析 Http 请求头
    bool ParseHttpHead(std::string &line)
    {
        // key: val\r\nkey: val\r\n...

        // 格式控制
        // 去除末尾换行符
        if (line.back() == '\n')
        {
            line.pop_back();
        }
        // 去除末尾回车符
        if (line.back() == '\r')
        {
            line.pop_back();
        }

        // 解析头
        size_t pos = line.find(": ");
        if (pos == std::string::npos)
        {
            _recv_statu = RECV_HTTP_ERROR;
            _resp_statu = 400; // NOT FOUND
            return false;
        }
        else
        {
            std::string key = line.substr(0, pos);
            std::string val = line.substr(pos + 1);
            _request.SetHeader(key, val);
            return true;
        }
    }

    // 接收 Http 正文
    bool RecvHttpBody(Buffer *buf)
    {
        if (_recv_statu != RECV_HTTP_BODY)
        {
            return false;
        }
        else
        {
            // 先获取正文长度
            size_t content_length = _request.ContentLength();
            if (content_length == 0)
            {
                // 没有正文则正确完毕请求解析正文
                _recv_statu = RECV_HTTP_OVER;
                return true;
            }
            else
            {
                // 先检查当前已经接受了多少正文 -- 即向 _request._body 中写入了多长数据
                size_t real_len = content_length - _request._body.size(); // 实际还需要接收的正文长度

                // 接收剩余正文内容到 _request._body 中 -- 同时要考虑当前缓冲区内的数据是否为全部的正文
                // 包含所有正文内容则取出所需
                if (buf->ReadAbleSize() >= real_len)
                {
                    std::optional<char *> read_pos = buf->GetReadPos();
                    if (read_pos)
                    {
                        _request._body.append(read_pos.value(), real_len);
                        buf->MoveReaderOffset(real_len);
                        _recv_statu = RECV_HTTP_OVER;
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                // 缓冲区中的数据无法满足当前正文长度需要, 取出数据, 等待新数据到来
                else
                {
                    std::optional<char *> read_pos = buf->GetReadPos();
                    uint64_t real_read_len = buf->ReadAbleSize();
                    if (read_pos)
                    {
                        _request._body.append(read_pos.value(), real_read_len);
                        buf->MoveReaderOffset(real_read_len);
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
    }

private:
    int _resp_statu;           // 响应状态码
    HttpRecvStatu _recv_statu; // 当前接收及解析的阶段状态标记
    HttpRequest _request;      // 已经解析得到的请求信息
};

/*----------------------------------------------------------------------------------------------------------------*/

#define DEFAULT_TIMEOUT 30
class HttpServer
{
private:
    using Handler = std::function<void(const HttpRequest &, HttpResponse *)>;

public:
    HttpServer(int port, int timeout = DEFAULT_TIMEOUT)
        : _server(port)
    {
        // 启动非活跃销毁, 并定义时长, 添加任务
        _server.EnableInactiveRelease(timeout);

        // 设置连接到来的用户端回调函数和请求到来的用户端回调函数
        _server.SetConnectedCallback(std::bind(&HttpServer::OnConnected, this, std::placeholders::_1));
        _server.SetMessageCallback(std::bind(&HttpServer::OnMessage, this, std::placeholders::_1, std::placeholders::_2));
    }

    // 设置相对根目录和默认资源
    void SetBaseDirAndFile(const std::string &path, const std::string src = "index.html")
    {
        assert(Util::IsDirectory(path));
        assert(Util::IsFile(path + src));
        _basedir = path;
        _default_src = src;
    }

    // 设置/添加请求的正则表达式与处理函数的映射关系
    void Get(const std::string &pattern, const Handler &handler)
    {
        _get_route.push_back(std::make_pair(std::regex(pattern), handler));
    }
    void Post(const std::string &pattern, const Handler &handler)
    {
        _post_route.push_back(std::make_pair(std::regex(pattern), handler));
    }
    void Put(const std::string &pattern, const Handler &handler)
    {
        _put_route.push_back(std::make_pair(std::regex(pattern), handler));
    }
    void Delete(const std::string &pattern, const Handler &handler)
    {
        _delete_route.push_back(std::make_pair(std::regex(pattern), handler));
    }

    // 设置线程数量
    void SetThreadCount(int count)
    {
        _server.SetThreadCount(count);
    }

    // 启动服务
    void Start()
    {
        _server.Start();
    }

private:
    // 错误展示页面
    void ErrorHandler(const HttpRequest &req, HttpResponse *rsp)
    {
        // 组织一个错误展示页面 -- 其实应当写成一个 .html 文件发送过去, 这里姑且先这样处理
        std::string body;
        body += "<html>";
        body += "<head>";
        body += "<meta http-equiv='Content-Type' content='text/html;charset=utf-8'>";
        body += "</head>";
        body += "<body>";
        body += "<h1>";
        body += std::to_string(rsp->_statu);
        body += " ";
        body += Util::StatuToDesc(rsp->_statu);
        body += "eeeeeeeeeeeeeeeeeee";
        body += "</h1>";
        body += "</body>";
        body += "</html>";

        // 将页面数据当作响应正文放入 rsp 中
        rsp->SetContent(body, "text/html");
    }

    // 将 HttpResponse 中的要素按照 Http 协议格式组织发送
    void WriteResponse(const std::shared_ptr<Connection> &conn, const HttpRequest &req, HttpResponse &rsp)
    {
        // 完善头部字段
        // 长短连接
        std::optional<bool> isClose = rsp.Close();
        if (isClose)
        {
            if (isClose.value() == true)
            {
                rsp.SetHeader("Connection", "close");
            }
            else
            {
                rsp.SetHeader("Connection", "keep-alive");
            }
        }
        else
        {
            Error_Log("judge isClose error!\n");
            return;
        }

        // 正文
        if (rsp._body.empty() == false && rsp.HasHeader("Content-Length") == false)
        {
            rsp.SetHeader("Content-Length", std::to_string(rsp._body.size()));
        }

        // 正文类型
        if (rsp._body.empty() == false && rsp.HasHeader("Content-Type") == false)
        {
            rsp.SetHeader("Content-Type", "application/octet-stream"); // 二进制流
        }

        // 重定向
        if (rsp._redirect_flag == true)
        {
            rsp.SetHeader("Location", rsp._redirect_url);
        }

        // 将 rsp 中的要素按照 Http 协议格式组织
        std::stringstream rsp_str;
        rsp_str << req._version << " " << std::to_string(rsp._statu) << " " << Util::StatuToDesc(rsp._statu) << "\r\n";
        for (auto &head : rsp._headers)
        {
            rsp_str << head.first << ": " << head.second << "\r\n";
        }
        rsp_str << "\r\n";
        rsp_str << rsp._body;

        // 发送数据
        conn->Send(rsp_str.str().c_str(), rsp_str.str().size());
    }

    // 检查是否是申请静态资源
    bool IsFileHandler(const HttpRequest &req)
    {
        // 必须有静态资源根目录
        if (_basedir.empty())
        {
            return false;
        }

        // 请求方法必须是 GET / HEAD 方法
        if (req._method != "GET" && req._method != "HEAD")
        {
            return false;
        }

        // 请求的静态资源路径必须合法
        if (Util::ValidPath(req._path) == false)
        {
            return false;
        }

        // 请求的资源必须存在且为普通文件
        // 但如果为 / 根目录, 则默认给后面加上主页 如: index.html
        // 要注意给相对路径加上前缀静态根目录转化为实际存在的路径
        std::string req_path = _basedir + req._path; // 不直接修改请求的资源路径, 定义一个临时对象
        if (req._path.back() == '/')
        {
            req_path += _default_src;
        }
        if (Util::IsFile(req_path) == false)
        {
            return false;
        }

        return true;
    }

    // 静态资源的请求处理 -- 将静态资源的文件数据进行读取放到 _rsp 的 _body 中, 并设置 mime
    bool FileHandler(const HttpRequest &req, HttpResponse *rsp)
    {
        std::string req_path = req._path;
        if (req_path == "/")
        {
            req_path = _basedir;
            req_path += _default_src;
        }
        else{
            req_path = _basedir + req._path;
        }
        bool ret = Util::ReadFile(req_path, &rsp->_body);
        if (ret == false)
        {
            return false;
        }
        else
        {
            std::string mime = Util::ExtMime(req_path);
            rsp->SetHeader("Content-Type", mime);
            return true;
        }
    }

    // 功能性请求分类处理
    bool Dispatcher(HttpRequest &req, HttpResponse *rsp, std::vector<std::pair<std::regex, Handler>> &handlers)
    {
        // 在对应请求方法的路由映射表中查找是否含有对应资源请求的处理函数, 如果存在就调用, 不存在就返回 404 -- NOT FOUND
        // 路由映射表存储键值对 -- 正则表达式匹配规则 && 处理回调函数
        // 使用正则表达式对请求的资源路径进行正则匹配, 匹配成功则使用对应处理回调函数进行处理
        for (auto &handler : handlers)
        {
            const std::regex &e = handler.first;
            bool ret = std::regex_match(req._path, req._matches, e);
            if (ret == false)
            {
                continue;
            }
            else
            {
                const Handler &functor = handler.second;
                functor(req, rsp); // 传入请求信息和空的rsp, 执行处理函数
                return true;
            }
        }
        rsp->_statu = 404;
        return false;
    }

    // 分辨请求并分配处理方法
    bool Route(HttpRequest &req, HttpResponse *rsp)
    {
        // 分辨是静态资源请求还是功能性请求
        // 静态资源请求就进行静态资源处理
        // 功能性请求就通过 Dispatchar 进行确认是否有对应的处理方法并进行处理(有方法处理, 没方法404)
        // 不是静态资源请求 or 功能性请求则返回 405 -- METHOD NOT FOUND
        if (IsFileHandler(req) == true)
        {
            return FileHandler(req, rsp);
        }
        else if (req._method == "GET" || req._method == "HEAD")
        {
            return Dispatcher(req, rsp, _get_route);
        }
        else if (req._method == "PUT")
        {
            return Dispatcher(req, rsp, _put_route);
        }
        else if (req._method == "POST")
        {
            return Dispatcher(req, rsp, _post_route);
        }
        else if (req._method == "DELETE")
        {
            return Dispatcher(req, rsp, _delete_route);
        }
        else
        {
            rsp->_statu = 405; // METHOD NOT FOUND
            return false;
        }
    }

    // 设置上下文
    void OnConnected(const std::shared_ptr<Connection> &conn)
    {
        conn->SetContext(HttpContext());
    }

    // 缓冲区数据解析 + 处理
    bool OnMessage(const std::shared_ptr<Connection> &conn, Buffer *buf)
    {
        while (buf->ReadAbleSize() > 0)
        {
            // 获取上下文
            std::any *any_context = conn->GetContext();
            HttpContext *context = std::any_cast<HttpContext>(any_context);
            if (context == nullptr)
            {
                return false;
            }
            else
            {
                // 通过上下文对缓冲区的数据进行解析, 得到 HttpRequest 对象
                // 解析出错则回复出错响应
                // 解析正常且请求获取完毕则处理, 未完毕则等待数据
                context->RecvHttpRequest(buf);
                HttpRequest &req = context->Request();
                HttpResponse rsp(context->RespStatu());

                // 进行错误响应并关闭连接
                if (context->RespStatu() >= 400)
                {
                    ErrorHandler(req, &rsp);       // 填充一个错误页面到 rsp 中
                    WriteResponse(conn, req, rsp); // 组织响应发送回客户端
                    context->ReSet();
                    buf->MoveReaderOffset(buf->ReadAbleSize()); // 出错了就清空缓冲区
                    conn->Shutdown();                           // 关闭连接
                    return false;
                }

                // 当前请求没有接收完整, 退出, 等待新数据到来
                if (context->RecvStatu() != RECV_HTTP_OVER)
                {
                    return true;
                }

                // 请求路由 + 业务处理
                Route(req, &rsp);

                // 对 HttpResponse 组织发送
                WriteResponse(conn, req, rsp);

                // 重置上下文
                context->ReSet();

                // 根据长短连接确定关闭或继续处理
                std::optional<bool> isClose = rsp.Close();
                if (isClose)
                {
                    if (isClose.value() == true)
                    {
                        conn->Shutdown(); // 短连接直接关闭
                    }
                    else
                    {
                        Error_Log("judge isClose error!\n");
                        return false;
                    }
                }
                else
                {
                    Error_Log("judge isClose error!\n");
                    return false;
                }
            }
        }
        return true;
    }

private:
    std::vector<std::pair<std::regex, Handler>> _get_route;
    std::vector<std::pair<std::regex, Handler>> _post_route;
    std::vector<std::pair<std::regex, Handler>> _put_route;
    std::vector<std::pair<std::regex, Handler>> _delete_route;
    std::string _basedir;     // 静态资源根目录
    std::string _default_src; // 如果为 / 根目录返回的默认静态资源
    TcpServer _server;
};