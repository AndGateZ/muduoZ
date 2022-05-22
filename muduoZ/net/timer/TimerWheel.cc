#include <iostream>
#include <math.h>
#include <sys/wait.h>

#include "muduoZ/base/Logger.h"
#include "muduoZ/net/EventLoop.h"
#include "muduoZ/net/timer/TimerWheel.h"

namespace muduoZ{

namespace net{

typedef std::shared_ptr<Timer> TimerPtr;

//timer位置的结构体，在那一层的哪个槽中
struct wheelPos{
	int slotToAdd;
	int level;
};

wheelPos calculatePosByTicks(size_t ticks, int slotNum){
	wheelPos pos;
	//计算层数和该层的槽位置
	int level = 0;
	size_t ticksTmp = ticks / slotNum;
	size_t slotToAdd = ticks;
	while(ticksTmp>0){
		slotToAdd = ticksTmp;
		ticksTmp = ticksTmp / slotNum;
		level++;
	}
	pos.slotToAdd = slotToAdd;
	pos.level = level;
	return pos;
}

////TimerWheel
void TimerWheel::addTimer(TimerPtr timer){
	slotVec_[timer->getSlot()].push_back(timer);
}

void TimerWheel::addTimerToLower(TimerPtr timer){
	int tickNum = timer->getTickNum() % TickNOneSlot;
	int slot = tickNum;

	//本层不是最底层
	if(level_!=0){
		//在此层需要插入timer
		if(tickNum!=0){
			//计算该层应该tick的数量
			timer->setLevel(level_);
			slot = (slotCur_ + slot) % TimerWheels::SlotNum;
			slotVec_[slot].push_back(timer);
		}
		//此层无需插入，交给下一层
		else{
			timerWheels_->getWheel(level_-1)->addTimerToLower(timer);//向下层送timer
		}
	}
	//本层是最底层
	else {
		timer->setLevel(level_);
		slot = (slotCur_ + slot) % TimerWheels::SlotNum;//slotCur_一定为0，因为最底层转了一圈上面才会转
		slotVec_[slot].push_back(timer);
	}
	
}

void TimerWheel::delTimer(TimerPtr timer){
	slotVec_[timer->getSlot()].remove(timer);
}

void TimerWheel::delAll(){
	for(int i = 0;i<TimerWheels::SlotNum;++i){
		slotVec_[i].clear();
	}
}


void TimerWheel::tick(){
	slotCur_ = (slotCur_+1) % TimerWheels::SlotNum;
}

//必定不是最底层运行此函数
void TimerWheel::checkCurSlot(){
	if(slotVec_[slotCur_].size()!=0){
		std::list<TimerPtr>::iterator it = slotVec_[slotCur_].begin();
		while(it!=slotVec_[slotCur_].end()){
			TimerPtr timer = *it;
			timerWheels_->getWheel(level_-1)->addTimerToLower(timer);//向下层送timer
			it++;
		}
		slotVec_[slotCur_].clear();//清空当前槽
	}
}

//必定是最底层的运行此函数
void TimerWheel::runSlot(){
	std::list<TimerPtr>::iterator it = slotVec_[slotCur_].begin();
	//遍历当前槽的每一个timer
	while(it!=slotVec_[slotCur_].end()){
		TimerPtr timer = *it;
		int level = timer->getLevel();
		//level=0已经在最底层的轮子，level>0说明需要向下换轮子
		//必定是在底层
		if(level==0) {
			
			timer->callFunc();//最低的轮子执行
			//timerWheels_->submitTask(timer->getFunc());
			//如果是重复的定时器
			if(timer->isRepeated()){
				size_t interval = timer->getInterval();
				timer->setTickNum(interval);
				
				//计算层数和该层的槽位置
				wheelPos pos = calculatePosByTicks(interval,TimerWheels::SlotNum);
				int slotCur =  timerWheels_->getWheel(pos.level)->getSlotCur();
				int slot = (slotCur + pos.slotToAdd) % TimerWheels::SlotNum;

				//赋值
				timer->setSlot(slot);
				timer->setLevel(pos.level);
				//派送timer
				timerWheels_->getWheel(pos.level)->addTimer(timer);
			}
		}
		it++;
	}
	slotVec_[slotCur_].clear();//清空当前槽
}

////TimerWheels

TimerWheels::TimerWheels(int maxLevel)
	:quit_(true),
	mutex_(){
	if(maxLevel<0) {
		//LOG<<"TimerWheels init error";
	}
	else{
		int num = TimerWheels::SlotNum;
		for(int i = 0;i<maxLevel;++i){
			wheelVec_.push_back(std::unique_ptr<TimerWheel>(new TimerWheel(i,SlotNum,this,num)));
			num *= TimerWheels::SlotNum;
		}
	}
}

TimerPtr TimerWheels::addTimer(TimeStamp expirationTime,TimerReachFunction func,size_t interval){
	//计算总tick数
	size_t ticks = TimeStamp::timeDiffInMillSeconds(TimeStamp::now(),expirationTime);
	if(ticks > TimerMax) {
		return NULL;
		//LOG
	}
	else if(ticks<=0) ticks = 1;

	//计算层数和该层的槽位置
	wheelPos pos = calculatePosByTicks(ticks,TimerWheels::SlotNum);
	int slotCur = wheelVec_[pos.level]->getSlotCur();
	int slot = (slotCur + pos.slotToAdd) % TimerWheels::SlotNum;

	TimerPtr timer(new Timer(ticks,slot,pos.level,std::move(func),interval));

	//addtimerinloop
	MutexLockGuard locker(mutex_);//开放给多线程的接口需要加锁
	wheelVec_[timer->getLevel()]->addTimer(timer);
	return timer;
}

void TimerWheels::delTimer(TimerPtr timer){
	MutexLockGuard locker(mutex_);//开放给多线程的接口需要加锁
	wheelVec_[timer->getLevel()]->delTimer(timer);
}

//先从高到低转移timer，然后执行最底层的timer，最后从低到高tick,一次tick耗费100us
void TimerWheels::tick(){
	for(int i = wheelVec_.size()-1;i>0;--i){
		wheelVec_[i]->checkCurSlot();
	}
	wheelVec_[0]->runSlot();

	for(int i = 0;i<wheelVec_.size();++i){
		wheelVec_[i]->tick();
		if(wheelVec_[i]->getSlotCur()!=0) break;
	}
}

//这里不应该用sleep,因为任务函数的执行时间会造成误差，应该用tventloop+timerFd来获取tick来源
void TimerWheels::run(){
	quit_ = false;
	while(!quit_){
		//usleep(1000);//睡眠
		 sleep(1);
		tick();
	}
}


void TimerWheels::delAll(){
	for(int i = 0;i<wheelVec_.size();++i){
		wheelVec_[i]->delAll();
	}
}


}

}