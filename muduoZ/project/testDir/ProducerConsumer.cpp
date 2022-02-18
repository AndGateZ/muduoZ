#include<iostream>
#include<thread>
#include<pthread.h>
#include<mutex>
#include<condition_variable>
#include<unistd.h>

using namespace std;

#define PRODUCERNUM 4
#define CONSUMERNUM 1
#define RECORESIZE 10


struct Resource{
    mutex topMutex;
    condition_variable conEmpty;
    condition_variable conFull;

    int resource[RECORESIZE];
    size_t consumerPos;
    size_t producerPos;

    //记忆结构体初始化写法
    Resource():consumerPos(0),producerPos(0){}
};

/*
//thread风格
//生产者行为：如果缓冲区满，等待自己被消费者唤醒；生产，index++；唤醒消费者
void producer(Resource& resource){
    while(true){
        sleep(random()%10);
        unique_lock<mutex> lock(resource.topMutex);
        while((resource.producerPos+1)%RECORESIZE==resource.consumerPos){
            resource.conFull.wait(lock);
            //流程：一开始是空的，进入wait，lock锁释放；被消费者唤醒，获得锁，判断不空，离开while
        }

        //生产产品
        resource.resource[resource.producerPos] = random()%100;
        cout<<"thread: "<<this_thread::get_id()<<" make thing: "<<resource.resource[resource.producerPos]<<endl;;

        //计算位置
        if(resource.producerPos==RECORESIZE-1) resource.producerPos = 0;
        else resource.producerPos++;

        //通知
        resource.conEmpty.notify_one();
        lock.unlock();//需要手动解锁
    }
    
}


//消费者行为：如果缓冲区空，等待自己被生产者唤醒；消费，index++；唤醒生产者
void consumer(Resource& resource){
    while(true){
        sleep(random()%10);
        unique_lock<mutex> lock(resource.topMutex);
        while(resource.consumerPos==resource.producerPos){
            resource.conEmpty.wait(lock);
            //流程：一开始是满的，进入wait，lock锁释放；被唤醒，获得锁，判断不满，离开while
        }

        //拿走产品
        cout<<"thread: "<<this_thread::get_id()<<" take thing: "<<resource.resource[resource.consumerPos]<<endl;;

        //计算位置
        if(resource.consumerPos==RECORESIZE-1) resource.consumerPos = 0;
        else resource.consumerPos++;

        //通知
        resource.conFull.notify_one();
        lock.unlock();//需要手动解锁
    }
    
}
//thread风格

int main(){
    Resource resource;
    thread producer1(producer,ref(resource));
    thread producer2(producer,ref(resource));
    thread producer3(producer,ref(resource));
    thread producer4(producer,ref(resource));

    thread consumer1(consumer,ref(resource));
    thread consumer2(consumer,ref(resource));
    thread consumer3(consumer,ref(resource));
    thread consumer4(consumer,ref(resource));

    producer1.join();
    producer1.join();
    producer1.join();
    producer1.join();

    consumer1.join();
    consumer2.join();
    consumer3.join();
    consumer4.join();

    return 0;
}*/




//pthread风格
//生产者行为：如果缓冲区满，等待自己被消费者唤醒；生产，index++；唤醒消费者
void* producer(void* obj){
    Resource* resource = static_cast<Resource*>(obj);
    while(true){
        sleep(random()%10);
        unique_lock<mutex> lock(resource->topMutex);
        while((resource->producerPos+1)%RECORESIZE==resource->consumerPos){
            resource->conFull.wait(lock);
            //流程：一开始是空的，进入wait，lock锁释放；被消费者唤醒，获得锁，判断不空，离开while
        }

        //生产产品
        resource->resource[resource->producerPos] = random()%100;
        cout<<"thread: "<<this_thread::get_id()<<" make thing: "<<resource->resource[resource->producerPos]<<endl;;

        //计算位置
        if(resource->producerPos==RECORESIZE-1) resource->producerPos = 0;
        else resource->producerPos++;

        //通知
        resource->conEmpty.notify_one();
        lock.unlock();//需要手动解锁
    }
    
}


//消费者行为：如果缓冲区空，等待自己被生产者唤醒；消费，index++；唤醒生产者
void* consumer(void* obj){
Resource* resource = static_cast<Resource*>(obj);
    while(true){
        sleep(random()%10);
        unique_lock<mutex> lock(resource->topMutex);
        while(resource->consumerPos==resource->producerPos){
            resource->conEmpty.wait(lock);
            //流程：一开始是满的，进入wait，lock锁释放；被唤醒，获得锁，判断不满，离开while
        }

        //拿走产品
        cout<<"thread: "<<this_thread::get_id()<<" take thing: "<<resource->resource[resource->consumerPos]<<endl;;

        //计算位置
        if(resource->consumerPos==RECORESIZE-1) resource->consumerPos = 0;
        else resource->consumerPos++;

        //通知
        resource->conFull.notify_one();
        lock.unlock();//需要手动解锁
    }
    
}

int main(){
    pthread_t t1;
    pthread_t t2;
    pthread_t t3;
    pthread_t t4;
    pthread_t t5;
    pthread_t t6;
    pthread_t t7;
    pthread_t t8;
    Resource* resource = new Resource;
    pthread_create(&t1,NULL,&producer,resource);
    pthread_create(&t2,NULL,&producer,resource);
    pthread_create(&t3,NULL,&producer,resource);
    pthread_create(&t4,NULL,&producer,resource);

    pthread_create(&t5,NULL,&consumer,resource);
    pthread_create(&t6,NULL,&consumer,resource);
    pthread_create(&t7,NULL,&consumer,resource);
    pthread_create(&t8,NULL,&consumer,resource);

    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
    pthread_join(t4,NULL);
    pthread_join(t5,NULL);
    pthread_join(t6,NULL);
    pthread_join(t7,NULL);
    pthread_join(t8,NULL);

    return 0;
}
