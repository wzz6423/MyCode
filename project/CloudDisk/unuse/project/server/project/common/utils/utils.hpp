/*
    实用工具类
*/

#pragma once

// C
#include <cstdio>
// C++
#include <string>
#include <array>
#include <vector>
#include <memory>
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <optional>
#include <atomic>
#include <format>
#include <chrono>
#include <random>
#include <iomanip>
#include <atomic>
#include <thread>
#include <bit>
#include <ranges>
// json
#include <jsoncpp/json/json.h>
// bundle
#include "../bundle/bundle.h"
// system call
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
// Other
#include "../log/logger.hpp"

namespace cloud_disk
{
    namespace Utils
    {
        // 随机 ID 和验证码生成工具类
        class RandomUtil
        {
        public:
            // 生成格式为 XXXX-XXXX-XXXX-XXXX 的 16 位 UUID 字符串
            auto uuid() -> std::string
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

                // 6. 使用字符串流进行格式化，避免 std::format 的编译问题
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

            // 生成6位随机数字验证码
            static auto VerifyCode() -> std::string
            {
                // 静态随机数生成器，避免每次调用重新初始化
                static std::mt19937 generator([]()
                                              {
        std::random_device rd;
        // 用随机设备生成种子序列，提高随机性
        std::seed_seq seed{rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd()};
        return std::mt19937(seed); }());

                // 线程局部的分布器，避免多线程竞争
                thread_local std::uniform_int_distribution<unsigned> distribution(0, 9);

                std::string code;
                code.reserve(6); // 预分配空间

                // 生成6个随机数字
                for (int i = 0; i < 6; ++i)
                {
                    code += '0' + distribution(generator);
                }

                return code;
            }

        private:
            // 获取本机 MAC 地址的工具函数 -- linux/unix
            auto getMacAddress() -> std::array<unsigned char, 6>
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

        // 时间相关
        class TimeUtil
        {
        public:
            // 获取当前时间戳(毫秒级)
            static auto Now() -> long long
            {
                // 使用 floor 替代 time_point_cast
                // floor 保证向下取整(截断)到毫秒精度
                auto ms_time_point = std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now());

                // 提取毫秒持续期并转换为整数
                return ms_time_point.time_since_epoch().count();
            }

            // 转换时间实用函数
            // 通过已经存在的时间获取格式化的时间
            static auto FormatExistTime(const std::time_t now,
                                        const std::string &format = "%Y-%m-%d %H:%M:%S") -> std::string
            {
                // 创建 system_clock 时间点
                const auto tp = std::chrono::system_clock::from_time_t(now);

                // 获取当前时区
                const auto *const current_zone = std::chrono::current_zone();

                // 转换为带时区的本地时间
                const std::chrono::zoned_time zt{current_zone, tp};

                // 使用 std::format 直接格式化时间
                return std::format("{}:{}", zt, format);
            }
        };

        // 文件操作实用工具类
        class FileUtil
        {
        public:
            // 构造函数, 接收文件路径
            explicit FileUtil(const std::string &filename)
                : _filename(filename)
            {
            }

            // 获取文件大小
            auto FileSize() const -> std::optional<int64_t>
            {
                std::error_code ec; // 用于捕获错误而不是抛出异常

                // 使用 filesystem 的 file_size 函数获取文件大小
                const auto size = std::filesystem::file_size(_filename, ec);

                // 检查是否发生错误
                if (ec) [[unlikely]]
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to get file size for {}: {}", _filename, ec.message());
                    return std::nullopt;
                }

                return size;
            }

            // 获取文件最后一次修改时间
            auto LastModifyTime() const -> std::optional<std::chrono::system_clock::time_point>
            {
                std::error_code ec; // 用于捕获错误而不是抛出异常

                // 获取文件最后修改时间
                auto ftime = std::filesystem::last_write_time(_filename, ec);

                // 检查是否发生错误
                if (ec) [[unlikely]]
                {
                    // 使用日志系统记录错误
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to get modify time for {}: {}", _filename, ec.message());
                    return std::nullopt;
                }

                // 将文件系统时间转换为系统时间
                return std::chrono::file_clock::to_sys(ftime);
            }

