
#include <map>
#include <string>

#include "muduoZ/net/tcpServer/Buffer.h"
#include "muduoZ/base/TimeStamp.h"

using std::string;
using std::map;

namespace muduoZ{

namespace net{

class HttpResponse{
public:
	enum HttpStatusCode{
		Unknown,
		C200Ok = 200,
		C301MovedPermanently = 301,
		C400BadRequest = 400,
		C404NotFound = 404
	};
	//enum Version { Http10, Http11 };

	HttpResponse(bool keepConnection);

	void addToBuffer(Buffer* outputBuf);

	//set
	void setCode( HttpStatusCode code ){ statusCode_ = code; }
	void setCodeMsg( string msg ) { codeMsg_ = msg; }
	void addHeader( string& header, string& value ) { headers_[header] = value;  }
	void setBody(string& body) { body_ = body; }
	void setKeepConnection(bool keep) { keepConnection_ = keep; }

private:
	HttpStatusCode statusCode_;
	string codeMsg_;
	map<string,string> headers_;
	bool keepConnection_;
	string version_;
	string body_;
};

}

}