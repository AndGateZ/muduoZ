
#include <map>
#include <string>

#include "muduoZ/net/tcpServer/Buffer.h"
#include "muduoZ/base/TimeStamp.h"

using std::string;
using std::map;

namespace muduoZ{

namespace net{

class HttpRequest{
public:
	enum State{ ExpectRequestLine, ExpectHeaders, ExpectBody, GotAll };
	enum Method{ Invalid, Get, Post, Put, Delete, Head };
	enum Version { Unknown, Http10, Http11 };

	HttpRequest();

	bool parseMessage(Buffer* buf,TimeStamp receiveTime);
	void reset();

	//get
	State getState() const {return state_;}
	const string getHeader(const string& str) { return headers_[str]; }
	Version getVersion() const {return version_;}
	Method getMethod() const {return method_;}
	string getPath() const {return path_;}
	string getQuery() const {return query_;}

private:
	bool parseRequestLine(const char* start, const char* CRLF);
	void parseHeader(const char* start, const char* colon, const char* CRLF);
	bool parseBody(const char* start, const char* CRLF);

	bool setMethod(string& str);
	bool setVersion(string& str);

	State state_;
	Method method_;
	Version version_;
	string path_;
	string query_;
	TimeStamp receiveTime_;
	map<string,string> headers_;

	static const char CRLF[3];
};

}

}