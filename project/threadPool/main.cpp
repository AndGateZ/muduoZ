#include <iostream>
#include <unistd.h>
//#include "safeQueue.h"
#include "ThreadPool.h"


void job(int i,int j){
    sleep(random()%5);
    cout<<this_thread::get_id()<<"   "<<i<<" "<<j<<endl;
}


int main(){
    /*
    SafeQueue<int> que;
    cout<<"empty: "<<que.empty()<<endl;
    int a = 6;
    que.enqueue(a);
    cout<<"empty: "<<que.empty()<<endl;
    cout<<que.size()<<endl;
    int b = 2;
    cout<<"dequeue: "<<que.dequeue(b)<<endl;
    cout<<b<<endl;
    cout<<"empty: "<<que.empty()<<endl;
    return 0;
    */
    ThreadPool pool(4);
    pool.init();

    auto future = pool.submit(job,111,111);
    for(int i = 1;i<=3;++i){
        for(int j = 1;j<=3;++j){
            pool.submit(job,i,j);
        }
    }

    future.get();
    int a;
    cin>>a;
    cout<<"shutdown"<<endl;
    pool.shutDown();
    return 0;

}