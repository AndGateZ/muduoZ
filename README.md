# muduoZ  
基于epoll的webserver，采用reactor模式，日志采用双缓冲机制，定时器采用多层时间轮，项目参考muduo、linyacool/WebServer  
<br />

## 项目介绍  
### 日志系统  
日志系统以异步的方式运行，分为前端和后端两个部分  
  1. 前端:  
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
- 性能测试：(pid小的为loop线程，大的为addtimer的子线程)
1. tick时间为1ms  
  1.1  1us加入一个timer，定时为1个tick，保持1k个timer运行
  ![image](https://user-images.githubusercontent.com/28748767/158811288-7bc5b7a1-370e-4504-ba36-2d751938d9c2.png)  
2. tick时间为1s
  2.1  1ms加入一个timer，定时为1s，保持1k个timer运行
  ![image](https://user-images.githubusercontent.com/28748767/158811767-dec4b65b-6d7e-43a3-90df-da7faea9c929.png)  
  2.2  10us加入一个timer，定时为1s，保持10w个timer运行  
  ![image](https://user-images.githubusercontent.com/28748767/158811779-9af820b2-2aab-4d09-885d-c0c783bfa834.png)  
3. muduo源码中的timer测试
  3.1  1ms加入一个timer，定时为1s，保持1k个timer运行  
  ![image](https://user-images.githubusercontent.com/28748767/158811942-324b8f36-6b64-4a35-935b-39228048264f.png)  
  3.2  10us加入一个timer，定时为1s，保持10w个timer运行  
  ![image](https://user-images.githubusercontent.com/28748767/158812004-3a05b70d-e898-41ba-9843-afb2aab2ffd1.png)  
- 负载分析：
1. muduo的addtimer线程负载比本项目的高了很多  
  原因仍不明确，测试得到如下结果：  
   - muduo addtimer线程：runafter耗时20us，可能原因是其返回了一个新的TimerId对象  
   - 本项目 addtimer线程：runafter耗时5us  
2. loop线程中本项目的负载较低  
   - muduo loop线程：添加timer的过程为两个小根堆的插入，时间复杂度为O(logn)
   - 本项目 loop线程：直接把指针插入列表，时间复杂度为O(1)
- 误差分析：  
当设定epoll的timeout为1ms：
  - epollwait的延迟：每poll一次平均延迟100us  
  - 1000次tick：100us*1000=误差0.1s  
  - 一次1s的定时：误差0.1s  
  - 10次1s定时：大概率出现一次1s的误差
结论：epoll的timeout为1ms将造成较大误差，因此将timeout设定为1s，能有效避免误差

