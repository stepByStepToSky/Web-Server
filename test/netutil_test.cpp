#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "../netutil.h"
#include "../base/thread.h"

using namespace std;

const char * servIp = "127.0.0.1";
const int port = 9508;

int main()
{	
	int pipefd[2];
	pipe(pipefd);
	char buf[1024];
	
	int fd = NetUtil::Listen(servIp, port);
	
	vector<string> vecIp;
	NetUtil::GetIpByDomain("smtp.gmail.com", vecIp);
	for (int i = 0; i < vecIp.size(); ++i)
	{
		cout << vecIp[i] << endl;
	}
	
	// Note that the async log can not be used in multi process environment, some bugs here.
	if (0 == fork())
	{
		close(fd);
		int i = 0, connfd;
		while(i < 10)
		{
			connfd = NetUtil::Connect(servIp, port);
			if (0 < connfd)
			{
				cout << "i = " << i << " connect sucess, fd = " << connfd << endl;
				++i;
				//close(connfd);
			}
			usleep(100);
		}
		_exit(0);
	}
	
	while (0 > accept(fd, NULL, NULL))
	{
		if (EAGAIN == errno || EWOULDBLOCK == errno)
		{
			cout << "accept nonblock fd, no date" << endl;
		}
		else
		{
			cout << "accept error" << endl;
			break;
		}
	}
	close(fd);
	
	NetUtil::SetNonblock(pipefd[0]);
	if (read(pipefd[0], buf, sizeof(buf)))
	{
		if (EAGAIN == errno || EWOULDBLOCK == errno)
		{
			cout << "read nonblock fd, no date" << endl;
		}
		else
		{
			cout << "read error" << endl;
		}
	}
}