#include<string>
#include<iostream>
#include<memory>
#include<map>

using namespace std;

map< int,shared_ptr<string> > m;

void test(){
	shared_ptr<string> p(new string("aa"));
	m[1] = p;
	cout<<m[1]<<endl;
	cout<<*m[1]<<endl;
}

int main(){
	test();
	cout<<m[1]<<endl;
	cout<<*m[1]<<endl;
	m.erase(1);
	cout<<m[1]<<endl;
	cout<<*m[1]<<endl;
    return 0;
}

//对于指针，erase只会删除指针，不会析构指针指向的内存
//如果是对象实体，erase会析构该对象