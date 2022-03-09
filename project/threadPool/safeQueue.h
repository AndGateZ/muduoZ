#ifndef _SAFEQUEUE_
#define _SAFEQUEUE_

#include<mutex>
#include<queue>
#include <iostream>

using namespace std;

template<typename T>
class SafeQueue{
    public:
        SafeQueue(){}
        ~SafeQueue(){}

        bool empty(){
            unique_lock<mutex> lock(_mutex);
            return _queue.empty();
        }
        void enqueue(T& t){
            unique_lock<mutex> lock(_mutex);
            _queue.emplace(t);
        }
        bool dequeue(T& t){
            unique_lock<mutex> lock(_mutex);
            if(_queue.size()==0) return false;
            t = move(_queue.front());//调用移动赋值运算符
            //cout<<"here: "<<_queue.front()<<endl;
            _queue.pop();
            return true;
        }
        int size(){
            unique_lock<mutex> lock(_mutex);
            return _queue.size();
        }

    private:
        queue<T> _queue;
        mutex _mutex;
};

#endif