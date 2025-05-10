/*
    实现异步工作器
*/

#pragma once

// C++
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>
// Other
#include "buffer.hpp"

namespace Log
{
    class ASyncLooper
    {
    private:
        using Functor = std::function<void(Buffer &)>;

    public:
        using asynclooperPtr = std::shared_ptr<ASyncLooper>;

        ASyncLooper(const Functor &cb, bool enhance = false, DefaultBufferSize defaultbuffersize = DefaultBufferSize::MB_50)
            : _producer_buffer(enhance, defaultbuffersize),
              _consumer_buffer(enhance, defaultbuffersize),
              _thread(std::thread(&ASyncLooper::ThreadEntry, this)),
              _callback(cb)
        {
        }

        // 停止异步日志器的工作
        void Stop()
        {
            _is_stop = true;             // 将停止工作标志置为 true
            _consumer_cond.notify_all(); // 唤醒所有工作线程处理残留数据
            _thread.join();              // 等待工作线程退出
        }

        // 添加数据
        bool Push(const char *data, const size_t len)
        {
            bool flag = false;

            // 1.可扩容 2.不可扩容
            {
                std::unique_lock<std::mutex> lock(_mtx);
                // 条件变量控制, 缓冲区剩余空间大小大于数据长度则可以添加数据, 否则阻塞
                _producer_cond.wait(lock, [&]() -> bool
                                    { return _producer_buffer.WriteableSize().first >= len; });

                flag = _producer_buffer.Push(data, len);

                // 唤醒消费者处理缓冲区中的数据
                _consumer_cond.notify_one();
            }

            return flag;
        }

        bool Push(const std::string &data)
        {
            return Push(data.c_str(), data.size());
        }

        ~ASyncLooper()
        {
            Stop(); // 修改状态 & 清理残余数据 & 等待工作线程退出
        }

    private:
        // 线程入口函数 -- 工作线程
        // 利用回调函数处理消费缓冲区的数据, 处理后初始化缓冲区并进行交换
        void ThreadEntry()
        {
            while (true)
            {
                {
                    // 判断生产缓冲区是否有数据 -- 有: 交换处理, 无: 阻塞 -- 先加锁
                    std::unique_lock<std::mutex> lock(_mtx);

                    // 退出标志被设置且缓冲区无数据才退出, 否则可能导致数据丢失
                    if (_is_stop == true && _producer_buffer.Empty() == true && _consumer_buffer.Empty() == true)
                    {
                        break;
                    }

                    // 退出前 / 有数据则继续向下执行
                    _consumer_cond.wait(lock, [&]() -> bool
                                        { return _is_stop == true || !_producer_buffer.Empty(); });
                    _consumer_buffer.Swap(_producer_buffer);

                    // 唤醒生产者
                    _producer_cond.notify_all();

                    // 缓冲区交换完毕就解锁, 数据写入 & 数据处理无需加锁保护
                }

                // 被唤醒后处理消费缓冲区数据
                _callback(_consumer_buffer);

                // 初始化消费缓冲区
                _consumer_buffer.Reset();
            }
        }

    private:
        Buffer _producer_buffer;                // 生产者缓冲区, 将日志数据写在该缓冲区内
        Buffer _consumer_buffer;                // 消费者缓冲区, 对日志数据进行处理
        std::mutex _mtx;                        // 互斥锁, 保证多线程环境下对 Buffer 的操作是安全的
        std::condition_variable _producer_cond; // 生产者环境变量
        std::condition_variable _consumer_cond; // 消费者环境变量
        std::atomic<bool> _is_stop = false;     // 异步日志器是否停止运行
        std::thread _thread;                    // 异步工作器的工作线程
        Functor _callback;                      // 处理日志数据的回调函数, 由异步工作器使用者传入
    };
}