# $^ 依赖 不重复
# $@ 目标
# @ 不显示命令执行 
# -rm -:失败不停止
# := 覆盖之前的值
# ?= 如果没有被赋值过就赋予等号后面的值
# += 添加等号后面的值
# ${HOME} /root
MUDUOZ_DIRECTORY ?= $(HOME)/muduo/muduoZ
MUDUOZ_INCLUDE = $(MUDUOZ_DIRECTORY)
MUDUOZ_OBJ_BASE = $(MUDUOZ_DIRECTORY)/muduoZ/base
MUDUOZ_OBJ_NET = $(MUDUOZ_DIRECTORY)/muduoZ/net
TARGET=echoServer
LIBS=-lpthread
BENCHMARK=-lbenchmark
GDB=-g

OBJS=echoServer.o $(MUDUOZ_OBJ_BASE)/Logger.o $(MUDUOZ_OBJ_BASE)/LogStream.o $(MUDUOZ_OBJ_BASE)/AsyncLogging.o \
	$(MUDUOZ_OBJ_BASE)/Thread.o $(MUDUOZ_OBJ_BASE)/LogFile.o $(MUDUOZ_OBJ_BASE)/LogFile.o $(MUDUOZ_OBJ_BASE)/TimeStamp.o \
	$(MUDUOZ_OBJ_NET)/Channel.o $(MUDUOZ_OBJ_NET)/Epoller.o $(MUDUOZ_OBJ_NET)/EventLoop.o $(MUDUOZ_OBJ_NET)/Socket.o \
	$(MUDUOZ_OBJ_NET)/timer/TimerWheel.o $(MUDUOZ_OBJ_NET)/timer/TimerWheelsThread.o $(MUDUOZ_OBJ_NET)/EventLoopThread.o \
	$(MUDUOZ_OBJ_NET)/EventLoopThreadPool.o $(MUDUOZ_OBJ_NET)/tcpServer/Acceptor.o $(MUDUOZ_OBJ_NET)/tcpServer/Buffer.o \
	$(MUDUOZ_OBJ_NET)/tcpServer/Connection.o $(MUDUOZ_OBJ_NET)/tcpServer/TcpServer.o
CXXFLAGS=-I$(MUDUOZ_INCLUDE)

$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS) $(BENCHMARK) $(GDB) 
%.o:%.cc
	$(CXX) $(CXXFLAGS) -c $^ -o $@ $(LIBS) $(GDB)
clean :
	rm $(OBJS) $(TARGET)
.PHONY: clean