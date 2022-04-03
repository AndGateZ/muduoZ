#ifndef _MUDUOZ_BASE_FIXEDBUFFER_
#define _MUDUOZ_BASE_FIXEDBUFFER_

#include <iostream>
#include <string.h>
#include "muduoZ/base/uncopyable.h"

namespace muduoZ{

const int bigBufferSize = 4000*1000;//AsyncLogging用这个，一个buffer存1000条信息
const int smallBufferSize = 4000;//Logger用这个

template<int SIZE>
class FixedBuffer : uncopyable{
public:
    //用data的指针位置初始化cur
    FixedBuffer():cur_(data_){}
    ~FixedBuffer(){}

	/* 固定长度buffer接口 */
    void append(const char* msg, size_t len){
        if(static_cast<int>(len) < getRemainLen()){
            //char类型的字节数就是1，无需sizeof
            memcpy(cur_,msg,len);
            cur_+=len;
        }
        //这里不考虑数据超出的情况,在调用方进行截断处理
    }


	/* 通用接口 */
    void addCurPos(size_t len){ cur_+=len;}

    //数据重置,<string.h>,参数是：字符串指针、赋值、长度
    void bZero(){ memset(data_,0,SIZE); }

    //const
    const char* getData() const {return data_;}

    char* current() {return cur_;}

    //获取数据长度：指针相减之后强制转换,用const保证不修改数据
    int getDataLen() const {return static_cast<int>(cur_ - data_);}

    //获取未使用的空间大小,返回的是字节数
    int getRemainLen(){ return static_cast<int>(end() - cur_);}

    void resetCur(){ cur_ = data_; }


	

private:
    //返回数据末尾的位置指针，这里是data_指针加整数，加的是字节数
    const char* end() const {return data_+sizeof(data_);}

    //成员：
    char data_[SIZE];
    char* cur_;
};

}


#endif