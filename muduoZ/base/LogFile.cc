#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "muduoZ/base/LogFile.h"

namespace muduoZ{

// 'e' for O_CLOEXEC
AppendFile::AppendFile(const std::string& logFileName):file_(fopen(logFileName.c_str(), "ae")),writtenBytes_(0){
    setbuffer(file_,buffer_,sizeof(buffer_));
}

AppendFile::~AppendFile(){
    fclose(file_);
}

void AppendFile::append(const char* msg,size_t len){
    size_t written = 0;

    while (written != len){
		size_t remain = len - written;
		size_t n = write(msg + written, remain);
		if (n != remain){//为了处理写入错误
			int err = ferror(file_);
			if (err){
			fprintf(stderr, "AppendFile::append() failed %s\n", strerror(err));
			break;
			}
		}
		written += n;
    }
    writtenBytes_+=written;
}

size_t AppendFile::write(const char* msg,size_t len){
    return fwrite_unlocked(msg,1,len,file_);//返回元素总数，即字节数
}

void AppendFile::flush(){
    fflush(file_);
}


LogFile::LogFile(bool threadSafe,size_t rollSize,const std::string& logFileName)
    :logFileName_(logFileName),
	rollSize_(rollSize),
    count_(0),
    mutex_(threadSafe==true?new Mutex:NULL){
        appendFile_.reset(new AppendFile(getFileName(logFileName)));
    }

LogFile::~LogFile(){}

void LogFile::output(const char* msg,size_t len){
    if(mutex_){
        MutexLockGuard locker(*mutex_);
        appendUnlocked(msg, len);
    }
    else appendUnlocked(msg, len);
}

void LogFile::appendUnlocked(const char* msg,size_t len){
    appendFile_->append(msg,len);
    ++count_;
    if(appendFile_->getWrittenBytes()>rollSize_){//写入大小大于最大值
        rollFile();//创建新文件
    }
}

void LogFile::flush(){
    if(mutex_){
        MutexLockGuard locker(*mutex_);
        appendFile_->flush();
    }
    else appendFile_->flush();
}

void LogFile::rollFile(){
    appendFile_.reset(new AppendFile(getFileName(logFileName_)));//文件指针指向一个新文件
}

std::string LogFile::getFileName(const std::string& logFileName){
    std::string filename;

    struct timeval tv;
    time_t time;
    char str_t[26] = {0};
    gettimeofday (&tv, NULL);
    time = tv.tv_sec;
    struct tm* p_time = localtime(&time);   
    strftime(str_t, 26, " %Y-%m-%d %H:%M:%S", p_time);

	filename += logFileName;
    filename += str_t;
    //filename += ".log";
    return filename;

}

}