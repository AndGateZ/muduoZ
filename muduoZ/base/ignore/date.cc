#include "muduoZ/base/date.h"
//#include <stdio.h>  // snprintf
namespace muduoZ{

namespace detail{
//char require_32_bit_integer_at_least[sizeof(int) >= sizeof(int32_t) ? 1 : -1];

int getJulianDayNumber(int year, int month, int day)//计算julian日期
{
  //(void) require_32_bit_integer_at_least; // no warning please 没看懂什么意思
  int a = (14 - month) / 12;
  int y = year + 4800 - a;
  int m = month + 12 * a - 3;
  return day + (153*m + 2) / 5 + y*365 + y/4 - y/100 + y/400 - 32045;
}

struct Date::YearMonthDay getYearMonthDay(int julianDayNumber)//julian日期计算年月日结构体
{
  int a = julianDayNumber + 32044;
  int b = (4 * a + 3) / 146097;
  int c = a - ((b * 146097) / 4);
  int d = (4 * c + 3) / 1461;
  int e = c - ((1461 * d) / 4);
  int m = (5 * e + 2) / 153;
  Date::YearMonthDay ymd;
  ymd.day = e - ((153 * m + 2) / 5) + 1;
  ymd.month = m + 3 - 12 * (m / 10);
  ymd.year = b * 100 + d - 4800 + (m / 10);
  return ymd;
}

}//namespace detail

const int Date::kJulianDayOf1970_01_01 = detail::getJulianDayNumber(1970, 1, 1);//计算机日期计算起始点

using namespace detail;

Date::Date(int year,int month,int day):julianDayNumber_(getJulianDayNumber(year,month,day)){}

Date::Date(const struct tm& t):julianDayNumber_(
    getJulianDayNumber(t.tm_year+1900,t.tm_mon+1,t.tm_mday)){}

string Date::toString() const{
    char buf[32];
    YearMonthDay ymd(toYearMonthDay());
    snprintf(buf, sizeof buf, "%4d-%02d-%02d", ymd.year, ymd.month, ymd.day);
    return buf;
}

Date::YearMonthDay Date::toYearMonthDay() const{
    return getYearMonthDay(julianDayNumber_);
}


}//namespace muduoZ