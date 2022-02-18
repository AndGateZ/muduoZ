#include<string>
#include<iostream>
#include<memory>

using namespace std;

void test(){

}

int main(){
    shared_ptr<string> p(new string("aa"));
    return 0;
}