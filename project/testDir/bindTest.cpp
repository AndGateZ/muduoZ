#include<string>
#include<iostream>
#include<memory>
#include<functional>

using namespace std;

void test(shared_ptr<string> p){
	cout<<*p<<endl;
	cout<<p.use_count()<<endl;
}

int main(){
	shared_ptr<string> p(new string("aa"));
	cout<<p.use_count()<<endl;//1
	
	{
		cout<<p.use_count()<<endl;//1
		function<void()> f = bind(test,p);//  bind所在的作用域内会增加一次拷贝
		cout<<p.use_count()<<endl;//2
		f();//3  调用函数内有一次引用
		cout<<p.use_count()<<endl;//2
	}
	cout<<p.use_count()<<endl;//1
    return 0;
}
