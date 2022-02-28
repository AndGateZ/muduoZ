#ifndef _THREADPOOL_
#define _THREADPOOL_

#include<iostream>
#include<vector>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<future>

#include"safeQueue.h"


class ThreadPool{
    private:
        class ThreadWorker{
            private:
                size_t w_id;
                ThreadPool* threadPool;
            public:
                //id前加const
                ThreadWorker(ThreadPool* pool,size_t id):w_id(id),threadPool(pool){}
                //重载()，作用是从线程池中拿一个任务出来执行
                void operator()();
        };

        bool shutDownFlag;
        //这里为什么是void,因为bind之后参数都指定了
        SafeQueue<std::function<void()>> que;
        std::vector<std::thread> threads;
        std::mutex mute;
        std::condition_variable cv;

    public:
        //默认参数
        ThreadPool(const size_t threadNum):threads(std::vector<std::thread>(threadNum)),shutDownFlag(false){}
        ThreadPool(const ThreadPool &threadPool) = delete;
        ThreadPool(ThreadPool &&threadPool) = delete;
        ThreadPool &operator=(const ThreadPool &threadPool) = delete;
        ThreadPool &operator=(ThreadPool &&threadPool) = delete;
        //初始化线程的工作成员,分配成员的工作线程
        void init();
        //关闭线程池，等待所有的线程完成任务，然后彻底关闭
        void shutDown();
        //提交函数有点复杂
        template<typename Func,typename ...Args>
        auto submit(Func &&func,Args &&...args)->std::future<decltype(func(args...))>{
            //1 创建一个function func，连接函数和参数定义，特殊函数类型，避免左右值错误
            //注意最左边的写法，decltype判断返回类型，function的模板参数是：返回类型(输入类型)
            //注意最右边...的写法，在括号外
            std::function<decltype(func(args...))()> fun = std::bind(std::forward<Func>(func),std::forward<Args>(args)...);
            //2 用packaged_task包裹func，创建一个任务的智能指针                
            //decltype(func(args...))():fun的输入输出类型，注意上面bind直接绑定了所有参数，所以输入参数是空的
            //不能拆开，只能这么写，(fun)初始化packaged_task，make_shared是一个函数，返回智能指针
            auto taskPtr = std::make_shared< std::packaged_task<decltype(func(args...))()> >(fun);
            //3 创建一个function warpper,将任务指针用function<void>包裹，线程会从队列中拆箱，运行内部指针指向的函数
            std::function<void()> funWarpper = [taskPtr](){ (*taskPtr)();  }; 
            //4 将function<void>放进任务队列,会不会存在退出作用域 funWarpper被析构的情况？
            que.enqueue(funWarpper);
            //5 唤醒一个等待中的线程
            cv.notify_one();
            //6 返回future的任务指针
            return taskPtr->get_future();
        }
};



#endif