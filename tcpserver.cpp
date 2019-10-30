#include <unistd.h>
#include <stdint.h>

#include "tcpserver.h"
#include "netutil.h"

void TcpServer::ReadFromFdToBuffer(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel)
{
	SimpleBuffer & inBuffer = ptChannel->GetInBuffer();
	int nRead = inBuffer.ReadFromFd(ptChannel->GetFd());
	if (0 < nRead)
	{
		DEBUGLOG("%s %s %d, read from fd=%d", __FILE__, __func__, __LINE__, ptChannel->GetFd());
		if (m_readCallback)
		{
			m_readCallback(ptChannel);
		}
		SimpleBuffer & outBuffer = ptChannel->GetOutBuffer();
		if (0 < outBuffer.BufferSize())
		{
			DEBUGLOG("%s %s %d, %d bytes in the buffer of fd=%d need to be writen", __FILE__, __func__, __LINE__, outBuffer.BufferSize(), ptChannel->GetFd());
			eventLoop.AddChannel(ptChannel, EPOLLOUT);
		}
	}
	else if (0 >= nRead)
	{
		DEBUGLOG("%s %s %d, fd=%d %s", __FILE__, __func__, __LINE__, ptChannel->GetFd(), (0 == nRead ? "close/shoutdown" : "gets error"));
		// peer socket close/shoutdown or get error
		eventLoop.RemoveChannel(ptChannel, EPOLLIN | EPOLLOUT);
		close(ptChannel->GetFd());
	}
}

void TcpServer::WriteFromBufferToFd(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel)
{
	SimpleBuffer & outBuffer = ptChannel->GetOutBuffer();
	int nWrite = outBuffer.WriteToFd(ptChannel->GetFd());
	if (0 > nWrite)
	{
		DEBUGLOG("%s %s %d, fd=%d gets error", __FILE__, __func__, __LINE__, ptChannel->GetFd());
		// peer socket get error
		eventLoop.RemoveChannel(ptChannel, EPOLLIN | EPOLLOUT);
		close(ptChannel->GetFd());
	}
	else if (0 == nWrite)
	{
		DEBUGLOG("%s %s %d, fd=%d write done", __FILE__, __func__, __LINE__, ptChannel->GetFd());
		// write all data in the outbuffer done
		if (m_writeCallback)
		{
			m_writeCallback(ptChannel);
		}
		eventLoop.RemoveChannel(ptChannel, EPOLLOUT);
	}
}

void TcpServer::ErrorCallBack(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel)
{
	m_errorCallback(ptChannel);
	eventLoop.RemoveChannel(ptChannel, EPOLLIN | EPOLLOUT);
	close(ptChannel->GetFd());
}

void TcpServer::NewConnectReadHandler(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel)
{
	int fd;
	struct sockaddr_in peerAddr;
	char ipStr[INET_ADDRSTRLEN];
	socklen_t peerAddrLen;
	
	while(-1 != (fd = accept(m_listenFd, reinterpret_cast<struct sockaddr*>(&peerAddr), &peerAddrLen)))
	{
		NetUtil::SetNonblock(fd);
		NetUtil::SetNoDelay(fd);
		ChannelPtr ptConnChannel(new Channel(fd));
		/*
		uint32_t ip = ntohl(peerAddr.sin_addr.s_addr);
		uint16_t port = ntohs(peerAddr.sin_port);
		snprintf(ipStr, sizeof(ipStr), "%d.%d.%d.%d", ip >> 24, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);
		*/
		memset(ipStr, 0, sizeof(ipStr));
		if (NULL == inet_ntop(AF_INET, &peerAddr.sin_addr.s_addr, ipStr, sizeof(ipStr)))
		{
			ERRLOG("inet_ntop error, %s", strerror(errno));
			close(fd);
			return;
		}
		uint16_t port = ntohs(peerAddr.sin_port);
		DEBUGLOG("%s %s %d, accept new socket, socket=%d from %s:%d", __FILE__, __func__, __LINE__, fd, ipStr, port);
		
		using namespace std::placeholders;
		ptConnChannel->SetReadCallback(std::bind(&TcpServer::ReadFromFdToBuffer, this, _1, _2));
		ptConnChannel->SetWriteCallback(std::bind(&TcpServer::WriteFromBufferToFd, this, _1, _2));
		ptConnChannel->SetErrorCallback(std::bind(&TcpServer::ErrorCallBack, this, _1, _2));
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
	close(m_listenFd);
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