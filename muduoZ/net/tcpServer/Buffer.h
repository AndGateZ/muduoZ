#ifndef _MUDUOZ_NET_BUFFER_
#define _MUDUOZ_NET_BUFFER_

#include <vector>
#include <string>

#include "muduoZ/base/copyable.h"

namespace muduoZ{

namespace net{

class Buffer:copyable{
public:
	Buffer(size_t size = 1024);

	//~Buffer();

	//get
	size_t getWriteableLen() const { return buffer_.size() - writeIndex_;  }
	size_t getReadableLen() const { return writeIndex_ - readIndex_; }
	size_t getPreEmptyLen() const { return readIndex_; }
	const char* peek() const { return &*buffer_.begin()+readIndex_; }
	const char* getWritePos() const { return &*buffer_.begin()+writeIndex_; }

	void append(const char* msg, int len);
	void append(const std::string& str);
	ssize_t readFd(int sockFd,int* savedErrno);
	ssize_t writeFd(int sockFd,int* savedErrno);

	void takeAll() { readIndex_ =  writeIndex_;}

	void haveReadForLen(int len) { readIndex_ +=len; }
	void haveReadAll() { readIndex_ =writeIndex_; }

private:
	void makeSpace(size_t len);

	std::vector<char> buffer_;
	size_t writeIndex_;//写入的位置
	size_t readIndex_;//可读的位置


};

}

}



#endif