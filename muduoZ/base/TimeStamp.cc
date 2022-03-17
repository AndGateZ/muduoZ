#include <sys/time.h>

#include "muduoZ/base/TimeStamp.h"

namespace muduoZ{

TimeStamp::TimeStamp():microSecondsSinceEpoch_(0){}
TimeStamp::TimeStamp(int64_t microSecondsSinceEpoch):microSecondsSinceEpoch_(microSecondsSinceEpoch){}
TimeStamp::~TimeStamp(){}

std::string TimeStamp::toFormattedString(){
	char buf[64] = {0};
	time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
	struct tm tm_time;
	gmtime_r(&seconds, &tm_time);//转换从1970开始的给定时间(seconds)，以 struct tm 格式表示成协调世界时（UTC）输出到tm

	int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
	snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
			tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
			tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
			microseconds);
	return buf;
}

bool TimeStamp::valid(){
	return microSecondsSinceEpoch_>0;
}

int64_t TimeStamp::getMicroSecondsSinceEpoch(){
	return microSecondsSinceEpoch_;
}

int64_t TimeStamp::getSecondsSinceEpoch(){
	return microSecondsSinceEpoch_/kMicroSecondsPerSecond;
}

TimeStamp TimeStamp::addTime(double second){
	TimeStamp newTimeStamp(static_cast<int64_t>(microSecondsSinceEpoch_+second*kMicroSecondsPerSecond));
	return newTimeStamp;
}

TimeStamp TimeStamp::addTime(size_t milliSecond){
	TimeStamp newTimeStamp(static_cast<int64_t>(microSecondsSinceEpoch_+milliSecond*kMillisecondsPerSecond));
	return newTimeStamp;
}

TimeStamp TimeStamp::now(){
	struct timeval tv;
	gettimeofday(&tv, NULL);//系统调用，把当前的时间信息放进tv中，第二个参数是当地时区信息
	int64_t seconds = tv.tv_sec;
	return TimeStamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}

TimeStamp TimeStamp::invalid(){
	return TimeStamp();
}

size_t TimeStamp::timeDiffInMillSeconds(TimeStamp t1,TimeStamp t2){
	if(t1>t2){
		return (t1.getMicroSecondsSinceEpoch()-t2.getMicroSecondsSinceEpoch())/kMillisecondsPerSecond;
	}
	else{
		return (t2.getMicroSecondsSinceEpoch()-t1.getMicroSecondsSinceEpoch())/kMillisecondsPerSecond;
	}
}

}