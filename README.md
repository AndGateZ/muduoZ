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
多线程网络框架可被分成三层  
  1. EventLoop-Channel-Poller  
  loop的核心底层架构，由上层实体为channel设置回调函数、监听类型，并交由loop中唯一对应的poller进行底层更新，监听到消息后唤醒poll，poll返回活动的channels，最终在loop()中执行channel回调  
  2. EventLoopThread-EventLoopThreadPool   
  用线程包装loop，线程池对loops进行调度，此时整个底层网络框架初具雏形  
  3. Acceptor-Connection-TcpServer、TcpClient  
  在one loop per thread的框架下开发多线程网络服务器，TcpServer就是一个典型用例  
  再往上就涉及到业务开发，例如WebHTTPSever，对数据流进行业务处理
<br />

### 定时器-TimerWheels
定时器采用多层时间轮机制，单独作为一个线程工作，以sleep作为tick来源  
- 数据结构：数组+链表  
一个时间轮拥有固定长度的数组，数组的每个位置称为一个槽，槽中存放链表
- 核心：tick  
TimerWheels的tick()分为三个步骤：
  1. 先从高轮到低轮转移timer（最底层时间轮不参与）,用于将高轮到期的timer放置到合适的位置
  2. 对最底层轮的激活槽进行处理，用于执行最底层需要被处理的槽内链表所有的timer
  3. 从低到高进行tick操作，时间步进，注意需要底层转一圈上一层才能步进
- Tick来源方案：最终采纳方案3
  1. 使用eventloop中epollwait的timeout，此方案显然不行，有其他任务触发epoll后会导致tick混乱（上一个版本出现了此问题）  
  2. 使用timerfd定时，唤醒handleRead()来模拟tick，问题仍然存在，tick时间会不准确（任务队列阻塞） 
  3. 使用单独的线程来管理时间轮，问题：如果平台不支持单线程运行，将无法使用定时器  

- 性能测试：(TimerWheel为TimerWheel的线程)
1. tick时间为1ms  
  1.1  1us加入一个timer，定时为1个tick，保持1k个timer运行  
    ![image](https://user-images.githubusercontent.com/28748767/158993021-28bb0b6c-c139-41f7-8135-2f0fff1bf197.png)  
2. tick时间为1s  
  2.1  1ms加入一个timer，定时为1s，保持1k个timer运行  
    ![image](https://user-images.githubusercontent.com/28748767/159683943-0c24fe04-450f-4d66-83ad-ba5364eff49d.png)   
  2.2  10us加入一个timer，定时为1s，保持10w个timer运行   
    ![image](https://user-images.githubusercontent.com/28748767/159683842-22673bf0-be9f-41ec-821c-af05cbd2c2f5.png)  
3. muduo源码中的timer测试  
  3.1  1ms加入一个timer，定时为1s，保持1k个timer运行  
    ![image](https://user-images.githubusercontent.com/28748767/158811942-324b8f36-6b64-4a35-935b-39228048264f.png)  
  3.2  10us加入一个timer，定时为1s，保持10w个timer运行  
    ![image](https://user-images.githubusercontent.com/28748767/158812004-3a05b70d-e898-41ba-9843-afb2aab2ffd1.png)  
- 负载分析：
1. muduo的addtimer线程负载比本项目的高了很多  
  原因仍不明确，测试得到如下结果：  
   - muduo addtimer线程：runafter耗时20us，可能原因是其返回了一个新的TimerId对象  
   - 本项目 addtimer线程：runafter耗时10us  
- 误差分析：  
设定tick为1ms：
  - usleep的误差：设定1ms，每sleep一次平均滞后50us  
  - 1000次tick：50us*1000=误差0.05s
  - 一次1s的定时：误差0.05s，20次定时会出现一次1s的误差
  - 20次1s定时：大概率出现一次1s的误差  
  
结论：epoll的timeout为1ms将造成较大误差，因此将timeout设定为1s，能有效避免误差

