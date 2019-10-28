#include "netutil.h"
#include "base/log.h"


int NetUtil::Listen(const char * serverIp, uint16_t port)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == fd)
	{
		ERRLOG("%s %s %d, socket error. %s", __FILE__, __func__, __LINE__, strerror(errno));
		return -1;
	}
	
	SetReuseAddr(fd);
	SetNonblock(fd);
	
	struct sockaddr_in servAddr;
	SetAddr(serverIp, port, servAddr);
	if (-1 == bind(fd, reinterpret_cast<struct sockaddr *>(&servAddr), static_cast<socklen_t>(sizeof(servAddr))))
	{
		ERRLOG("%s %s %d, bind error. %s", __FILE__, __func__, __LINE__, strerror(errno));
		close(fd);
		return -1;
	}
	if (-1 == listen(fd, 128))
	{
		ERRLOG("%s %s %d, listen error. %s", __FILE__, __func__, __LINE__, strerror(errno));
		close(fd);
		return -1;
	}
	return fd;
}

int NetUtil::Connect(const char * serverIp, uint16_t port)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == fd)
	{
		ERRLOG("%s %s %d, socket error. %s", __FILE__, __func__, __LINE__, strerror(errno));
		return -1;
	}
	
	struct sockaddr_in servAddr;
	SetAddr(serverIp, port, servAddr);
	if (-1 == connect(fd, reinterpret_cast<struct sockaddr *>(&servAddr), static_cast<socklen_t>(sizeof(servAddr))))
	{
		close(fd);
		ERRLOG("%s %s %d, connect error. %s", __FILE__, __func__, __LINE__, strerror(errno));
		return -1;
	}
	
	return fd;
}

void NetUtil::SetReuseAddr(int fd, int optval /* = 1 */)
{
	if (0 != setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof(optval))))
	{
		ERRLOG("%s %s %d, set reuse address error. %s", __FILE__, __func__, __LINE__, strerror(errno));
	}
}

void NetUtil::SetNonblock(int fd)
{
	int ret = fcntl(fd, F_GETFL);
	ret = ret | O_NONBLOCK;
	if (-1 == fcntl(fd, F_SETFL, ret))
	{
		ERRLOG("%s %s %d, set fd = %d nonblock error. %s", __FILE__, __func__, __LINE__, fd, strerror(errno));
	}
}

void NetUtil::SetNoDelay(int fd, int optval /* = 1 */)
{
	if (0 != setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof(optval))))
	{
		ERRLOG("%s %s %d, set fd = %d nodelay error. %s", __FILE__, __func__, __LINE__, fd, strerror(errno));
	}
}

void NetUtil::SetAddr(const char * serverIp, uint16_t port, struct sockaddr_in & sockAddr)
{
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	if (1 != inet_pton(AF_INET, serverIp, &sockAddr.sin_addr))
	{
		ERRLOG("%s %s %d, set server address error. %s", __FILE__, __func__, __LINE__, strerror(errno));
	}
}