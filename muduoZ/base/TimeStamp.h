#ifndef _MUDUOZ_NET_TIMESTAMP_
#define _MUDUOZ_NET_TIMESTAMP_

#include <boost/operators.hpp>
#include <stdint.h> 
#include <string> 

#include "muduoZ/base/copyable.h"

namespace muduoZ{

class TimeStamp :copyable,
				boost::equality_comparable<TimeStamp>,
                boost::less_than_comparable<TimeStamp>
{
public:
	TimeStamp();
	explicit TimeStamp(int64_t);
	~TimeStamp();
	std::string toFormattedString();
	bool valid();
	int64_t getMicroSecondsSinceEpoch();
	int64_t getSecondsSinceEpoch();
	TimeStamp addTime(double second);

	static TimeStamp now();
	static const int kMicroSecondsPerSecond = 1000*1000;//1秒==1000ms   1ms==1000us

private:
    int64_t  microSecondsSinceEpoch_;//从1970.01.01开始计算的微秒数
};

//重载操作符
inline bool operator<(TimeStamp& timeStamp1,TimeStamp& timeStamp2){
	return timeStamp1.getMicroSecondsSinceEpoch()<timeStamp2.getMicroSecondsSinceEpoch();
}
inline bool operator==(TimeStamp& timeStamp1,TimeStamp& timeStamp2){
	return timeStamp1.getMicroSecondsSinceEpoch()==timeStamp2.getMicroSecondsSinceEpoch();
}

}

#endif

