#ifndef _MUDUOZ_NET_TIMERID_
#define _MUDUOZ_NET_TIMERID_

#include <memory>

#include "muduoZ/base/uncopyable.h"
#include "muduoZ/net/timer/Timer.h"

namespace muduoZ{

namespace net{

class TimerId:uncopyable{
public:
	TimerId(std::shared_ptr<Timer> timer,int64_t id)
	:timer_(timer),
	id_(id){}
	
	~TimerId();


private:
	std::shared_ptr<Timer> timer_;
  	int64_t id_;

};

}

}

#endif