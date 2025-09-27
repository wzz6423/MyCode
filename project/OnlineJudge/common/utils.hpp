/*
    实用工具
*/

#pragma once

// C++
#include <filesystem>
#include <atomic>
#include <random>
#include <concepts>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <memory>
#include <ranges>
#include <algorithm>
#include <optional>
#include <format>
// syscall
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
// json
#include <jsoncpp/json/json.h>
// Other
#include "log.hpp"

namespace utils
{
    // 路径
    class PathUtil
    {
    public:
        // 编译时
        // 源文件路径 + 后缀
        static auto Src(const std::string &fileName) -> std::string
        {
            return std::move(addSuffix(fileName, _srcSuffix));
        }

        // 可执行程序路径 + 后缀
        static auto Exe(const std::string &fileName) -> std::string
        {
            return std::move(addSuffix(fileName, _exeSuffix));
        }

        // 错误文件路径 + 后缀
        static auto CompileError(const std::string &fileName) -> std::string
        {
            return std::move(addSuffix(fileName, _compileErrorSuffix));
        }

        // 运行时
        // 标准输入
        static auto StdIn(const std::string &fileName) -> std::string
        {
            return std::move(addSuffix(fileName, _stdInSuffix));
        }

        // 标准输出
        static auto StdOut(const std::string &fileName) -> std::string
        {
            return std::move(addSuffix(fileName, _stdOutSuffix));
        }

        // 错误文件路径 + 后缀
        static auto StdError(const std::string &fileName) -> std::string
        {
            return std::move(addSuffix(fileName, _stdErrorSuffix));
        }

    private:
        static auto addSuffix(const std::string &fileName, const std::string &suffix) -> std::string
        {
            return _prefix + fileName + suffix;
        }

    private:
        inline static const std::string _prefix = "./temp/";
        inline static const std::string _srcSuffix = ".cpp";
        inline static const std::string _exeSuffix = ".exe";
        inline static const std::string _compileErrorSuffix = ".compileErr";
        inline static const std::string _stdInSuffix = ".stdin";
        inline static const std::string _stdOutSuffix = ".stdout";
        inline static const std::string _stdErrorSuffix = ".stdErr";
    };

    // 定义概念: 约束处理器必须接受const std::string&并返回bool
    // 放在 FileUtil 内更好, 但写代码时编译器未支持
    template <typename F>
    concept LineProcessor = requires(F f, const std::string &line) {
        { f(line) } -> std::same_as<void>; // 核心约束：返回值必须是bool
    };

    // 文件
    class FileUtil
    {
    public:
        // 检测文件存在性
        static auto IsFileExists(const std::string &pathName) -> bool
        {
            return std::filesystem::exists(pathName);
        }

        // 生成唯一文件名
        // 生成格式为 XXXX-XXXX-XXXX-XXXX 的 16 位 UUID 字符串
        static auto UniqueId() -> std::string
        {
            // 1. 获取 MAC 地址作为基础标识符
            static const auto mac_addr = getMacAddress();

            // 2. 使用随机设备初始化高性能随机数生成器
            static thread_local std::random_device rd;
            static thread_local std::mt19937 generator(rd());

            // 3. 创建均匀分布的整数生成器（0-65535 范围）
            std::uniform_int_distribution<unsigned short> distribution(0, 65535);

            // 4. 生成随机数部分
            std::array<unsigned short, 4> random_parts;
            for (auto &part : random_parts)
            {
                part = distribution(generator);
            }

            // 5. 使用原子计数器提供额外唯一性保证
            static std::atomic<unsigned short> counter(0);
            unsigned short count_value = counter.fetch_add(1, std::memory_order_relaxed);

            // 6. 使用字符串流进行格式化
            std::stringstream ss;
            ss << std::hex << std::setfill('0');

            // 第一组：4位十六进制
            ss << std::setw(4) << random_parts[0] << "-";

            // 第二组：4位十六进制
            ss << std::setw(4) << random_parts[1] << "-";

            // 第三组：MAC地址后两位 + 随机数混合
            ss << std::setw(2) << static_cast<unsigned>(mac_addr[4])
               << std::setw(2) << static_cast<unsigned>(mac_addr[5]) << "-";

            // 第四组：计数器和随机数的混合值
            unsigned short mixed_value = count_value ^ random_parts[2] ^ random_parts[3];
            ss << std::setw(4) << mixed_value;

            return std::move(ss.str());
        }

