#ifndef _MUDUOZ_BASE_TIMESTAMP_
#define _MUDUOZ_BASE_TIMESTAMP_

#include <boost/operators.hpp>

#include "muduoZ/base/copyable.h"
#include "muduoZ/base/types.h"

using namespace std;

namespace muduoZ{
class TimeStamp : copyable,
                boost::equality_comparable<TimeStamp>,
                boost::less_than_comparable<TimeStamp>{
public:
    //构造函数
    TimeStamp():microSecondsSinceEpoch_(0){}
    explicit TimeStamp(int64_t  microSecondsSinceEpoch):microSecondsSinceEpoch_(microSecondsSinceEpoch){}

    //功能函数
    //交换,这里的参数只有一个，自己和其他stamp交换时间
    void swap(TimeStamp &t1){
        std::swap(microSecondsSinceEpoch_, t1.microSecondsSinceEpoch_);
    }

    //获取成员
    int64_t microSecondsSinceEpoch() const {return microSecondsSinceEpoch_;}

    //判断对象有效性,注意const
    bool valid() const {return microSecondsSinceEpoch_>0;}

    //格式转换输出
    string toString() const;//cc实现
    string toFormattedString(bool showMicroseconds) const;//cc实现

    //静态工具，用命名空间名字(类名)调用、定义
    static TimeStamp now();//获取当前时间戳，cc实现
    static TimeStamp invalid(){return TimeStamp();}

    static TimeStamp fromUnixTime(time_t t){return fromUnixTime(t,0);}

    static TimeStamp fromUnixTime(time_t t, int microseconds ){
        return TimeStamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + microseconds);//time_t是long int
    }

    //int类型就够了
    static const int kMicroSecondsPerSecond = 1000*1000;

private:
    int64_t  microSecondsSinceEpoch_;

};

//操作符重载==
inline bool operator==(TimeStamp &t1,TimeStamp &t2){
    return t1.microSecondsSinceEpoch()==t2.microSecondsSinceEpoch();
}

//操作符重载<
inline bool operator<(TimeStamp &t1,TimeStamp &t2){
    return t1.microSecondsSinceEpoch()<t2.microSecondsSinceEpoch();
}

// inline double timeDifference(TimeStamp &high,TimeStamp &low){//这里先转换为double再做除法
//     return static_cast<double>(high.microSecondsSinceEpoch()-low.microSecondsSinceEpoch())/TimeStamp::kMicroSecondsPerSecond;
// }

inline TimeStamp addTime(TimeStamp &t1,double second){//时间戳只支持s为单位增加时间
    TimeStamp timeStamp(static_cast<int64_t>(second*TimeStamp::kMicroSecondsPerSecond)+t1.microSecondsSinceEpoch());
    return timeStamp;
}

}//namespace muduoZ 

#endif