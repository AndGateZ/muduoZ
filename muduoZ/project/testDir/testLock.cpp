#include<thread>
#include<iostream>
#include<mutex>
using namespace std;

void fun1(int& num,mutex& mu){
    for(int i = 0;i<5000;i++){
        unique_lock<std::mutex> guard(mu);
        num+=i;
    }
}
void fun2(int& num,mutex& mu){
    for(int i = 5000;i<10000;i++){
        unique_lock<mutex> guard(mu);
        num+=i;

    }
}
int fun() {
	int sum = 0;
	for (int i = 0; i < 10000; i++) {
		sum += i;
	}
	return sum;
}
int main(){
    mutex mu;
    int num = 0;
    thread t1(fun1,ref(num),ref(mu));
    thread t2(fun2,ref(num),ref(mu));
    t1.join();
    t2.join();
    cout<<"thread: "<<num<<endl;
    cout<<"fun: "<<fun()<<endl;
}


//测试1：mutex
//测试2：lock_guard
//测试3：unique_lock