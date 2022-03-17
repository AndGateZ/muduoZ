# muduoZ  
基于epoll的webserver，采用reactor模式，日志采用双缓冲机制，定时器采用多层时间轮，项目参考muduo、linyacool/WebServer  
<br />

## 项目介绍  
### 日志系统  
日志系统以异步的方式运行，分为前端和后端两个部分  
1. 前端:  
We have a temporary logger object record the info and use the API of backend for recording info in file.  
前端采用临时logger对象输入信息，对象内部stream对信息进行处理格式化，处理完毕后调用后端api，交由后端提供输入到磁盘  
2. 后端:   
提供开放给前端（多线程）的api，以异步的方式运行，后端也分为两个部分，第一个部分临时存储来自前端的数据，第二个部分定时、检测存满后写入磁盘，两个部分各有两个buffer作为临时缓冲  
<br />

### 多线程网络框架  
多线程网络框架可被分成三个部分  
1. EventLoop-Channel-Poller  
loop的核心底层架构，由上层实体为channel设置回调函数、监听类型，并交由loop中唯一对应的poller进行底层更新，监听到消息后唤醒poll，poll返回活动的channels，最终在loop()中执行channel回调
2. 
<br />

### 定时器-TimerWheels
定时器采用多层时间轮机制，以eventloop的epoll的timeout作为tick来源  
- 数据结构：数组+链表
一个时间轮拥有固定长度的数组，数组的每个位置称为一个槽，槽中存放链表
- 核心：tick
TimerWheels的tick()分为三个步骤：
1. 先从高轮到低轮转移timer（最底层时间轮不参与）,用于将高轮到期的timer放置到合适的位置
2. 对最底层轮的激活槽进行处理，用于执行最底层需要被处理的槽内链表所有的timer
3. 从低到高进行tick操作，时间步进，注意需要底层转一圈上一层才能步进
- 
