
#include <iostream>
#include <functional>

#include "muduoZ/base/Thread.h"
#include "muduoZ/net/EventLoop.h"

using namespace std;

using namespace muduoZ::net;
using namespace muduoZ;


void func(EventLoop* loop){
	int i = 0;
	while(i!=10){
		sleep(6);
		loop->wakeup();
		i++;
	}
}

int main(){
	
	EventLoop* loop = new EventLoop();
	Thread t(bind(func,loop));
	t.start();
	loop->loop();
	return 0;
}