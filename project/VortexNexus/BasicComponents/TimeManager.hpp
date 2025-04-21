#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <cstdint>
#include <unistd.h>

using TaskFunc = std::function<void()>;
using ReleaseFunc = std::function<void()>;
class TimerTask{
public:
    TimerTask(uint64_t id, uint32_t timeout, const TaskFunc& task_cb)
        :_id(id),
        _timeout(timeout),
        _task_cb(task_cb)
    {}

    void Cancel(){
        _canceled = true;
    }

    void SetRelease(const ReleaseFunc& release_cb){
        _release_cb = release_cb;
    }

    uint32_t DelayTime(){
        return _timeout;
    }

    ~TimerTask(){
        if(_canceled == false){
            _task_cb();
        }
        _release_cb();
    }
private:
    uint64_t _id = 0; // 定时器对象任务 id
    uint32_t _timeout = 0; // 定时任务的超时时间
    bool _canceled = false; // false-没有被取消 true-被取消
    TaskFunc _task_cb; // 定时器对象要执行的定时任务
    ReleaseFunc _release_cb; // 用于删除 TimeWheel 中保存的定时器对象的信息
};

class TimerWheel{
public:
TimerWheel()
        :_tick(0),
        _capacity(60),
        _wheel(_capacity)
    {}

    // 添加定时任务
    void TimerAdd(uint64_t id, uint32_t delayTime, const TaskFunc& task_cb){
        std::shared_ptr<TimerTask> ptr = std::make_shared<TimerTask>(id, delayTime, task_cb);
        ptr->SetRelease(std::bind(&TimerWheel::RemoveTimer, this, id));
        int pos = (_tick + delayTime) % _capacity;
        _wheel[pos].push_back(ptr);
        _timers[id] = std::weak_ptr<TimerTask>(ptr);
    }

    // 刷新/延迟定时任务 -- 通过 shared_ptr 销毁时检查计数器的原理实现
    bool TimerRefresh(uint64_t id){
        // 通过保存的 weak_ptr 构造出一个 shared_ptr 添加到时间轮中
        std::unordered_map<uint64_t, std::weak_ptr<TimerTask>>::iterator it = _timers.find(id);
        if(it == _timers.end()){
            return false; // 没有找到定时任务, 无法进行刷新延迟
        }
        std::shared_ptr<TimerTask> ptr = it->second.lock(); // 通过 it 获取定时任务对象的 weak_ptr 后通过 lock 函数获取其管理对象的 shared_ptr
        int pos = (_tick + ptr->DelayTime()) % _capacity;
        _wheel[pos].push_back(ptr);
        return true;
    }

    bool TimerCancel(uint64_t id){
        std::unordered_map<uint64_t, std::weak_ptr<TimerTask>>::iterator it = _timers.find(id);
        if(it == _timers.end()){
            return false; // 没有找到定时任务, 无法进行取消
        }
        std::shared_ptr<TimerTask> ptr = it->second.lock(); // 通过 it 获取定时任务对象的 weak_ptr 后通过 lock 函数获取其管理对象的 shared_ptr
        if(ptr){
            ptr->Cancel();
        }
        return true;
    }

    // 此函数每秒钟被执行一次, 相当于嘀嗒指针 1s 向后走了一步, 执行一次定时任务
    void RunTimerTask(){
        _tick = (_tick + 1) % _capacity;
        _wheel[_tick].clear(); // 清空指定位置的数组就会把该数组中保存的所有管理定时器对象的 shared_ptr 释放掉
    }

    ~TimerWheel() = default;
private:
    // 取消定时任务
    void RemoveTimer(uint64_t id){
        std::unordered_map<uint64_t, std::weak_ptr<TimerTask>>::iterator it = _timers.find(id);
        if(it != _timers.end()){
            _timers.erase(id);
        }
    }

    TimerWheel(const TimerWheel&) = delete;
    TimerWheel& operator=(const TimerWheel&) = delete;
private:
    int _tick = 0; // 嘀嗒指针, 走到哪里释放哪里(执行哪里的任务)
    int _capacity = 0; // 表盘的最大容量 -- 本质是最大延迟时间
    std::vector<std::vector<std::shared_ptr<TimerTask>>> _wheel; // 时间轮数组 -- 如果需要扩充定时时间只需要再创建 分钟数组、小时数组、日子数组、周数组、月数组、年数组... 进行扩展即可(时间到了把任务向上一级数组进行转移即可)
    std::unordered_map<uint64_t, std::weak_ptr<TimerTask>> _timers; // 记录 TimerTask 的定时器对象任务 id 与其对应的 weak_ptr
};