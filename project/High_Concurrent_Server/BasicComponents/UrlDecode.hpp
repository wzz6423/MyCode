#pragma once

#include <iostream>
#include <string>
#include <optional>
#include <regex>

// 会出现乱码 -- 直接放在main函数中不会 -- std::smatch 是引用返回, 而非拷贝值返回因此局部变量销毁会导致内容不确定
// 通过正则表达式解析 HTTP 链接
// std::optional<std::smatch> UrlDecode(std::string url)
// {
//     // HTTP请求行格式: "GET /wzz/login?user=admin&passwrd=1234567 HTTP/1.1\r\n"
//     // 预处理：移除换行符
//     url.erase(std::remove(url.begin(), url.end(), '\r'), url.end());
//     url.erase(std::remove(url.begin(), url.end(), '\n'), url.end());
//     std::regex urlEncodeRule(R"((GET|HEAD|POST|PUT|DELETE)\s+([^?\s]+)(?:\?([^\s]*))?\s+(HTTP\/1\.[01]))");
//     // std::regex urlEncodeRule(R"((GET|HEAD|POST|PUT|DELETE) ([^\?]*)(?:\?(.*))? (HTTP\/1\.[01])(?:\n|\r\n)?)");
//     // R 表示为原始字符串不使用转移字符
//     // GET|HEAD|POST|PUT|DELETE   表示匹配并提取其中任意一个字符串直到遇到空格
//     // [^?]*   [^?]表示匹配非问号字符, *表示匹配0次或多次
//     // (?:...)   表示匹配某个格式的字符串, 但是不提取
//     // \?(.*)   \\?表示原始的?字符, (.*)表示提取?之后的任意字符0次或多次直到遇到空格
//     // HTTP\/1\.[01]   表示匹配以HTTP/1.开始, 后面跟一个0或一个1的字符串
//     // (?:\n|\r\n)?   最后的?表示匹配前面的表达式0次或1次

//     std::smatch matches;
//     bool ret = std::regex_match(url, matches, urlEncodeRule);
//     if (ret == false)
//     {
//         return std::nullopt;
//     }
//     else
//     {
//         return matches;
//     }
// }

// 定义URL解析结果结构体
struct UrlResult {
    std::string method;
    std::string path;
    std::string query;
    std::string protocol;
    std::string version;
};

// 预处理字符串：移除控制字符和冗余空格
void preprocess_input(std::string& input) {
    // 移除所有非打印字符（ASCII 32-126）
    input.erase(std::remove_if(input.begin(), input.end(),
        [](char c){ return !std::isprint(static_cast<unsigned char>(c)); }), input.end());
    
    // 合并连续空格为单个空格
    auto new_end = std::unique(input.begin(), input.end(),
        [](char a, char b){ return std::isspace(a) && std::isspace(b); });
    input.erase(new_end, input.end());
}

// HTTP请求行解析函数
std::optional<UrlResult> UrlDecode(const std::string& raw_request) {
    std::string processed = raw_request;
    preprocess_input(processed);

    // 核心正则表达式（优化版本）
    std::regex request_regex(
        R"((GET|HEAD|POST|PUT|DELETE)\s+)"     // 方法 [1]
        R"((/[^\s\?]*))"                      // 路径 [2]
        R"((?:\?([^\s]*))?)"                  // 查询参数 [3]
        R"(\s+(HTTP)/(1\.0|1\.1))"            // 协议版本 [4][5]
    , std::regex::optimize | std::regex::ECMAScript);

    std::smatch matches;
    if (!std::regex_match(processed, matches, request_regex)) {
        return std::nullopt;
    }

    if (matches.size() < 6) {
        return std::nullopt;
    }

    return UrlResult{
        .method = matches[1].str(),
        .path = matches[2].str(),
        .query = matches[3].str(),
        .protocol = matches[4].str(),
        .version = matches[5].str()
    };
}