
#include <iostream>

using namespace std;

int main(){
	int p[10];
	int *x = p;
	cout<<sizeof(p)<<endl;
	cout<<sizeof(*p)<<endl;
	cout<<sizeof(&p)<<endl;
	cout<<sizeof(x)<<endl;
	cout<<sizeof(*x)<<endl;
	cout<<sizeof(&x)<<endl;

}