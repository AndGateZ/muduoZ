# $^ 依赖 不重复
# $@ 目标
# @ 不显示命令执行 
# -rm -:失败不停止
TARGET=loggerTest
LIBS=-lpthread
#OBJS=loggerTest.o AsyncLogging.o Condition.o CountDownLatch.o \
	CurrentThread.o FixedBuffer.o LogFile.o Logger.o LogStream.o \
	Mutex.o Thread.o uncopyable.o
OBJS=loggerTest.o ../Logger.o ../LogStream.o ../AsyncLogging.o ../Thread.o ../LogFile.o
CXXFLAGS=-I../

$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS) 
%.o:%.cc
	$(CXX) $(CXXFLAGS) -c $^ -o $@ $(LIBS) 
clean :
	rm $(OBJS) $(TARGET)
.PHONY: clean