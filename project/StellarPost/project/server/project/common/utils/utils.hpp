/*
    实用工具类
*/

#pragma once

// C++
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <atomic>
#include <chrono>
#include <random>
#include <iomanip>
// Other
#include "../log/logger.hpp"

namespace stellar_post
{
    namespace Utils
    {
        // 生成唯一 ID -- 由 16 位随机字符组成的字符串作为唯一 ID
        // 如果有需要可以获取本机 mac 地址添加进去能够进一步提高生成随机数的唯一性概率
        auto Uuid() -> std::string
        {
            // 1.生成 6 个 0-255 之间的随机数字(1 字节 -> 转换为 16 进制字符) -- 生成 12 位 16 进制字符
            std::random_device rd;                                   // 实例化设备随机数对象 -- 用于生成设备随机数(随机度极高, 效率低)
            std::mt19937 generator(rd());                            // 以设备随机数为种子, 实例化伪随机数对象
            std::uniform_int_distribution<int> distribution(0, 255); // 限定数据范围
            std::stringstream ss;
            for (size_t i = 0; i < 6; ++i)
            {
                if (i == 2 || i == 4)
                {
                    ss << "-";
                }
                ss << std::setw(2) << std::setfill('0') << std::hex << distribution(generator);
            }
            ss << "-";

            // 2.通过一个静态变量生成一个 2 字节的编号数字 -- 生成 4 位 16 进制字符
            static std::atomic<short> idx(0);
            short tmp = idx.fetch_add(1);
            ss << std::setw(4) << std::setfill('0') << std::hex << tmp;

            // 3.返回拼接后的字符串
            return ss.str();
        }

        // 生成 6 位随机验证码
        auto VCode() -> std::string
        {
            // 1.生成 6 个 0-9 之间的随机数字
            std::random_device rd;                                 // 实例化设备随机数对象 -- 用于生成设备随机数(随机度极高, 效率低)
            std::mt19937 generator(rd());                          // 以设备随机数为种子, 实例化伪随机数对象
            std::uniform_int_distribution<int> distribution(0, 9); // 限定数据范围
            std::stringstream ss;
            for (size_t i = 0; i < 6; ++i)
            {
                ss << distribution(generator);
            }
            return ss.str();
        }

        // 文件的读写操作接口
        // 读
        auto ReadFile(const std::string &fileName, std::string *body) -> bool
        {
            std::ifstream ifs(fileName, std::ios::binary | std::ios::in);
            if (ifs.is_open() == false)
            {
                Log::lg("error", Log::FileName(), Log::Line(), "open file {} error!", fileName);
                return false;
            }

            body->assign(
                (std::istreambuf_iterator<char>(ifs)), // 起始迭代器：指向文件开头
                (std::istreambuf_iterator<char>())     // 结束标记：表示文件结尾
            );
            if (ifs.good() == false)
            {
                Log::lg("error", Log::FileName(), Log::Line(), "read file {} error!", fileName);
                return false;
            }

            return true;
        }

        // 写
        auto WriteFile(const std::string &fileName, const std::string &body) -> bool
        {
            std::ofstream ofs(fileName, std::ios::binary | std::ios::out | std::ios::trunc);
            if (ofs.is_open() == false)
            {
                Log::lg("error", Log::FileName(), Log::Line(), "open file {} error!", fileName);
                return false;
            }

            ofs << body;
            if (ofs.good() == false)
            {
                Log::lg("error", Log::FileName(), Log::Line(), "write file {} error!", fileName);
                return false;
            }

            ofs.close(); // fstream 是 RAII 的, 读文件可以不用手动关闭, 但是写文件时手动关闭能够保证数据写入文件没有差错
            return true;
        }

        // 获取当前时间戳（毫秒级）
        auto Now() -> long long
        {
            auto now = std::chrono::system_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch());
            return timestamp.count();
        }
    }
}