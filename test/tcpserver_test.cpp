#include <iostream>
#include <string.h>

#include "../tcpserver.h"

using namespace std;

void ReadCallback(std::shared_ptr<Channel> ptChannel);
void WriteCallback(std::shared_ptr<Channel> ptChannel);
void ErrorCallback(std::shared_ptr<Channel> ptChannel);

const char * servIp = "127.0.0.1";
const int port = 9508;

int main()
{
	TcpServer server(servIp, port);
	server.SetReadCallback(ReadCallback);
	server.SetWriteCallback(WriteCallback);
	server.SetErrorCallback(ErrorCallback);
	server.Loop();
}

void ReadCallback(std::shared_ptr<Channel> ptChannel)
{
	cout << "read from " << ptChannel->GetFd() << endl;
	SimpleBuffer & inBuffer = ptChannel->GetInBuffer();
	SimpleBuffer & outBuffer = ptChannel->GetOutBuffer();
	cout << inBuffer.Buffer() << endl;
	inBuffer.ReadFromBuffer(inBuffer.BufferSize());
	
	const char * respond = "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\nContent-Length:13\r\n\r\nHello World\r\n";
	outBuffer.WriteToBuffer(respond, strlen(respond));
}

void WriteCallback(std::shared_ptr<Channel> ptChannel)
{
	cout << "write from " << ptChannel->GetFd() << " done" << endl;
}

void ErrorCallback(std::shared_ptr<Channel> ptChannel)
{
	cout << "error from " << ptChannel->GetFd() << endl;
}