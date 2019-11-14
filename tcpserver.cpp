#include <unistd.h>
#include <stdint.h>

#include "tcpserver.h"
#include "netutil.h"


const int TcpServer::kMaxConnectionCnt = 10000;

void TcpServer::ReadFromFdToBuffer(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel)
{
	SimpleBuffer & inBuffer = ptChannel->GetInBuffer();
	SimpleBuffer & outBuffer = ptChannel->GetOutBuffer();
	int nRead = inBuffer.ReadFromFd(ptChannel->GetFd());
	int readCallbackResult = 0;
	if (0 < nRead)
	{
		DEBUGLOG("%s %s %d, read from fd=%d", __FILE__, __func__, __LINE__, ptChannel->GetFd());
		if (m_readCallback)
		{
			readCallbackResult = m_readCallback(ptChannel);
		}
		
		if (0 < outBuffer.BufferSize())
		{
			DEBUGLOG("%s %s %d, %d bytes in the buffer of fd=%d need to be writen", __FILE__, __func__, __LINE__, outBuffer.BufferSize(), ptChannel->GetFd());
			eventLoop.AddChannel(ptChannel, EPOLLOUT);
		}
		eventLoop.ChangeLastActivedTime(ptChannel, eventLoop.GetLastActiveTime());
	}
	
	if (1 != nRead || 1 != readCallbackResult)
	{
		if (0 == readCallbackResult && 0 < outBuffer.BufferSize())	// should close, but maybe there is some data in outBuffer, we should write them to fd, then close
		{
			for (; 1 == outBuffer.WriteToFd(ptChannel->GetFd()); )
			{
			}
			DEBUGLOG("%s %s %d, fd=%d read callback function return close", __FILE__, __func__, __LINE__, ptChannel->GetFd());
		}
		else
		{
			DEBUGLOG("%s %s %d, fd=%d %s", __FILE__, __func__, __LINE__, ptChannel->GetFd(), (0 == nRead ? "socket close/shoutdown" : (-1 == nRead ? "socket gets error" : "read callback function return error")));
		}
		
		// peer socket close/shoutdown or get error
		eventLoop.RemoveChannel(ptChannel, EPOLLIN | EPOLLOUT);
		eventLoop.DeleteLastActivedTime(ptChannel);
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
		eventLoop.DeleteLastActivedTime(ptChannel);
		close(ptChannel->GetFd());
		return;
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
	eventLoop.ChangeLastActivedTime(ptChannel, eventLoop.GetLastActiveTime());
}

void TcpServer::ErrorCallBack(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel)
{
	m_errorCallback(ptChannel);
	eventLoop.RemoveChannel(ptChannel, EPOLLIN | EPOLLOUT);
	eventLoop.DeleteLastActivedTime(ptChannel);
	close(ptChannel->GetFd());
}

void TcpServer::NewConnectReadHandler(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel)
{
	int fd;
	struct sockaddr_in peerAddr;
	char ipStr[INET_ADDRSTRLEN];
	socklen_t peerAddrLen = 1;
	
	while(-1 != (fd = accept(m_listenFd, reinterpret_cast<struct sockaddr*>(&peerAddr), &peerAddrLen)))
	{
		// limit connection nums
		if (kMaxConnectionCnt < fd)
		{
			close(fd);
			continue;
		}
		
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
		
		if (m_eventThreadPool.Empty())
		{
			ERRLOG("%s %s %d, m_eventThreadPool.GetThreadCnt()=%d", __FILE__, __func__, __LINE__, (int)(m_eventThreadPool.GetThreadCnt()));
			using namespace std::placeholders;
			ptConnChannel->SetReadCallback(std::bind(&TcpServer::ReadFromFdToBuffer, this, _1, _2));
			ptConnChannel->SetWriteCallback(std::bind(&TcpServer::WriteFromBufferToFd, this, _1, _2));
			ptConnChannel->SetErrorCallback(std::bind(&TcpServer::ErrorCallBack, this, _1, _2));
			m_eventLoop.AddChannel(ptConnChannel, EPOLLIN);
			m_eventLoop.AddLastActivedTime(ptChannel, m_eventLoop.GetLastActiveTime());
		}
		else
		{
			ERRLOG("%s %s %d, m_eventThreadPool.GetThreadCnt()=%d", __FILE__, __func__, __LINE__, (int)(m_eventThreadPool.GetThreadCnt()));
			m_eventThreadPool.PushFd(fd);
		}
		
		peerAddrLen = 1; // to avoid accept(): Invalid Argument
	}
	
	//ERRLOG("%s %s %d, m_listenFd=%d, listen error. %s", __FILE__, __func__, __LINE__, m_listenFd, strerror(errno));
}


TcpServer::TcpServer(const char * serverIp, const uint16_t port, int threadCnt /* = 0 */) : m_listenFd(NetUtil::Listen(serverIp, port)),
																			m_eventThreadPool(threadCnt),
																			m_ptListenChannel(new Channel(m_listenFd))
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
	using namespace std::placeholders;
	m_eventThreadPool.SetReadCallback(std::bind(&TcpServer::ReadFromFdToBuffer, this, _1, _2));
	m_eventThreadPool.SetWriteCallback(std::bind(&TcpServer::WriteFromBufferToFd, this, _1, _2));
	m_eventThreadPool.SetErrorCallback(std::bind(&TcpServer::ErrorCallBack, this, _1, _2));
	m_eventThreadPool.Loop();
	m_eventLoop.Loop();
}

void TcpServer::SetReadCallback(ReadCallbackType readCallback)
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