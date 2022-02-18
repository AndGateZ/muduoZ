#include <inttypes.h>
#include <iostream>

#include"muduoZ/base/types.h"


int main(){
    char buf[64];
    char buf1[64];
    int64_t seconds = 1;
    int64_t microseconds = 2;
    //PRId64 是类型ld
    snprintf(buf, sizeof(buf), "%ld this and .%06ld"  , seconds, microseconds);
    snprintf(buf1, sizeof(buf1), "%" PRId64 ".%06" PRId64 "", seconds, microseconds);//snprintf()函数用于将格式化的数据写入字符串
    std::cout<<buf<<std::endl;
    std::cout<<buf1<<std::endl;
    return 0;
}