#pragma once

#include <iostream>
#include <vector>
#include <optional>
#include <cstring>
#include <cstdint>
#include <cassert>

#define DEFAULT_BUFFER_SIZE 4096

class Buffer
{
public:
    Buffer()
        : _reader_idx(0),
          _writer_idx(0),
          _buffer(DEFAULT_BUFFER_SIZE)
    {
    }

    std::optional<char *> Begin()
    {
        std::vector<char>::iterator it = _buffer.begin();
        if (it != _buffer.end())
        {
            return &(*it);
        }
        else
        {
            return std::nullopt;
        }
    }

    // 获取当前写起始地址 -- _buffer空间起始地址加写偏移量
    std::optional<char *> GetWritePos()
    {
        std::optional<char *> ret = Begin();
        if (ret)
        {
            return ret.value() + _writer_idx;
        }
        else
        {
            return std::nullopt;
        }
    }

    // 获取当前读起始地址
    std::optional<char *> GetReadPos()
    {
        std::optional<char *> ret = Begin();
        if (ret)
        {
            return ret.value() + _reader_idx;
        }
        else
        {
            return std::nullopt;
        }
    }

    // 获取缓冲区末尾空闲空间大小 -- 写偏移之后的空闲空间
    uint64_t TailIdleSize()
    {
        return _buffer.size() - _writer_idx;
    }

    // 获取缓冲区起始空闲空间大小 -- 读偏移之前的空闲空间
    uint64_t HeadIdleSize()
    {
        return _reader_idx;
    }

    // 获取可读数据大小
    uint64_t ReadAbleSize()
    {
        return _writer_idx - _reader_idx;
    }

    // 将读偏移向后移动
    bool MoveReaderOffset(uint64_t len)
    {
        if (len == 0)
        {
            return true;
        }

        // 向后移动的大小必须小于可读的大小
        assert(len <= ReadAbleSize());
        if (len <= ReadAbleSize())
        {
            _reader_idx += len;
            return true;
        }
        else
        {
            return false;
        }
    }

    // 将写偏移向后移动
    bool MoveWriterOffset(uint64_t len)
    {
        if (len == 0)
        {
            return true;
        }

        // 向后移动的大小必须小于可写的大小 -- 后面的空闲空间
        assert(len <= TailIdleSize());
        if (len <= TailIdleSize())
        {
            _writer_idx += len;
            return true;
        }
        else
        {
            return false;
        }
    }

    // 确保可写空间足够 -- 整体空闲空间足够就移动数据, 否则扩容(若回头部写入设计成环回的不好维护)
    bool EnsureWriteSpace(uint64_t len)
    {
        // 如果末尾空闲空间大小足够就直接返回
        if (len <= TailIdleSize())
        {
            return true;
        }
        // 末尾空闲空间不足, 判断起始位置空闲空间与末尾空闲空间总和是否足够, 足够则移动数据到起始位置腾出空间
        else if (len <= HeadIdleSize() + TailIdleSize())
        {
            // 将数据移动到起始位置
            uint64_t rsz = ReadAbleSize(); // 保存当前数据大小
            std::copy(GetReadPos().value(), GetReadPos().value() + rsz, Begin().value());
            _reader_idx = 0;   // 读偏移归零
            _writer_idx = rsz; // 将写位置重置为可读数据大小 -- 即写偏移量
            return true;
        }
        // 总体空间不够, 需要扩容, 多扩一点方便后续使用
        else if (len > HeadIdleSize() + TailIdleSize())
        {
            uint64_t rsz = ReadAbleSize(); // 保存当前数据大小
            std::copy(GetReadPos().value(), GetReadPos().value() + rsz, Begin().value());
            _reader_idx = 0;   // 读偏移归零
            _writer_idx = rsz; // 将写位置重置为可读数据大小 -- 即写偏移量
            size_t addSize = len > (DEFAULT_BUFFER_SIZE / 8) ? len : (DEFAULT_BUFFER_SIZE / 8);
            _buffer.resize(_buffer.size() + addSize);
            return true;
        }
        else
        {
            return false;
        }
    }

