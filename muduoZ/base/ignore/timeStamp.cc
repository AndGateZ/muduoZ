#include <stdio.h>
#include <sys/time.h>

#include"muduoZ/base/timeStamp.h"

namespace muduoZ{
    string TimeStamp::toString() const{
        char buf[64] = {0};//初始化
        int64_t second = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
        int64_t mircoSecond = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
        snprintf(buf,sizeof(buf),"%ld%.06ld",second,mircoSecond);
        return buf;
    }

    string TimeStamp::toFormattedString(bool showMicroseconds) const{
        char buf[64] = {0};
        time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
        struct tm tm_time;
        gmtime_r(&seconds, &tm_time);//转换从1970开始的给定时间(seconds)，以 struct tm 格式表示成协调世界时（UTC）输出到tm

        if (showMicroseconds)
        {
            int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
            snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
                    tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                    tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                    microseconds);
        }
        else
        {
            snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
                    tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                    tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        }
        return buf;
    }

    TimeStamp TimeStamp::now(){
        struct timeval tv;
        gettimeofday(&tv, NULL);//系统调用，把当前的时间信息放进tv中，第二个参数是当地时区信息
        int64_t seconds = tv.tv_sec;
        return TimeStamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);//用微秒构造Timestamp

    }
}
