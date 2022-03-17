#include<sys/time.h>

#include"muduoZ/base/Logger.h"
#include"muduoZ/base/AsyncLogging.h"

namespace muduoZ{

std::string Logger::logFileName_ = "./LogFile";
const size_t rollSize = 500*1000*1000;//最大字节数为500M

//对AsyncLogging进行初始化，整个程序只初始化一次
static pthread_once_t once = PTHREAD_ONCE_INIT;
static AsyncLogging* asyncLogging;

void initAsyncLogging(){
    asyncLogging = new AsyncLogging(Logger::getLogFileName().c_str(),rollSize);
    asyncLogging->start();
}

////输出到后端，由后端提供接口
void output(const char* msg,size_t len){
    pthread_once(&once,initAsyncLogging);
    asyncLogging->append(msg,len);
}

Logger::Logger(const char * programFileName,size_t line):impl_(programFileName,line){}
Logger::~Logger(){
    impl_.stream_<<" -- "<< impl_.programFileName_<<':'<<impl_.line_<<'\n';
    output(impl_.stream_.buffer().getData(),impl_.stream_.buffer().getDataLen());
}

//勿忘初始化列表初始化对象成员
Logger::Impl::Impl(const char * programFileName,size_t line):stream_(),programFileName_(programFileName),line_(line){
    formatTime();//每条log第一句就是时间
}

void Logger::Impl::formatTime(){
    struct timeval tv;
    time_t time;
    char str_t[26] = {0};
    gettimeofday (&tv, NULL);
    time = tv.tv_sec;
    struct tm* p_time = localtime(&time);   
    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S  ", p_time);
    stream_ << str_t<<tv.tv_usec<<" ";
	//stream_ << str_t;
}

}
