#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <iostream>

using namespace std;

int64_t get_current_time(){
    timeval now;
    int ret = gettimeofday(&now, NULL);
    assert(ret != -1);
 
    return now.tv_sec * 1000000 + now.tv_usec;//us
}
 
int main(int argc, char** argv){
    int ep = epoll_create(1024);
    if(ep == -1){
        printf("create epoll error!\n"); exit(-1);
    }
    
	for(int i = 0;i<10;++i){
		//int64_t start = get_current_time();

		epoll_event events[1];
		if(epoll_wait(ep, events, 1,1) == -1){
			printf("wait epoll error!\n"); exit(-1);
		}

		int64_t end = get_current_time();
		cout<<end<<endl;
	}

    close(ep);
    
    return 0;
}