            // 获取文件最后一次访问时间
            auto LastAccessTime() const -> std::optional<std::chrono::system_clock::time_point>
            {
                std::error_code ec; // 用于捕获错误而不是抛出异常

                // 获取文件最后访问时间
                auto ftime = std::filesystem::last_write_time(_filename, ec);

                // 检查是否发生错误
                if (ec) [[unlikely]]
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to get access time for {}: {}", _filename, ec.message());
                    return std::nullopt;
                }

                // 将文件系统时间转换为系统时间
                return std::chrono::file_clock::to_sys(ftime);
            }

            // 获取文件名(不含路径)
            auto Filename() const -> std::string
            {
                // 使用 filesystem 路径处理库提取文件名
                return std::filesystem::path(_filename).filename().string();
            }

            // 读取文件指定区域的数据
            auto PartFile(size_t pos, size_t len) const -> std::optional<std::string>
            {
                // 以二进制和文件尾模式打开文件
                std::ifstream ifs(_filename, std::ios::binary | std::ios::ate);
                if (!ifs) [[unlikely]]
                {
                    // 记录文件打开失败错误
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to open file for reading: {}", _filename);
                    return std::nullopt;
                }

                // 获取文件大小(从文件末尾位置)
                const auto file_size = static_cast<size_t>(ifs.tellg());

                // 校验请求位置是否有效
                if (pos > file_size) [[unlikely]]
                {
                    // 记录读取位置超出文件大小的错误
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Read position {} exceeds file size {}: {}",
                            pos, file_size, _filename);
                    return std::nullopt;
                }

                // 设置读取起始位置
                ifs.seekg(pos, std::ios::beg);

                // 计算实际可读取长度
                const auto remaining = file_size - pos;
                const auto read_len = std::min(len, remaining);

                // 预分配内存并读取数据
                std::string content(read_len, '\0');
                ifs.read(content.data(), static_cast<std::streamsize>(read_len));

