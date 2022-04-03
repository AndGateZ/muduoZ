
#include "muduoZ/net/HttpServer/HttpResponse.h"
#include "muduoZ/base/Logger.h"

namespace muduoZ{

namespace net{

HttpResponse::HttpResponse(bool keepConnection)
	:keepConnection_(keepConnection),
	statusCode_(Unknown),
	version_("HTTP/1.1"){}

void HttpResponse::addToBuffer(Buffer* outputBuf){
	char buf[14];
	snprintf(buf,sizeof(buf),"%s %d ",version_.c_str(),statusCode_);
	outputBuf->append(buf);
	outputBuf->append(codeMsg_);
	outputBuf->append("\r\n");

	if (keepConnection_){
		outputBuf->append("Connection: Keep-Alive\r\n");
  	}
	else{
		outputBuf->append("Connection: close\r\n");
  	}

	for (const auto& header : headers_){
		outputBuf->append(header.first);
		outputBuf->append(": ");
		outputBuf->append(header.second);
		outputBuf->append("\r\n");
	}

	outputBuf->append("\r\n");
	outputBuf->append(body_);
}


}

}