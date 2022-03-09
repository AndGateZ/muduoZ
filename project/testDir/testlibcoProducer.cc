#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <libco/co_routine.h>
#include <iostream>

using namespace std;

struct stTask_t
{
	int id;
};

struct stEnv_t
{
	stCoCond_t* cond;
	queue<stTask_t*> task_queue;
};

void* Producer(void* args)
{
	co_enable_hook_sys();
	stEnv_t* env=  (stEnv_t*)args;
	int id = 0;
	while (true)
	{
		stTask_t* task = (stTask_t*)calloc(1, sizeof(stTask_t));
		task->id = id++;
		env->task_queue.push(task);
		printf("%s:%d produce task %d\n", __func__, __LINE__, task->id);
		cout<<3<<endl;
		co_cond_signal(env->cond);
		cout<<4<<endl;
		poll(NULL, 0, 1000);//1000ms
		cout<<5<<endl;
	}
	return NULL;
}

void* Consumer(void* args)
{
	co_enable_hook_sys();
	stEnv_t* env = (stEnv_t*)args;
	while (true)
	{
		if (env->task_queue.empty())
		{
			cout<<1<<endl;
			co_cond_timedwait(env->cond, -1);
			cout<<2<<endl;
			continue;
		}
		stTask_t* task = env->task_queue.front();
		env->task_queue.pop();
		printf("%s:%d consume task %d\n", __func__, __LINE__, task->id);
		free(task);
	}
	return NULL;
}

int main()
{
	stEnv_t* env = new stEnv_t;
	env->cond = co_cond_alloc();

	stCoRoutine_t* consumer_routine;
	co_create(&consumer_routine, NULL, Consumer, env);
	co_resume(consumer_routine);//启动协程

	stCoRoutine_t* producer_routine;
	co_create(&producer_routine, NULL, Producer, env);
	co_resume(producer_routine);
	
	co_eventloop(co_get_epoll_ct(), NULL, NULL);
	return 0;
}
//编译
//g++ testlibco.cc -lcolib -lpthread -ldl