                // 检查读取操作是否成功
                if (!ifs) [[unlikely]]
                {
                    // 记录文件读取错误
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "File read error at position {}: {}", pos, _filename);
                    return std::nullopt;
                }

                return content;
            }

            // 获取文件全部内容
            auto File() const -> std::optional<std::string>
            {
                // 最高效读大文件处理方式 -- 使用 mmap 文件内存映射
                int fd = open(_filename.c_str(), O_RDONLY);
                if (fd == -1) [[unlikely]]
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to open file for reading: {}", _filename);
                    return std::nullopt;
                }

                struct stat sb;
                if (fstat(fd, &sb) == -1) [[unlikely]]
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to get file status: {}", _filename);
                    close(fd);
                    return std::nullopt;
                }

                size_t size = sb.st_size;
                char *addr = static_cast<char *>(mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));
                close(fd); // 文件描述符可立即关闭
                if (addr == MAP_FAILED) [[unlikely]]
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to map file to memory: {}", _filename);
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
            auto File(const std::string &body) const -> bool
            {
                // 打开文件描述符
                int fd = open(_filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) [[unlikely]]
                {
                    // 记录文件打开失败错误
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to open file for writing: {}: {}",
                            _filename, strerror(errno));
                    return false;
                }

                // 调整文件大小
                if (ftruncate(fd, body.size()) == -1) [[unlikely]]
                {
                    // 记录文件大小调整失败错误
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to set file size for {}: {}",
                            _filename, strerror(errno));
                    close(fd);
                    return false;
                }

                // 映射文件到内存
                void *mapped = mmap(nullptr, body.size(), PROT_WRITE, MAP_SHARED, fd, 0);
                if (mapped == MAP_FAILED) [[unlikely]]
                {
                    // 记录内存映射失败错误
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to mmap file {}: {}",
                            _filename, strerror(errno));
                    close(fd);
                    return false;
                }

                // 复制数据到映射内存
                memcpy(mapped, body.data(), body.size());

                // 确保数据写入磁盘
                if (msync(mapped, body.size(), MS_SYNC) == -1) [[unlikely]]
                {
                    // 记录同步失败错误
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to sync file {}: {}",
                            _filename, strerror(errno));
                }

                // 解除映射
                if (munmap(mapped, body.size()) == -1) [[unlikely]]
                {
                    // 记录解除映射失败错误
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to unmap file {}: {}",
                            _filename, strerror(errno));
                }

                // 关闭文件描述符
                if (close(fd) == -1) [[unlikely]]
                {
                    // 记录文件关闭失败错误
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to close file {}: {}",
                            _filename, strerror(errno));
                }

                return true;
            }

            // 压缩文件(使用 bundle 库)
            auto Compress(const std::string &packageName) const -> bool
            {
                std::optional<std::string> body_opt = File();
                // 读取文件内容
                if (!body_opt.has_value()) [[unlikely]]
                {
                    // 记录读取失败错误
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to read content for compression: {}", _filename);
                    return false;
                }
                std::string body = std::move(body_opt.value());

                // 使用 bundle 库进行压缩
                std::string packed = bundle::pack(bundle::LZIP, body);

                // 将压缩数据写入新文件
                if (FileUtil(packageName).File(packed))
                {
                    return true;
                }
                else [[unlikely]]
                {
                    // 记录写入压缩文件失败错误
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to write compressed file: {}", packageName);
                    return false;
                }
            }

            // 解压缩文件
            auto UnCompress(const std::string &filename) const -> bool
            {
                std::optional<std::string> body_opt = File();
                // 读取压缩文件内容
                if (!body_opt.has_value()) [[unlikely]]
                {
                    // 记录读取失败错误
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to read content for decompression: {}", _filename);
                    return false;
                }
                std::string body = std::move(body_opt.value());

                // 使用 bundle 库进行解压缩
                std::string unpacked = bundle::unpack(body);

                // 将解压数据写入新文件
                if (FileUtil(filename).File(unpacked))
                {
                    return true;
                }
                else [[unlikely]]
                {
                    // 记录写入解压文件失败错误
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to write decompressed file: {}", filename);
                    return false;
                }
            }

            // 检查文件/目录是否存在
            auto Exists() const noexcept -> bool
            {
                // 使用 filesystem 的 exists 函数检查存在性
                return std::filesystem::exists(_filename);
            }

            // 创建目录(递归创建所有中间目录)
            auto CreateDirectory() const -> bool
            {
                // 如果目录已存在, 直接返回成功
                if (Exists())
                {
                    return true;
                }

                std::error_code ec; // 用于捕获错误而不是抛出异常

                // 递归创建目录
                const bool result = std::filesystem::create_directories(_filename, ec);

                // 检查是否发生错误
                if (ec) [[unlikely]]
                {
                    // 记录创建目录失败错误
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to create directory {}: {}", _filename, ec.message());
                }

                return result;
            }

            // 扫描目录内容(仅文件, 非递归)
            auto ScanDirectory() const -> std::optional<std::vector<std::string>>
            {
                // 确保目录存在
                if (!CreateDirectory()) [[unlikely]]
                {
                    return std::nullopt;
                }

                std::error_code ec; // 用于捕获错误而不是抛出异常
                std::vector<std::string> files;

                // 遍历目录项
                for (const auto &entry : std::filesystem::directory_iterator(_filename, ec))
                {
                    // 只处理普通文件
                    if (entry.is_regular_file())
                    {
                        files.push_back(entry.path().relative_path().string());
                    }
                }

                // 检查遍历过程中是否发生错误
                if (ec) [[unlikely]]
                {
                    // 记录目录扫描失败错误
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Directory scan failed for {}: {}", _filename, ec.message());
                    return std::nullopt;
                }

                return files;
            }

            // 删除文件/目录
            auto Remove() const -> bool
            {
                // 如果文件不存在, 直接返回 false
                if (!Exists()) [[unlikely]]
                {
                    return false;
                }

                std::error_code ec; // 用于捕获错误而不是抛出异常

                // 递归删除目录或文件
                const auto result = std::filesystem::remove_all(_filename, ec);

                // 检查是否发生错误
                if (ec) [[unlikely]]
                {
                    // 记录删除失败错误
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to remove {}: {}", _filename, ec.message());
                    return false;
                }

                // 如果删除了至少一个文件/目录, 则返回 true
                return result > 0;
            }

        private:
            std::string _filename; // 文件路径
        };

        // Json 操作实用工具类
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
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "JSON serialization failed");
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
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "JSON parse error: {}", err);
                    return std::nullopt;
                }

                return root;
            }
        };
    }
}