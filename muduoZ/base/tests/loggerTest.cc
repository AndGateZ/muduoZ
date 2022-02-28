#include <iostream>
#include <unistd.h>

#include "Logger.h"
#include "Thread.h"

void func(){
	int i = 0;
	while(i!=4){
		muduoZ::LOG<<"ok";
		sleep(3);
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