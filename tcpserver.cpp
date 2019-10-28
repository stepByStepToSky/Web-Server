#include <unistd.h>
#include <stdint.h>

#include "tcpserver.h"
#include "netutil.h"

void TcpServer::NewConnectReadHandler(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel)
{
	int fd;
	struct sockaddr_in peerAddr;
	char ipStr[64];
	socklen_t peerAddrLen;
	
	while(-1 != (fd = accept(m_listenFd, reinterpret_cast<struct sockaddr*>(&peerAddr), &peerAddrLen)))
	{
		NetUtil::SetNonblock(fd);
		NetUtil::SetNoDelay(fd);
		ChannelPtr ptConnChannel(new Channel(fd));
		uint32_t ip = ntohl(peerAddr.sin_addr.s_addr);
		uint16_t port = ntohs(peerAddr.sin_port);
		snprintf(ipStr, sizeof(ipStr), "%d.%d.%d.%d", ip >> 24, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);
		DEBUGLOG("accept new socket, socket=%d from %s:%d", fd, ipStr, port);
		ptConnChannel->SetReadCallback(m_readCallback);
		ptConnChannel->SetWriteCallback(m_writeCallback);
		ptConnChannel->SetErrorCallback(m_errorCallback);
		m_eventLoop.AddChannel(ptConnChannel, EPOLLIN);
	}
}

TcpServer::TcpServer(const char * serverIp, uint16_t port) : m_listenFd(NetUtil::Listen(serverIp, port)), m_ptListenChannel(new Channel(m_listenFd))
{
	using namespace std::placeholders;
	m_ptListenChannel->SetReadCallback(std::bind(&TcpServer::NewConnectReadHandler, this, _1, _2));
	m_eventLoop.AddChannel(m_ptListenChannel, EPOLLIN);
}

TcpServer::~TcpServer()
{
	close(m_ptListenChannel->GetFd());
}

void TcpServer::Loop()
{
	m_eventLoop.Loop();
}

void TcpServer::SetReadCallback(CallbackType readCallback)
{
	m_readCallback = readCallback;
}

void TcpServer::SetWriteCallback(CallbackType writeCallback)

{
	m_writeCallback = writeCallback;
}

void TcpServer::SetErrorCallback(CallbackType errorCallback)
{
	m_errorCallback = errorCallback;
}