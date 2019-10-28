#include <iostream>
#include <string.h>

#include "../tcpserver.h"

using namespace std;

void ReadCallback(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel);
void WriteCallback(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel);
void ErrorCallback(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel);

const char * servIp = "127.0.0.1";
const int port = 9508;

int main()
{
	TcpServer server(servIp, port);
	server.SetReadCallback(ReadCallback);
	server.SetWriteCallback(WriteCallback);
	server.SetErrorCallback(ErrorCallback);
	if (0 == fork())
	{
		sleep(3);
		while(true)
		{
			
		}
	}
	server.Loop();
}

void ReadCallback(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel)
{
	cout << "read from " << ptChannel->GetFd() << endl;
	char recvBuff[4096];
	memset(recvBuff, 0, sizeof(recvBuff));
	int nRead = read(ptChannel->GetFd(), recvBuff, sizeof(recvBuff));
	if (0 < nRead)
	{
		cout << recvBuff << endl;
	}
	else if (0 == nRead)
	{
		eventLoop.RemoveChannel(ptChannel, EPOLLIN | EPOLLOUT);
		close(ptChannel->GetFd());
	}
}

void WriteCallback(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel)
{
	cout << "write from " << ptChannel->GetFd() << endl;
}

void ErrorCallback(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel)
{
	cout << "error from " << ptChannel->GetFd() << endl;
}