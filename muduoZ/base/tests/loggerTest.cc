#include <iostream>
#include <unistd.h>

#include "muduoZ/base/Logger.h"
#include "muduoZ/base/Thread.h"

void func(){
	int i = 0;
	while(i!=10){
		muduoZ::LOG<<"ok";
		sleep(6);
		i++;
	}
}

int main(){
    //std::cout<<__FILE__<<std::endl;
	//std::cout<<__LINE__<<std::endl;
    muduoZ::Thread t(func);
	t.start();
	t.join();
    return 0;
}
//目前问题：程序崩溃的时候，最后的存在缓存区的消息丢失，不能flush进硬盘
//已解决：定时flush(AsyncLogging里的interval自动flush)
//解决方案2：指针哨兵（不知道怎么做）

//待改善部分：AsyncLogging前端后端目前都只有两个缓冲区，可以使用尺寸固定的缓冲buffer数组，空间换时间，减少大量数据时。缓存区new带来的时间消耗