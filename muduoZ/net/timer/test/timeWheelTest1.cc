
#include <unistd.h>
#include <iostream>
#include <thread>
#include <sys/time.h>
#include <benchmark/benchmark.h>

#include "muduoZ/net/timer/TimerWheel.h"
#include "muduoZ/base/TimeStamp.h"
#include "muduoZ/base/Logger.h"
#include "muduoZ/net/EventLoop.h"

using namespace std;
using namespace muduoZ::net;
using namespace muduoZ;

int64_t get_current_time(){
    timeval now;
    int ret = gettimeofday(&now, NULL);
    assert(ret != -1);
 
    return now.tv_sec * 1000000 + now.tv_usec;//us
}

void callback(){
	//LOG<<"call";
}

void func(EventLoop& loop){
	while(true){
		usleep(1000);
		//int64_t start = get_current_time();
		loop.runAfter(1,callback);
		//int64_t end = get_current_time();
		//cout<<end-start<<endl;
	}
	//for(int i = 0;i<1;++i) {
	//loop.runEvery(1,callback);
	//}
	//loop.runAfter(1000,callback);
}

int main(){
	EventLoop loop;
	thread th(func,ref(loop));
	loop.loop();
	th.join();
}