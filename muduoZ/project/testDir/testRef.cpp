#include<thread>
#include<iostream>
using namespace std;

class A{
    public:
        A(){}
        A(int val):_val(val){}
        //拷贝构造参数一定是const
        A(const A& a){
            _val = a._val;
        }
        ~A(){}

        int _val;
};

void fun(A& b){
    cout<<"参数对象地址: "<<&b<<endl;
    cout<<"线程id: "<<this_thread::get_id()<<endl;
    cout<<"val: "<<b._val<<endl;
}
int main(){
    A a(10);
    thread th1(fun,ref(a));
    th1.join();
    return 0;
}