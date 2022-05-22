# High-performance TCP service framework
基于epoll的网络编程框架，采用reactor模式，日志采用双缓冲机制，定时器采用多层时间轮，项目参考muduo、linyacool/WebServer  
<br />

## 项目介绍  
___
### 日志系统  
日志系统以异步的方式运行，分为前端和后端两个部分  
  1. 前端:  
  前端采用临时logger对象输入信息，对象内部stream对信息进行处理格式化，处理完毕后调用后端api，交由后端提供输入到磁盘  
  2. 后端:   
  提供开放给前端（多线程）的api，以异步的方式运行，后端也分为两个部分，第一个部分临时存储来自前端的数据，第二个部分定时、检测存满后写入磁盘，两个部分各有两个buffer作为临时缓冲  
<br />

### 多线程网络框架  
多线程网络框架可被分成两层  
  1. EventLoop-Channel-Poller  
  loop的核心底层架构，由上层实体为channel设置回调函数、监听类型，并交由loop中唯一对应的poller进行底层更新，监听到消息后唤醒poll，poll返回活动的channels，最终在loop()中执行channel回调  
  2. EventLoopThread-EventLoopThreadPool   
  用线程包装loop，线程池对loops进行调度，此时整个底层网络框架初具雏形  
<br />

### Tcp网络库
在one loop per thread的框架下开发多线程网络服务器，TcpServer就是一个典型用例，采用reactor模式对请求进行响应    
再往上就涉及到对数据流进行业务处理的业务开发，例如WebHTTPSever  
  1. TcpServer-Acceptor-Connection  
  2. TcpClient-Connector  
详见：[TcpServer笔记](https://github.com/AndGateZ/muduoZ/blob/master/record/TcpServer%E7%AC%94%E8%AE%B0.md)
<br />

### HttpServer
在TCP网络库的基础上开发Http请求响应工具，包括request和response
  1. HttpRequest  
  http请求类的封装，采用状态机解析请求的各部分
  2. HttpResponse  
  http响应类的封装，对请求进行业务处理后，返回响应
  3. Buffer
  connection需要有一对缓冲buffer，以便在固定的动态buffer中处理发送、接收数据，增加效率  
  详见：[BUffer](https://github.com/AndGateZ/muduoZ/blob/master/record/Buffer.md)
<br />

### 定时器-TimerWheels
定时器采用多层时间轮机制，单独作为一个线程工作，以sleep作为tick来源（有bug，未来修改）  
注意：此方案有明显bug，正确的方案应该是在工作loop中嵌入时间轮，epoll的阻塞参数作为定时触发，无需严格的tick来源，每次唤醒epollwait后处理前后时间区间内的slot即可
- 数据结构：数组+链表  
详见：[时间轮](https://github.com/AndGateZ/muduoZ/blob/master/record/%E6%97%B6%E9%97%B4%E8%BD%AE.md)

<br />

## 压力测试
___
* 压力测试工具：WebBench 1.5  
* 测试结果：QPS：最高1.6w  
详见：[压测分析](https://github.com/AndGateZ/muduoZ/blob/master/record/%E5%8E%8B%E6%B5%8B.md)
<br />

如有错误，欢迎交流指正
