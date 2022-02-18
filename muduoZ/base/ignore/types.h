#ifndef _MUDUOZ_BASE_TYPES_
#define _MUDUOZ_BASE_TYPES_

#include <stdint.h>  //定义整形类型 int16_t 代替short等
#include <string.h>  //memset
#include <string>

#ifndef NDEBUG
#include <assert.h>
#endif

namespace muduoZ{
    
using std::string;

inline void memZero(void* p,size_t n){//封装了一下memset
    memset(p,0,n);
}

template<typename To,typename From>
inline To implicit_cast(From const &f){//最好用于向上转换
    return f;
}

template<typename To,typename From>
inline To down_cast(From const &f){//向下转换，需要注意转换安全
    //debug模式
    #if !defined(NDEBUG) && !defined(GOOGLE_PROTOBUF_NO_RTTI)
    assert(f==NULL || dynamic_cast<To>(f) !=NULL );//要么f是空的，或者转换后不是NULL，都表明转换成功
    #endif
    return static_cast<To> (f);
}

}


#endif