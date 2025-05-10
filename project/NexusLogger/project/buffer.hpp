/*
    实现异步日志缓冲区
*/

#pragma once

// C++
#include <string>
#include <vector>
#include <optional>
// C
#include <cstdint>
#include <cassert>

namespace Log
{
    enum class DefaultBufferSize
    {
        MB = 1024 * 1024,
        MB_10 = 10 * 1024 * 1024,
        MB_50 = 50 * 1024 * 1024,
        MB_100 = 100 * 1024 * 1024,
        MB_512 = 512 * 1024 * 1024
    };

    class Buffer
    {
    public:
        Buffer(bool enhance = false, DefaultBufferSize defaultbuffersize = DefaultBufferSize::MB_50)
            : _buffer(static_cast<int>(defaultbuffersize)),
              _enhance(enhance)
        {
        }

        // 写入数据
        bool Push(const char *data, const size_t len)
        {
            // 缓冲区空间不足: 1.阻塞返回 false 2.扩容 -- 有最大大小限制的
            // 不允许扩容总空间足够但尾空间不足 || 允许扩容且空间不足
            if ((_enhance == false && len > WriteableSize().first && len < WriteableSize().second) || (_enhance == true && len > WriteableSize().second))
            {
                bool flag = EnsureWriteable(len);
                if (flag == false)
                {
                    return false;
                }
            }
            // 空间不足且不允许扩容直接返回 false
            else if (_enhance == false && len > WriteableSize().first)
            {
                return false;
            }
            
            // 空间足够
            // 拷贝数据到缓冲区
            std::copy(data, data + len, &_buffer[_writer_index]);

            // 偏移可写位置指针
            MoveWriter(len);

            return true;
        }

        bool Push(const std::string &data)
        {
            return Push(data.c_str(), data.size());
        }

        // 返回可读数据起始地址
        const char *Begin() const
        {
            return &_buffer[_reader_index];
        }

        // 返回可读数据长度
        size_t ReadableSize() const
        {
            // 此缓冲区不是环形缓冲区, 金仅会向后写入
            return _writer_index - _reader_index;
        }

        // 返回可写空间大小
        std::pair<size_t, size_t> WriteableSize() const
        {
            // 总空闲大小, 尾空闲大小
            return std::make_pair((_buffer.size() - _writer_index) + _reader_index, _buffer.size() - _writer_index);
        }

        // 移动读位置指针
        void MoveReader(const size_t len)
        {
            assert(len <= ReadableSize());
            _reader_index += len;
        }

        // 重置读写位置, 初始化缓冲区
        void Reset()
        {
            _reader_index = 0;
            _writer_index = 0;
        }

        // 互换 Buffer 的缓冲区空间
        void Swap(Buffer &buffer)
        {
            _buffer.swap(buffer._buffer);
            std::swap(_reader_index, buffer._reader_index);
            std::swap(_writer_index, buffer._writer_index);
        }

        // 判断缓冲区是否为空
        bool Empty()
        {
            // return _reader_index == _writer_index;
            return ReadableSize() == 0;
        }

    private:
        // 扩容缓冲区
        bool EnsureWriteable(const size_t len)
        {
            // 如果总空闲大小足够但尾部空闲大小不足
            if (len > WriteableSize().first && len < WriteableSize().second)
            {
                // 将数据移动到起始位置
                uint64_t rsz = ReadableSize(); // 保存当前数据大小
                std::copy(Begin(), Begin() + rsz, &_buffer[0]);
                _reader_index = 0;   // 读偏移归零
                _writer_index = rsz; // 将写位置重置为可读数据大小 -- 即写偏移量
                return true;
            }
            else
            {
                // 如果达到 512MB 则不进行扩容直接返回false
                if (_buffer.size() >= static_cast<int>(DefaultBufferSize::MB_512))
                {
                    return false;
                }

                // 到达阈值大小之前扩容程度较大
                if (_buffer.size() >= static_cast<int>(DefaultBufferSize::MB_100) || _buffer.size() + len >= static_cast<int>(DefaultBufferSize::MB_100))
                {
                    _buffer.resize(_buffer.size() + static_cast<int>(DefaultBufferSize::MB_10) > _buffer.size() + len ? _buffer.size() + static_cast<int>(DefaultBufferSize::MB_10) : _buffer.size() + len + static_cast<int>(DefaultBufferSize::MB_10));
                }
                else
                {
                    _buffer.resize(_buffer.size() * 2 > _buffer.size() + len ? _buffer.size() * 2 : _buffer.size() * 2 + len);
                }
            }

            return true;
        }

        // 移动写位置指针
        void MoveWriter(const size_t len)
        {
            assert(len <= WriteableSize().second);
            _writer_index += len;
        }

    public:
        std::vector<char> _buffer; // 利用 vector<char> 模拟实现缓冲区, string 无法很好地处理 \0 等问题
        size_t _reader_index = 0;  // 可读数据指针 -- 数组下标
        size_t _writer_index = 0;  // 可写数据下标 -- 数组下标
        bool _enhance;             // 缓冲区是否可以进行扩容
    };
}