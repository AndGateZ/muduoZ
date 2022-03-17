
#include <stdio.h>

#include "muduoZ/net/EventLoopThreadPool.h"

namespace muduoZ{

namespace net{

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop,const std::string name)
	:baseLoop_(baseLoop),
	name_(name){}

void EventLoopThreadPool::start(const ThreadInitCallBack &cb){
	if(threadNum_>0){
		for(int i = 0;i<threadNum_;++i){
			char buf[name_.size() + 32];
			snprintf(buf, sizeof(buf), "%s%d", name_.c_str(), i);
			EventLoopThread* t = new EventLoopThread(cb, buf);
			threads_.push_back(std::unique_ptr<EventLoopThread>(t));
			loops_.push_back(t->start());
		}
	}
	else if(cb){
		cb();
	}
	
}

EventLoop* EventLoopThreadPool::getLoop(){
	EventLoop* loop = baseLoop_;
	if (!loops_.empty()){
		loop = loops_[nextLoopIndex_];
		++nextLoopIndex_;
		if (nextLoopIndex_ >= loops_.size()) nextLoopIndex_ = 0;
	}
  return loop;
}

}

}