        // 获取文件全部内容
        static auto File(const std::string &fileName) -> std::optional<std::string>
        {
            // 最高效读大文件处理方式 -- 使用 mmap 文件内存映射
            int fd = open(fileName.c_str(), O_RDONLY);
            if (fd == -1) [[unlikely]]
            {
                LOG(ERROR) << "Failed to open file for reading" << fileName << std::endl;
                return std::nullopt;
            }

            struct stat sb;
            if (fstat(fd, &sb) == -1) [[unlikely]]
            {
                LOG(ERROR) << "Failed to get file status" << fileName << std::endl;
                close(fd);
                return std::nullopt;
            }

            size_t size = sb.st_size;
            char *addr = static_cast<char *>(mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));
            close(fd); // 文件描述符可立即关闭
            if (addr == MAP_FAILED) [[unlikely]]
            {
                LOG(ERROR) << "Failed to map file to memory" << fileName << std::endl;
                return std::nullopt;
            }

            // 直接构造字符串(避免复制)
            std::string out = std::move(std::string(addr, size));
            // 解除内存映射关系, 释放资源
            munmap(addr, size);

            return out;
        }

        // 写入文件内容
        // 使用 mmap 高效写入文件内容
        static auto File(const std::string &fileName, const std::string &body) -> bool
        {
            // 获取文件所在目录路径
            std::filesystem::path dirPath = std::filesystem::path(fileName).parent_path();
            // 检测目录是否存在
            if (!dirPath.empty() && !std::filesystem::exists(dirPath))
            {
                // 递归创建所有缺失的目录
                if (!std::filesystem::create_directories(dirPath))
                {
                    // 目录创建失败处理
                    LOG(ERROR) << "Failed to create directory for file" << fileName << std::endl;
                    return false;
                }
            }

            // 打开文件描述符
            int fd = open(fileName.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) [[unlikely]]
            {
                LOG(ERROR) << "Failed to open file for writing" << fileName << strerror(errno) << std::endl;
                return false;
            }

            // 调整文件大小
            if (ftruncate(fd, body.size()) == -1) [[unlikely]]
            {
                LOG(ERROR) << "Failed to set file size for" << fileName << strerror(errno) << std::endl;
                close(fd);
                return false;
            }

            // 映射文件到内存
            void *mapped = mmap(nullptr, body.size(), PROT_WRITE, MAP_SHARED, fd, 0);
            if (mapped == MAP_FAILED) [[unlikely]]
            {
                LOG(ERROR) << "Failed to mmap file" << fileName << strerror(errno) << std::endl;
                close(fd);
                return false;
            }

            // 复制数据到映射内存
            memcpy(mapped, body.data(), body.size());

            // 确保数据写入磁盘
            if (msync(mapped, body.size(), MS_SYNC) == -1) [[unlikely]]
            {
                LOG(ERROR) << "Failed to sync file" << fileName << strerror(errno) << std::endl;
            }

            // 解除映射
            if (munmap(mapped, body.size()) == -1) [[unlikely]]
            {
                LOG(ERROR) << "Failed to unmap file" << fileName << strerror(errno) << std::endl;
            }

            // 关闭文件描述符
            if (close(fd) == -1) [[unlikely]]
            {
                LOG(ERROR) << "Failed to close file" << fileName << strerror(errno) << std::endl;
            }

            return true;
        }

        // 按行读取文件并返回所有行的vector
        static auto ReadAllLines(const std::string &file_path) -> std::optional<std::vector<std::string>>
        {
            // 打开文件(C++11起支持移动语义，无需手动close)
            std::ifstream file(file_path);
            if (!file.is_open())
            {
                // 抛出自定义异常，包含详细错误信息
                LOG(ERROR) << std::format("无法打开文件: '{}'（可能路径错误或权限不足）", file_path) << std::endl;
                return std::nullopt;
            }

            // 核心：将文件流转换为行范围，再转换为vector
            // std::views::istream 生成按行读取的视图
            return std::make_optional(std::views::istream<std::string>(file) | std::ranges::to<std::vector<std::string>>());
        }

        // 逐行处理文件(不存储所有行，适合大文件)
        static auto ProcessLines(const std::string &filePath, LineProcessor auto &&lineWorker) -> bool
        {
            std::ifstream file(filePath);
            if (!file.is_open())
            {
                LOG(ERROR) << std::format("无法打开文件: '{}'", filePath) << std::endl;
                return false;
            }

            // 直接遍历行视图，逐行处理(内存高效), 根据处理器返回值决定是否继续
            for (const std::string &line : std::views::istream<std::string>(file))
            {
                lineWorker(line);
            }
            return true;
        }

        // 删除文件
        static auto Remove(const std::string &fileName) -> bool
        {
            if (std::filesystem::exists(fileName))
            {
                if (std::filesystem::remove(fileName)) [[likely]]
                {
                    return true;
                }
                else
                {
                    LOG(ERROR) << "Failed to remove file" << fileName << std::endl;
                    return false;
                }
            }
            return true;
        }

