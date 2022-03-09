#include <libco/co_routine.h>
#include <libco/co_routine_inner.h>
#include <iostream>
#include <unistd.h>

using namespace std;

struct stEnv_t
{
	stCoRoutine_t* agent_routine;
	stCoCond_t* cond;
	int num = 5;
};

void* agent( void* agrs){
	co_enable_hook_sys();
	stEnv_t* envA = (stEnv_t*)agrs;
	if(envA->num>0) {
		envA->num--;
		cout<<"envA->num="<<envA->num<<" 让出cpu"<<endl;
		co_cond_timedwait(envA->cond, -1);
		cout<<"被唤醒"<<endl;
	}
	else{
		cout<<"唤醒其他协程"<<endl;
		co_cond_signal(envA->cond);
		poll(NULL, 0, 0);//1000ms
		cout<<"唤醒完成"<<endl;
	}
	if(envA->num==0){
		cout<<"执行agent任务"<<endl;
	}
}

int main(){
	stEnv_t* env = new stEnv_t;
	env->cond = co_cond_alloc();
	
	for(int i = 0;i<10;++i){
		stCoRoutine_t* agent_routine;
		co_create(&agent_routine, NULL, agent, env);
		cout<<"co i: "<<i<<endl;
		co_resume(agent_routine);//启动协程
	}	
	co_eventloop(co_get_epoll_ct(), NULL, NULL);// “调度器”的核心
	return 0;
}