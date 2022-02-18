#ifndef _MUDUOZ_BASE_DATE_
#define _MUDUOZ_BASE_DATE_

#include "muduoZ/base/copyable.h"
#include "muduoZ/base/types.h"

#include <boost/operators.hpp>

struct tm;//完整日期时间的一个结构体

namespace muduoZ{

class Date : public muduoZ::copyable,
            boost::less_than_comparable<Date>,
            boost::equality_comparable<Date>
{
public:
    //用于格式转换后的输出
    struct YearMonthDay{
        int year; // [1900..2500]
        int month;  // [1..12]
        int day;  // [1..31]
    };

    static const int kDaysPerWeek = 7;
    static const int kJulianDayOf1970_01_01;

    //Date的四种构造方式
    Date():julianDayNumber_(0){}
    Date(int year,int month,int day);//在cc实现
    explicit Date(int julianDayNumber):julianDayNumber_(julianDayNumber){}
    explicit Date(const struct tm&);//在cc实现
    
    //默认的拷贝、赋值、析构

    //和另一个Date对象交换数值
    void swap(Date &date){
        std::swap(julianDayNumber_,date.julianDayNumber_);
    }

    //判断是否合法
    bool valid() const {return julianDayNumber_>0;}

    //格式转换
    string toString() const;//在cc实现
    struct YearMonthDay toYearMonthDay() const;//在cc实现

    //返回信息julianDayNumber_
    int julianDayNumber() const { return julianDayNumber_; }
    
    int year() const{
        return toYearMonthDay().year;
    }

    int month() const{
        return toYearMonthDay().month;
    }

    int day() const{
        return toYearMonthDay().day;
    }

    int weekDay() const{
        return (julianDayNumber_+1) % kDaysPerWeek;
    }

private:
    int julianDayNumber_;//从julianDay算起的日期数

};//头文件定义类有分号

//对Date比较的运算符重载
inline bool operator==(const Date &date1,const Date &date2)  {
    return date1.julianDayNumber() == date2.julianDayNumber();
}

inline bool operator<(const Date &date1,Date &date2)  {
    return date1.julianDayNumber() < date2.julianDayNumber();
}


}

#endif