    private:
        // 获取本机 MAC 地址的工具函数 -- linux/unix
        static auto getMacAddress() -> std::array<unsigned char, 6>
        {
            std::array<unsigned char, 6> mac_addr{};
            int fd = socket(AF_INET, SOCK_DGRAM, 0);
            if (fd >= 0)
            {
                struct ifreq ifr{};
                // 使用 eth0 接口 (可根据需要修改)
                std::ranges::copy_n("eth0", 4, ifr.ifr_name);

                if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0)
                {
                    std::ranges::copy_n(ifr.ifr_hwaddr.sa_data, 6, mac_addr.begin());
                }
                close(fd);
            }
            return mac_addr;
        }
    };

    // 时间
    class TimeUtil
    {
    public:
        // 获取当前时间戳
        static auto TimeStamp() -> std::string
        {
            struct timeval time;
            gettimeofday(&time, nullptr);
            return std::to_string(time.tv_sec * 1000 + time.tv_usec / 1000);
        }
    };

    // Json
    class JsonUtil
    {
    public:
        // 序列化 JSON 对象为字符串
        static auto Serialize(const Json::Value &root) -> std::optional<std::string>
        {
            // 创建 JSON 流写入器构建器
            Json::StreamWriterBuilder swb;

            // 创建流写入器
            std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());

            // 使用字符串流收集输出
            std::ostringstream oss;

            // 写入 JSON 数据
            if (sw->write(root, &oss) != 0) [[unlikely]]
            {
                // 记录序列化失败错误
                LOG(ERROR) << "JSON serialization failed" << std::endl;
                return std::nullopt;
            }

            return oss.str();
        }

        // 反序列化字符串为 JSON 对象
        static auto UnSerialize(const std::string &str) -> std::optional<Json::Value>
        {
            // 创建 JSON 对象存储结果
            Json::Value root;

            // 创建 JSON 字符读取器构建器
            Json::CharReaderBuilder crb;

            // 创建字符读取器
            std::unique_ptr<Json::CharReader> cb(crb.newCharReader());

            // 解析 JSON 字符串
            std::string err;
            const bool success = cb->parse(
                str.data(),
                str.data() + str.size(),
                &root,
                &err);

            // 检查解析结果
            if (!success || !err.empty()) [[unlikely]]
            {
                // 记录解析错误
                LOG(ERROR) << "JSON parse error: " << err << std::endl;
                return std::nullopt;
            }

            return root;
        }
    };

    // 字符串切分工具
    class StringUtil
    {
    public:
        /**
         * @brief 切分字符串
         *
         * @param str 待切分字符串
         * @param delimiter 分隔符
         * @return std::vector<std::string> 切分后的字符串数组
         */
        // 1. 按单个字符分隔
        auto Split(const std::string &str, char delimiter) -> std::vector<std::string>
        {
            // 核心逻辑：split_view分割 -> 转换为string -> 收集到vector
            return str | std::views::split(delimiter) | std::views::transform([](auto &&subrange)
                                                                              { return std::string(subrange.begin(), subrange.end()); }) |
                   std::ranges::to<std::vector<std::string>>();
        }

        // 2. 按子串分隔(如",,"、"||"等多字符分隔符)
        auto Split(const std::string &str, const std::string &delimiter) -> std::vector<std::string>
        {
            return str | std::views::split(delimiter) | std::views::transform([](auto &&subrange)
                                                                              { return std::string(subrange.begin(), subrange.end()); }) |
                   std::ranges::to<std::vector<std::string>>();
        }

        // 3. 扩展：忽略空字符串(如连续分隔符产生的空项)
        auto SplitNonEmpty(const std::string &str, char delimiter) -> std::vector<std::string>
        {
            return str | std::views::split(delimiter) | std::views::transform([](auto &&subrange)
                                                                              { return std::string(subrange.begin(), subrange.end()); }) |
                   std::views::filter([](const std::string &s) { // 过滤空字符串
                       return !s.empty();
                   }) |
                   std::ranges::to<std::vector<std::string>>();
        }
        static auto SplitNonEmpty(const std::string &str, const std::string &delimiter) -> std::vector<std::string>
        {
            return str | std::views::split(delimiter)            // 按子串分割为子范围视图
                   | std::views::transform([](auto &&subrange) { // 将子范围转换为std::string
                         return std::string(subrange.begin(), subrange.end());
                     }) |
                   std::views::filter([](const std::string &s) { // 过滤空字符串
                       return !s.empty();
                   }) |
                   std::ranges::to<std::vector<std::string>>(); // 转换为vector
        }
    };
}