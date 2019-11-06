#pragma once

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "eventloop.h"
#include "channel.h"
#include "base/log.h"
#include "eventloopthreadpool.h"

class TcpServer
{
public:
	typedef EventLoop::ChannelPtr ChannelPtr;
	typedef std::function<void (std::shared_ptr<Channel>)> CallbackType;
	
	// return -1 means request gets error or return 0 we should close the socket, we should close the channel
	// else return 1
	typedef std::function<int (std::shared_ptr<Channel>)> ReadCallbackType;
	
	TcpServer(const char * serverIp, const uint16_t port, int threadCnt = 0);
	~TcpServer();
	
	void SetReadCallback(ReadCallbackType readCallback);
	void SetWriteCallback(CallbackType writeCallback);
	void SetErrorCallback(CallbackType errorCallback);
	void Loop();
	
private:
	void NewConnectReadHandler(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel);
	void ReadFromFdToBuffer(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel);
	void WriteFromBufferToFd(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel);
	void ErrorCallBack(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel);
	
private:
	int m_listenFd;
	ChannelPtr m_ptListenChannel;
	EventLoop m_eventLoop;
	EventLoopThreadPool m_eventThreadPool;
	ReadCallbackType m_readCallback;
	CallbackType m_writeCallback;
	CallbackType m_errorCallback;
};

#endif