    // 写入数据
    bool Write(const void *data, uint64_t len)
    {
        if (len == 0)
        {
            return true;
        }

        // 先保证空间足够
        EnsureWriteSpace(len);

        // 写入数据
        const char *cd = static_cast<const char *>(data);
        std::copy(cd, cd + len, GetWritePos().value());
        return true;
    }

    bool WriteAndPush(const void *data, uint64_t len)
    {
        bool retWri = Write(data, len);
        bool retMov = MoveWriterOffset(len);
        if (retWri && retMov)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool WriteString(const std::string &data)
    {
        return Write(data.c_str(), data.size());
    }

    bool WriteStringAndPush(const std::string &data)
    {
        bool retWri = Write(data.c_str(), data.size());
        bool retMov = MoveWriterOffset(data.size());
        if (retWri && retMov)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool WriteBuffer(const Buffer &buf)
    {
        return Write(const_cast<Buffer &>(buf).GetReadPos().value(), const_cast<Buffer &>(buf).ReadAbleSize());
    }

    bool WriteBufferAndPush(const Buffer &buf)
    {
        bool retWri = Write(const_cast<Buffer &>(buf).GetReadPos().value(), const_cast<Buffer &>(buf).ReadAbleSize());
        bool retMov = MoveWriterOffset(const_cast<Buffer &>(buf).ReadAbleSize());
        if (retWri && retMov)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    // 读取数据
    bool Read(void *buf, uint64_t &len)
    {
        // 要获取的数据大小必须小于可读数据大小
        assert(len <= ReadAbleSize());
        if (len <= ReadAbleSize())
        {
            std::copy(GetReadPos().value(), GetReadPos().value() + len, static_cast<char *>(buf));
            return true;
        }
        else
        {
            return false;
        }
    }

    bool ReadAndPop(void *buf, uint64_t &len)
    {
        bool retRed = Read(buf, len);
        bool retMov = MoveReaderOffset(len);
        if (retRed && retMov)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    std::optional<std::string> ReadAsString(uint64_t len)
    {
        std::string ret;
        ret.resize(len);
        bool retRed = Read(&ret[0], len); // c_str() 返回的是 const 的, 无法进行操作, 这样写可以正常写入
        if (retRed)
        {
            return ret;
        }
        else
        {
            return std::nullopt;
        }
    }

    std::optional<std::string> ReadAsStringAndPop(uint64_t len)
    {
        std::optional<std::string> ret = ReadAsString(len);
        if (ret)
        {
            MoveReaderOffset(len);
            return ret;
        }
        else
        {
            return std::nullopt;
        }
    }

    std::optional<char *> FindCRLF()
    {
        std::optional<char *> readPos = GetReadPos();
        char *ret = nullptr;
        if (readPos)
        {
            ret = static_cast<char *>(memchr(readPos.value(), '\n', ReadAbleSize()));
        }
        if (ret)
        {
            return ret;
        }
        else
        {
            return std::nullopt;
        }
    }

    // 获取一行数据是安全的, 因为通常(此处)针对的是ASCII码
    std::optional<std::string> GetLine()
    {
        std::optional<char *> pos = FindCRLF();
        if (pos)
        {
            std::optional<char *> readPos = GetReadPos();
            if (readPos)
            {
                std::optional<std::string> ret = ReadAsStringAndPop(pos.value() - readPos.value() + 1);
                return ret; // 在 ReadAsStringAndPop 已经进行过安全性检查, 故此处不必再次进行检查
            }
            else
            {
                return std::nullopt;
            }
        }
        else
        {
            return std::nullopt;
        }
    }

    std::optional<std::string> GetLineAndPop(){
        std::optional<std::string> res = GetLine();
        if(res){
            bool retMov = MoveReaderOffset(res.value().size());
            if(retMov){
                return res;
            }
        }
        return std::nullopt;
    }

    // 清空缓冲区
    bool Clear()
    {
        // 不用清除内容, 归零两个偏移量即可
        _reader_idx = 0;
        _writer_idx = 0;
        return true;
    }

private:
    std::vector<char> _buffer; // 使用 vector 进行内存空间管理
    uint64_t _reader_idx;      // 读偏移
    uint64_t _writer_idx;      // 写偏移
};