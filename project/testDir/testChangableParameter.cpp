#include <iostream>
#include <cstdarg>

using namespace std;

/*
c语言可变模板参数的宏：
va_arg：宏定义，用来获取下一个参数
va_start：宏定义，开始使用可变参数列表
va_end：宏定义，结束使用可变参数列表
va_list：类型，存储可变参数的信息
*/

void cChangableParameter(int n,...){
    va_list args;
    va_start(args, n);
    while(n--){
        std::cout<<va_arg(args, int)<<" ";
    }
    va_end(args);
    std::cout<<std::endl;
}


void cChangableParameter1(int n,...){
    va_list args;
    va_start(args, n);
    while(n--){
         if(n == 0){
            std::cout<<va_arg(args, const char*)<<" ";
            continue;
        }
        std::cout<<va_arg(args, int)<<" ";
    }
    va_end(args);
    std::cout<<std::endl;
}

template<typename T>
void cPlus(T arg){
    cout<<arg<<endl;
}

template<typename TFirst,typename... T>
void cPlus(TFirst arg,T... args){
    cout<<arg<<endl;
    cPlus(args...);
}

template<typename TFirst,typename... T>
void cPlusRuntime(TFirst arg,T... args){
    cout<<arg<<endl;
    //变量获得在编译阶段即可计算出结果的能力
    if constexpr(sizeof...(args) > 0){
        cPlusRuntime(args...);
    }
}

int main(){
    //printf("%sthis is %d\n","what",11);
    //cChangableParameter(4,2,3,4,5);
    //cChangableParameter1(4,2,3,4,"here");
    cPlus(1,2,3,"hehe");//递归读完参数，用重载来结束
    cPlusRuntime(1,2,3,"hehe"); //编译期if表达式constexpr，-std=c++17
    return 0;
}