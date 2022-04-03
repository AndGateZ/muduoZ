
#include <iostream>

using namespace std;

class A{
public:
	void set(int a){num = a;}
	int num;
};

static A a;

int main(){


	a.set(1);//静态对象可以调用非静态成员
	cout<<a.num<<endl;

	return 0;
}
