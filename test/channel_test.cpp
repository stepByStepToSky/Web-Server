#include <iostream>
#include <vector>
#include <memory>
#include <unistd.h>

#include "../channel.h"

using namespace std;

class EventLoop
{
	
};

void ReadCallback(EventLoop &, std::shared_ptr<Channel>)
{
	cout << "read" << endl;
}

void WriteCallback(EventLoop &, std::shared_ptr<Channel>)
{
	cout << "write" << endl;
}

void ErrorCallback(EventLoop &, std::shared_ptr<Channel>)
{
	cout << "error" << endl;
}

int main()
{
	shared_ptr<Channel> ptChannel(new Channel(1));
	ptChannel->SetReadCallback(ReadCallback);
	ptChannel->SetWriteCallback(WriteCallback);
	ptChannel->SetErrorCallback(ErrorCallback);
	
	EventLoop eventLoop;
	ptChannel->HandleEvent(eventLoop, ptChannel, EPOLLIN);
	ptChannel->HandleEvent(eventLoop, ptChannel, EPOLLOUT);
	ptChannel->HandleEvent(eventLoop, ptChannel, EPOLLERR);
}