#pragma once

#include "Common.h"

class TimeManager{
public:
    TimeManager(time_t time = time(nullptr), time_t max_time = 600)
        :_last_time(time),
        _max_living_time(max_time)
    {}

    static time_t GetNowTime(){
        return time(nullptr);
    }

    void UpdateLastTime(time_t newtime = GetNowTime()){
        _last_time = newtime;
    }

    bool IsLiving(time_t now = GetNowTime()){
        return _last_time - now >= _max_living_time;
    }
private:
    time_t _last_time = 0;

    size_t _max_living_time = 0;
};