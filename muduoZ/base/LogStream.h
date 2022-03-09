#ifndef _MUDUOZ_BASE_LOGSTREAM_
#define _MUDUOZ_BASE_LOGSTREAM_

#include <string>

#include "muduoZ/base/uncopyable.h"
#include "muduoZ/base/FixedBuffer.h"

namespace muduoZ{

//临时存储信息的类
class LogStream:uncopyable{
public:
    typedef FixedBuffer<smallBufferSize> Buffer; 

    /*操作符重载*/
    LogStream& operator<<(bool);

    LogStream& operator<<(short);
    LogStream& operator<<(unsigned short);
    LogStream& operator<<(int);
    LogStream& operator<<(unsigned int);
    LogStream& operator<<(long);
    LogStream& operator<<(unsigned long);
    LogStream& operator<<(long long);
    LogStream& operator<<(unsigned long long);

    LogStream& operator<<(float);
    LogStream& operator<<(double);
    LogStream& operator<<(long double);

    LogStream& operator<<(char);
    LogStream& operator<<(const char*);
    LogStream& operator<<(const unsigned char*);
    LogStream& operator<<(std::string&);//string对象地址


    void append(const char* msg,size_t len){buffer_.append(msg,len);}
    void resetBuffer(){buffer_.resetCur();}
    const Buffer& buffer() const {return buffer_;}

    //格式化整数，包括剪裁超长部分，处理负数
    template <typename T>
    void formatInteger(T);

private:
    Buffer buffer_;
    static const int kMaxNumericSize = 32;//当buffer剩余的空间小于kMaxNumericSize，该条信息<<剩下的内容会被无条件丢弃
};


}


#endif