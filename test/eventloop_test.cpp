#include <iostream>
#include <memory>
#include <unistd.h>
#include "../eventloop.h"
#include "../channel.h"

using namespace std;

EventLoop eventLoop;
int nFdpairs = 10;
vector<vector<int>> fdpairs(nFdpairs, vector<int>(2));

void WritePipe();

void ReadCallback(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel)
{
	cout << "read from " << ptChannel->GetFd() << endl;
	sleep(1);
	eventLoop.RemoveChannel(ptChannel, EPOLLIN);
	//eventLoop.RemoveChannel(ptChannel, EPOLLIN | EPOLLOUT);
	cout << "EventSize = " << eventLoop.EventSize() << endl;
}

void WriteCallback(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel)
{
	cout << "write from " << ptChannel->GetFd() << endl;
	sleep(1);
	eventLoop.RemoveChannel(ptChannel, EPOLLOUT);
	cout << "EventSize = " << eventLoop.EventSize() << endl;
}

void ErrorCallback(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel)
{
	cout << "error from " << ptChannel->GetFd() << endl;
	sleep(1);
	eventLoop.RemoveChannel(ptChannel, EPOLLIN | EPOLLOUT);
	cout << "EventSize = " << eventLoop.EventSize() << endl;
}

int main()
{
	for (int i = 0; i < nFdpairs; ++i)
	{
		// fd[0] for read, fd[1] for write
		pipe(static_cast<int *>(&fdpairs[i][0]));
	}
	
	for (int i = 0; i < nFdpairs; ++i)
	{
		shared_ptr<Channel> ptChannel(new Channel(fdpairs[i][0]));
		//shared_ptr<Channel> ptChannel(new Channel(fdpairs[i][1]));
		ptChannel->SetReadCallback(ReadCallback);
		ptChannel->SetWriteCallback(WriteCallback);
		ptChannel->SetErrorCallback(ErrorCallback);
		eventLoop.AddChannel(ptChannel, EPOLLIN);
		cout << "EventSize = " << eventLoop.EventSize() << endl;
	}
	
	for (int i = 0; i < nFdpairs; ++i)
	{
		//shared_ptr<Channel> ptChannel(new Channel(fdpairs[i][0]));
		shared_ptr<Channel> ptChannel(new Channel(fdpairs[i][1]));
		ptChannel->SetReadCallback(ReadCallback);
		ptChannel->SetWriteCallback(WriteCallback);
		ptChannel->SetErrorCallback(ErrorCallback);
		eventLoop.AddChannel(ptChannel, EPOLLOUT);
		cout << "EventSize = " << eventLoop.EventSize() << endl;
	}
	
	Thread thread(WritePipe);
	thread.Start();
	
	eventLoop.Loop();
	thread.Join();
}

void WritePipe()
{
	int deleteNum = 0;
	while(true)
	{
		for (int i = 0; i < nFdpairs; ++i)
		{
			sleep(3);
			int n = write(fdpairs[i][1], "xiao", 4);
			if (0 > n)
			{
				ERRLOG("write error");
			}
		}
		
		if (deleteNum < nFdpairs)
		{

		}
		++deleteNum;
	}
}