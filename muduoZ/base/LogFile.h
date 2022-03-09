#ifndef _MUDUOZ_BASE_LOGFILE_
#define _MUDUOZ_BASE_LOGFILE_

#include <string>
#include <memory>
#include <stdio.h>

#include "muduoZ/base/uncopyable.h"
#include "muduoZ/base/Mutex.h"

namespace muduoZ{

class AppendFile:uncopyable{
public:
    AppendFile(const std::string& logFileName = "LogFile");
    ~AppendFile();

    void append(const char* msg,size_t len);//写入数据
    void flush();//从缓冲区写入文件，减少io次数
    size_t getWrittenBytes(){return writtenBytes_;}
    

private:
    size_t write(const char *msg, size_t len);//写入一条数据
    FILE *file_;
    size_t writtenBytes_;//记录已写的字节数，即文件大小
    char buffer_[64 * 1024];
};


class LogFile:uncopyable{
public:
    LogFile(bool threadSafe,size_t rollSize,const std::string& logFileName = "LogFile");
    ~LogFile();

    void output(const char* msg,size_t len);//接收大buffer,考虑线程安全
    void flush();//考虑线程安全
    void rollFile();//创建新文件
    std::string getFileName(const std::string& logFileName);//处理文件名，文件名+时间+后缀

private:
    void appendUnlocked(const char *msg, size_t len);

    const std::string logFileName_;
    size_t count_;//记录写入次数
    size_t rollSize_;//最大文件大小
    std::unique_ptr<Mutex> mutex_;//可以是有锁或者无锁两个情况，这里用指针是因为，便于判断有锁无锁的情况
    //有锁：保证线程安全，多个线程调用AsyncLogging后，AsyncLogging以无锁的形式调用LogFile.output，LogFile底层是fwrite_unlocked，保证数据不乱
    //无锁：当使用这个日志的程序不是多线程的时候，用无锁的版本
    std::unique_ptr<AppendFile> appendFile_;//这里用指针是因为，一个appendFile_和一个系统里的文件对应
};


}



#endif