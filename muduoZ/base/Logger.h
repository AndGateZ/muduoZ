#ifndef _MUDUOZ_BASE_LOGGER_
#define _MUDUOZ_BASE_LOGGER_

#include<string>

#include"muduoZ/base/uncopyable.h"
#include"muduoZ/base/LogStream.h"
#include"muduoZ/base/FixedBuffer.h"

namespace muduoZ{

#define LOG Logger(__FILE__, __LINE__).stream()

class Logger:uncopyable{
public:
    Logger(const char * programFileName,size_t line);
    ~Logger();

    LogStream& stream(){return impl_.stream_;}

    //静态类型，整个项目的log文件名统一
    static void setLogFileName(std::string name){logFileName_ = name;}
    static std::string getLogFileName(){return logFileName_;}

private:
    class Impl:uncopyable{
        public:
            Impl(const char * programFileName,size_t line);
            void formatTime();//记录时间

            LogStream stream_;
            size_t line_;//记录行号
            std::string programFileName_;//输出log程序的文件名
    };
    Impl impl_;
    static std::string logFileName_;
};

}


#endif