#include <algorithm>
#include <stdio.h>
#include <string>

#include"muduoZ/base/LogStream.h"

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;

namespace muduoZ{

template<typename T>
size_t convert(char* buf,T t){
    T i = t;
    char* p = buf;

    do{
        int pos = static_cast<int>(i%10);
        i = i/10;
        *p++ = zero[pos];//用zero拓展出负数空间
    }while(i!=0);	//这里用do为了兼容t==0的情况

    if(t<0) *p++ = '-';
    *p = '\0';//最后一位为空
    std::reverse(buf,p);
    return p-buf;
}

template <typename T>
void LogStream::formatInteger(T t){
    //当buffer剩余的空间小于kMaxNumericSize，该条信息<<剩下的内容会被无条件丢弃
    if(buffer_.getRemainLen() >= kMaxNumericSize){
        size_t len = convert(buffer_.current(),t);//原地操作负数转换存储,返回长度
        buffer_.addCurPos(len);
    }
}

/*操作符重载*/
LogStream& LogStream::operator<<(bool t){
    buffer_.append(t ? "1" : "0", 1);
    return *this;
}

/*整数类型,全用formatInteger*/
LogStream& LogStream::operator<<(short t){
    formatInteger<short>(t);
    return *this;
}

LogStream& LogStream::operator<<(unsigned short t){
    formatInteger<unsigned short>(t);
    return *this;
}

LogStream& LogStream::operator<<(int t){
    formatInteger<int>(t);
    return *this;
}

LogStream& LogStream::operator<<(unsigned int t){
    formatInteger<unsigned int>(t);
    return *this;
}

LogStream& LogStream::operator<<(long t){
    formatInteger<long>(t);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long t){
    formatInteger<unsigned long>(t);
    return *this;
}

LogStream& LogStream::operator<<(long long t){
    formatInteger<long long>(t);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long t){
    formatInteger<unsigned long long>(t);
    return *this;
}

/*小数类型，用snprintf将数据复制到buf*/
LogStream& LogStream::operator<<(float t){
    *this<<static_cast<double>(t);
    return *this;
}

LogStream& LogStream::operator<<(double t){
    if(buffer_.getRemainLen() >= kMaxNumericSize){
        int len = snprintf(buffer_.current(),sizeof(t),"%.12g",t);
        buffer_.addCurPos(len);
    }
    return *this;
}

LogStream& LogStream::operator<<(long double t){
    if(buffer_.getRemainLen() >= kMaxNumericSize){
        int len = snprintf(buffer_.current(),sizeof(t),"%.12Lg",t);
        buffer_.addCurPos(len);
    }
    return *this;
}

/*字符类型，用snprintf将数据复制到buf*/
LogStream& LogStream::operator<<(char t){
    buffer_.append(&t,1);//如果超出，在append里被丢弃
    return *this;
}

LogStream& LogStream::operator<<(const char* t){
    if(t) buffer_.append(t,strlen(t));
    else buffer_.append("(null)", 6);
    return *this;
}

LogStream& LogStream::operator<<(const unsigned char* t){
    return operator<<(reinterpret_cast<const char*>(t));
}

LogStream& LogStream::operator<<(std::string& t){
    buffer_.append(t.c_str(),t.size());
    return *this;
}


}