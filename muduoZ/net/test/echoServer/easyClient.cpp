#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(){
	int connector = socket(AF_INET,SOCK_STREAM,0);
	
	struct sockaddr_in sock_addr_c;
	bzero((char *)&sock_addr_c, sizeof(sock_addr_c));
	sock_addr_c.sin_family = AF_INET; //地址族
	sock_addr_c.sin_addr.s_addr  = inet_addr("127.0.0.1");
	sock_addr_c.sin_port  = htons((unsigned short)8000); //端口号

	cout<<connect(connector,(struct sockaddr *)&sock_addr_c,sizeof(sockaddr));
	
	string a;
	while(true){
		cin>>a;
		send(connector,a.c_str(),sizeof(a),0);
	}
	shutdown(connector,SHUT_RDWR);

	return 0;
}