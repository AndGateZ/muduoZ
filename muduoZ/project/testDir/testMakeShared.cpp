#include<iostream>
#include<memory>
#include<functional>


void f(){
    std::cout<<"here"<<std::endl;
}
void f1(){
    std::cout<<"here1"<<std::endl;
}

int main(){
    std::function<void()> fun1 = f1;
    std::shared_ptr<std::function<void()>> share(new std::function<void()>(f));
    std::shared_ptr<std::function<void()>> share1 = std::make_shared<std::function<void()>>(fun1);
    (*share)();
    (*share1)();
}