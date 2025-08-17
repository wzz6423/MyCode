/*
    实现 html 文件的去标签工作
*/

// const &: 输入型参数
// *: 输出型参数
// &: 输入输出型参数

#pragma once

// C++
#include <iostream>
#include <print>
#include <string>
#include <vector>
#include <filesystem>
// Other
#include "utils.hpp"

namespace parse
{
    // 存放所有 html 网页的目录路径
    const std::string srcPath = "data/input";
    // 存放所有去标签后的 html 网页资源的文件路径
    const std::string rswPath = "data/rawHtml/raw.txt";
    // boost 官网前置 url (带版本)
    static const std::string boostUrl = "https://www.boost.org/doc/libs/1_88_0/doc/html";
    // 文件内容的间隔
    static const std::string contentSeparator = "\3"; // (^C)
    // 文件间的间隔
    static const std::string fileSeparator = "\n";

    struct HtmlInfo
    {
        std::string title;   // 网页标题
        std::string content; // 网页内容
        std::string url;     // 该网页在官网中的 url
    };

    // 获取所有 html 资源文件
    auto EnumFiles(const std::string &filesPath, std::vector<std::string> *filesName) -> bool
    {
        std::filesystem::path rootPath(filesPath);
        // 目录文件存在性
        if (!std::filesystem::exists(rootPath))
        {
            std::println("rootPath not exists");
            return false;
        }
        // 是否为目录
        if (!std::filesystem::is_directory(rootPath))
        {
            std::println("rootPath is not a directory");
            return false;
        }
        // 遍历目录下的所有文件
        for (const auto &entry : std::filesystem::directory_iterator(rootPath))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".html")
            {
                filesName->push_back(entry.path().string());
            }
        }

        return true;
    }

    // 对 html 文件进行解析
    static auto ParseTitle(const std::string &fileContent, std::string *title) -> bool
    {
        size_t begin = fileContent.find("<title>");
        if (begin == std::string::npos)
        {
            std::println("<title> not found");
            return false;
        }

        size_t end = fileContent.find("</title>", begin);
        if (end == std::string::npos)
        {
            std::println("</title> not found");
            return false;
        }

        begin += std::string("<title>").size();

        if (begin > end)
        {
            return false;
        }

        *title = fileContent.substr(begin, end - begin);

        return true;
    }

    static auto ParseContent(const std::string &fileContent, std::string *content) -> bool
    {
        // 去标签, 数据清洗(状态机)
        enum class status_t
        {
            LABLE,
            CONTENT
        } status = status_t::LABLE;

        for (char c : fileContent)
        {
            switch (status)
            {
            case status_t::LABLE:
                if (c == '>')
                {
                    status = status_t::CONTENT;
                }
                break;
            case status_t::CONTENT:
                if (c == '<')
                {
                    status = status_t::LABLE;
                }
                else
                {
                    // 不保留 \n 字符
                    // 使用 \n 作为 html 解析之后文本的分隔符
                    if (c == '\n')
                    {
                        c = ' ';
                    }

                    content->push_back(c);
                }
                break;
            default:
                break;
            }
        }

        return true;
    }

    static auto MakeUrl(const std::string &fileName, std::string *url) -> bool
    {
        *url = boostUrl + fileName.substr(srcPath.size());

        return true;
    }

    auto ParseHtml(const std::vector<std::string> &filesName, std::vector<HtmlInfo> *htmlInfos) -> bool
    {
        for (const auto &fileName : filesName)
        {
            // 打开并读取文件内容
            std::string content;
            if (!util::FileUtil::ReadFile(fileName, &content))
            {
                std::println("FileUtil failed : {}", fileName);
                continue;
            }

            HtmlInfo html;
            // 解析文件提取 title
            if (!ParseTitle(content, &html.title))
            {
                std::println("ParseTitle failed : {}", fileName);
                continue;
            }

            // 解析文件提取 content
            if (!ParseContent(content, &html.content))
            {
                std::println("ParseContent failed : {}", fileName);
                continue;
            }

            // 解析指定文件路径构建 url
            if (!MakeUrl(fileName, &html.url))
            {
                std::println("MakeUrl failed : {}", fileName);
                continue;
            }

            // 将解析结果保存进数组
            htmlInfos->push_back(std::move(html));
        }

        return true;
    }

    // 保存解析后的 html 文件内容
    auto SaveHtmlInfos(const std::vector<HtmlInfo> &htmlInfos, const std::string &rswPath) -> bool
    {
        std::ofstream ofs(rswPath, std::ios::trunc | std::ios::out | std::ios::binary);
        if (!ofs.is_open())
        {
            std::println("ofs open failed: {}", rswPath);
            return false;
        }

        for (auto &e : htmlInfos)
        {
            ofs << e.title << contentSeparator;
            ofs << e.content << contentSeparator;
            ofs << e.url << fileSeparator;
        }

        ofs.close();

        return true;
    }
}