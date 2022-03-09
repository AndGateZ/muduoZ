#include<iostream>

class A{
public:
    A(){}
    A(int num1):num(num1){}
    A(A&a){//如果没有A(A&a)的赋值构造函数，有参数为const的，调用A a4(a1)会执行const的构造函数
        std::cout<<"1"<<std::endl;
        this->num = a.num;
    }
    A(const A&a){//如果没有A(const A&a)的赋值构造函数，有参数为const的，调用A a4(const a1)会编译失败
        std::cout<<"2"<<std::endl;
        this->num = a.num;
    }
    A& operator=(const A&a){//这里的返回类型，如果是void，则不返回，如果是A&，主要作用的连续计算a = b + c + d;
        std::cout<<"3"<<std::endl;
        this->num = a.num;
    }
    int num;
    
};

int main(){
    const A a1(1);
   // a1.num = 1;
    
    A a3 = a1;//调用拷贝构造A(const A&a)
    A a4(a1);//调用拷贝构造A(const A&a)
    A a2;
    a2 = a1;//调用等号重载,其返回类型是用户设定的
    std::cout<<a2.num;
    return 0;
}