#ifndef _MUDUOZ_BASE_TIMEZONE_
#define _MUDUOZ_BASE_TOMEZONE_

#include <memory>

#include "muduoZ/base/copyable.h"

namespace muduoZ{

class TimeZone: copyable{
public:
    explicit TimeZone(const char* zonefile);//从文件构造，在cc中实现
    TimeZone(int eastOfUtc, const char* tzname);//输入时区，名字，在cc中实现
    TimeZone() = default;//拥有默认构造函数

    //判断对象是否有效
    bool valid(){return static_cast<bool>(data_);}

    //工具函数
    struct tm toLocalTime(time_t secondsSinceEpoch) const;
    time_t fromLocalTime(const struct tm&) const;

    //静态工具函数
    static struct tm toUtcTime(time_t secondsSinceEpoch, bool yday = false);
    static time_t fromUtcTime(const struct tm&);
    static time_t fromUtcTime(int year, int month, int day,//// year in [1900..2500], month in [1..12], day in [1..31]
                            int hour, int minute, int seconds);
    
    //结构体，在cc定义
    struct Data;
    

private:
    std::shared_ptr<Data> data_;
};

}//namespace muduoZ


#endif