#include <iostream>
#include <vector>
#include <unistd.h>

#include "../epoller.h"
#include "../base/thread.h"

using namespace std;

typedef Epoller::vecActiveEventType vecActiveEventType;

int nFdpairs = 10;
Epoller epoller;
vector<vector<int>> fdpairs(nFdpairs, vector<int>(2));

void WritePipe();

int main()
{
	vecActiveEventType vecActive;
	vecActive.resize(1);
	
	for (int i = 0; i < nFdpairs; ++i)
	{
		pipe(static_cast<int *>(&fdpairs[i][0]));
		// fd[0] for read, fd[1] for write
		// Fix me, set fd to non-blocking, Epoll work in ET model should togther with non-blocking fd, otherwise, read/write fd will be blocked.
		epoller.EpollCtl(fdpairs[i][0], EPOLL_CTL_ADD, EPOLLIN);
		epoller.EpollCtl(fdpairs[i][0], EPOLL_CTL_ADD, EPOLLIN | EPOLLOUT);
		//epoller.EpollCtl(fdpairs[i][0], EPOLL_CTL_DEL, EPOLLOUT);
	}
	
	Thread thread(WritePipe);
	thread.Start();
	
	while(true)
	{
		int nActive = epoller.EpollDispatch(vecActive, 3000);
		cout << nActive << " active events" << endl;
		for (int i = 0; i < nActive; ++i)
		{
			cout << __func__ << " " << __LINE__ << "active fd = " << vecActive[i].data.fd << endl;
			char buff[1024];
			read(vecActive[i].data.fd, buff, 4);
		}
	}
	
	for (int i = 0; i < nFdpairs; ++i)
	{
		close(fdpairs[i][0]);
		close(fdpairs[i][1]);
	}
	thread.Join();
}

void WritePipe()
{
	int deleteNum = 0;
	while(true)
	{
		sleep(5);
		for (int i = 0; i < 10; ++i)
		{
			int n = write(fdpairs[i][1], "xiao", 4);
			if (0 > n)
			{
				ERRLOG("write error");
			}
		}
		
		if (deleteNum < nFdpairs)
		{
			epoller.EpollCtl(fdpairs[deleteNum][0], EPOLL_CTL_DEL, EPOLLIN | EPOLLOUT);
		}
		++deleteNum;
	}
}