#include <iostream>
#include <future>
#include <chrono>       
#include <thread>       
using namespace std;

// count down taking a second for each value:
int countdown (int from, int to) {
    for (int i=from; i!=to; --i) {
        std::cout << i << '\n';
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "Finished!\n";
    return from - to;
}

class Fctor {
public:
    Fctor(){}
    //Fctor(const Fctor&) = delete;
    void operator() () {
    }


};

int main ()
{
    int a = 10;
    std::packaged_task<int(int,int)> task(countdown); // 设置 packaged_task
    std::future<int> ret = task.get_future(); // 获得与 packaged_task 共享状态相关联的 future 对象.
    //task对象用move或者ref指针
    std::thread th(move(task),a,0);   //创建一个新线程完成计数任务.
    std::cout <<"thread start" <<endl;;
    int value = ret.get();                    // 阻塞等待任务完成并获取结果.
    std::cout << "The countdown lasted for " << value << " seconds.\n";
    th.join();

    /*
    Fctor f;
    std::thread th1(f);
    */
    return 0;
}