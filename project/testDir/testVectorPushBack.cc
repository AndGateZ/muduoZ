#include<vector>
#include<memory>
#include<iostream>

using namespace std;

class A{
    public:
        A(int b):a(b){}
        int a;
};

int main(){
    unique_ptr<A> ptr(new A(2));
    vector< unique_ptr<A> > v;
    v.push_back(move(ptr));
    unique_ptr<A> ptr1(new A(3));
    ptr1 = move(v.back());
    //cout<<v[0]->a;;
    //v.pop_back();
    v.clear();
    //cout<<ptr->a<<endl;
    cout<<ptr1->a<<endl;
}