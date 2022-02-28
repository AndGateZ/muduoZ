#include<iostream>

using namespace std;

int main(){
    int p[3] = {0};
    cout<<sizeof(p)<<endl;//12
    cout<<sizeof(p[1])<<endl;//4
    cout<<sizeof(&p)<<endl;//8,p是指针，指针的地址大小是8字节，64位的电脑

    cout<<"========"<<endl;

    char a[3] = {'0'};
    cout<<sizeof(a)<<endl;//3
    cout<<sizeof(a[1])<<endl;//1
    cout<<sizeof(&a)<<endl;//8

    return 0;
}