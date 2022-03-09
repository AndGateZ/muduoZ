#include <libco/co_routine.h>
#include <libco/co_routine_inner.h>
#include <iostream>
#include <unistd.h>

using namespace std;

int num = 10;//拥有社交关系的人数
int taskSum = 10;//控制协程退出的变量

struct task_t
{
    stCoRoutine_t *co;//协程控制字
	stCoCond_t* cond;//条件变量
    int id;//协程id
};

void* agent( void* agrs){
	co_enable_hook_sys();
	task_t* _coWoker = (task_t*)agrs;

	if(num>1) {
		num--;
		cout<<_coWoker->id<<": num="<<num<<" 让出cpu"<<endl;
		co_cond_timedwait(_coWoker->cond, -1);
		cout<<_coWoker->id<<" 被唤醒"<<endl;
	}
	else{
		cout<<_coWoker->id<<" 唤醒其他协程"<<endl;
		// co_cond_signal(_coWoker->cond);
		co_cond_broadcast(_coWoker->cond);
		poll(NULL, 0, 0);//1000ms
		cout<<_coWoker->id<<" 唤醒完成"<<endl;
	}
	taskSum--;
	cout<<_coWoker->id<<" 执行agent任务"<<endl;
	return NULL;
}

int end(void * arg)
{
	int* taskSum = (int*)arg;
	if(*taskSum==0) return -1;
}

int main(){

	task_t _coWoker[10]={0};
	stCoCond_t* cond = co_cond_alloc();
	
	for(int i = 0;i<10;++i){
		_coWoker[i].id = i;
		_coWoker[i].cond = cond;
		co_create(&_coWoker[i].co, NULL, agent, &_coWoker[i]);
		co_resume(_coWoker[i].co);//启动协程
	}	
	co_eventloop(co_get_epoll_ct(), end, &taskSum);// “调度器”的核心
	cout<<"结束"<<endl;
	return 0;
}