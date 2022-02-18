#ifndef _MUDUOZ_BASE_UNCOPYABLE_
#define _MUDUOZ_BASE_UNCOPYABLE_

namespace muduoZ{

class uncopyable{
public:
    uncopyable(const uncopyable&) = delete;//拷贝构造函数
    void operator=(const uncopyable&) = delete;//=号的操作符重载，返回类型是void

protected:
    uncopyable() = default;
    ~uncopyable() = default;
};

}


#endif