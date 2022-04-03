#include <string.h> 
#include <algorithm> 

#include "muduoZ/net/HttpServer/HttpRequest.h"

using namespace std;

namespace muduoZ{

namespace net{

const char HttpRequest::CRLF[3] = "\r\n";

HttpRequest::HttpRequest()
	:state_(ExpectRequestLine),method_(Invalid),version_(Unknown){}


bool HttpRequest::parseMessage(Buffer* buf,TimeStamp receiveTime){
	bool succeed = false;
	bool hasMore = true;
	const char* crlf;
	while(hasMore){
		if(state_ == ExpectRequestLine){
			//找到第一个回车换行
			crlf = std::search(buf->peek(),buf->getWritePos(),CRLF,CRLF+2);//返回找到CRLF的第一个字符
			//解析请求行			
			if(!parseRequestLine( buf->peek(), crlf)) hasMore = false;
			//状态修改,buf前进
			buf->haveReadForLen( crlf + 2 - buf->peek() );
			succeed = true;
			state_ = ExpectHeaders;
		}
		else if(state_ == ExpectHeaders){
			crlf = std::search(buf->peek(),buf->getWritePos(),CRLF,CRLF+2);//返回找到CRLF的第一个字符
			//找到\r\n
			if(crlf != buf->getWritePos()){
				const char* colon = std::find(buf->peek(),crlf,':');
				//找到冒号
				if(colon != crlf){
					parseHeader(buf->peek(),colon,crlf);
					buf->haveReadForLen( crlf + 2 - buf->peek() );
					state_ == ExpectBody;
				}
				//没找到冒号
				else{
					state_ = GotAll;
					hasMore = false;
				}
			}
			//没找到\r\n
			else {
				hasMore = false;
			}
		}
		else if(state_ == ExpectBody){
			state_ = GotAll;
		}
		else if(state_ == GotAll){
			buf->haveReadAll();
			hasMore = false;
		}
	}
	return succeed;
}

void HttpRequest::reset(){
	HttpRequest req;
	state_ = ExpectRequestLine;
	method_ = Invalid;
	version_ = Unknown;
	std::swap(receiveTime_,req.receiveTime_);
	path_.swap(req.path_);
	query_.swap(req.query_);
	headers_.swap(req.headers_);
}

bool HttpRequest::parseRequestLine(const char* start, const char* crlf){
	//找空格 设置method
	const char* space = std::find(start, crlf, ' ');
	if(space == crlf) return false;
	string method(start,space);
	if(!setMethod(method)){
		return false;
	}
	start = space+1;//向右挪一位
	if(start == crlf) return false;

	//找问号 设置path query   或者无问号
	const char* question = std::find(start, crlf, '?');
	space = std::find(start, crlf, ' ');
	if(space == crlf) return false;
	if(question == crlf){
		path_.assign(start,space);
	}
	else{
		path_.assign(start,question);
		query_.assign(question+1,space);
	}
	start = space+1;
	if(start == crlf) return false;

	//剩下的设置协议
	string version(start,crlf);
	if(!setVersion(version)){
		return false;
	}
	return true;
}

void HttpRequest::parseHeader(const char* start, const char* colon, const char* CRLF){
	string headerName(start,colon);
	colon++;
	//去除value前和后的空格
	while(colon<CRLF && *colon == ' ') colon++;
	string headerValue(colon,CRLF);
	while( !headerValue.empty() && headerValue[headerValue.length()-1] == ' ')  headerValue.resize(headerValue.length()-1);
	headers_[headerName] = headerValue;
}

//set
bool HttpRequest::setMethod(string& str){
	//Invalid, Get, Post, Put, Delete, Head 
	if(str == "GET") method_ = Get;
	else if(str == "POST") method_ = Post;
	else if(str == "PUT") method_ = Put;
	else if(str == "DELETE") method_ = Delete;
	else if(str == "HEAD") method_ = Head;
	else {
		method_ = Invalid;
		return false;
	}
	return true;
}

bool HttpRequest::setVersion(string& str){
	//Unknown, Http10, Http11
	if(str == "HTTP/1.0") version_ = Http10;
	else if(str == "HTTP/1.1") version_ = Http11;
	else {
		version_ = Unknown;
		return false;
	}
	return true;
}


}

}
