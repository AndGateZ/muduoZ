
#include <unistd.h>
#include <iostream>
#include <thread>
#include <sys/time.h>
#include <benchmark/benchmark.h>

#include "muduoZ/net/timer/TimerWheel.h"
#include "muduoZ/base/TimeStamp.h"
#include "muduoZ/base/Logger.h"
#include "muduoZ/net/EventLoop.h"
#include "muduoZ/net/EventLoopThreadPool.h"

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

void init(){
	//LOG<<"init";
}

int main(){
	EventLoop baseLoop;
	EventLoopThreadPool pool(&baseLoop,"baseLoop",true);

	pool.start(init);
	
	while(true){
		// int64_t start = get_current_time();
		// int64_t end = get_current_time();
		// cout<<end-start<<endl;
		usleep(1000);
		// sleep(1);
		pool.runAfter(1,callback);
		
	}

}