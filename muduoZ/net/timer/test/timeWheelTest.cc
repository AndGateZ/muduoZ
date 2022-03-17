
#include <unistd.h>
#include <iostream>
#include <thread>
#include <benchmark/benchmark.h>
#include "muduoZ/net/timer/TimerWheel.h"
#include "muduoZ/base/TimeStamp.h"
#include "muduoZ/base/Logger.h"

using namespace std;
using namespace muduoZ::net;
using namespace muduoZ;

void func(TimerWheels& tw){
	int a = 0;
	while(true){
		//usleep(1000);//1ms
		sleep(1);//1s
		tw.tick();
	}
}

void callback(){}

int demo(){
	TimerWheels tw;
	tw.addTimer(TimeStamp::now().addTime(size_t(3)),callback,0);
	thread th(func,ref(tw));
	th.join();
}

int demo1(TimerWheels& tw){
	tw.addTimer(TimeStamp::now().addTime(size_t(1)),callback,0);
}

static void BM_demo(benchmark::State& state) {
	TimerWheels tw;
	thread th(func,ref(tw));
    // for (auto _ : state){
	// 	while(true){
	// 		usleep(50);//1s内有2w个定时器同时存在
	// 		demo1(tw);
	// 	}/
	// }
	while(true){
		usleep(10);
		demo1(tw);
	}
	th.join();
}

// Register the function as a benchmark
// BENCHMARK(BM_demo)->Iterations(1); //指定BM_demo函数中，for循环的迭代次数, 添加20w个timer，1tick
BENCHMARK(BM_demo);
BENCHMARK_MAIN(); //程序入口
