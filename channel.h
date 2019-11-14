#pragma once

#ifndef CHANNEL_H
#define CHANNEL_H


#include <functional>
#include <memory>
#include <sys/epoll.h>
#include <sys/time.h>

#include "base/log.h"
#include "simplebuffer.h"
#include "httpmessage.h"

class EventLoop;

class Channel
{
public:
	typedef std::function<void (EventLoop &, std::shared_ptr<Channel>)> CallbackType;
	
	explicit Channel(int fd);
	
	void SetReadCallback(CallbackType readCallback);
	void SetWriteCallback(CallbackType writeCallback);
	void SetErrorCallback(CallbackType errorCallback);
	void SetEvents(int events);
	int GetFd();
	int GetEvents();
	SimpleBuffer & GetInBuffer();
	SimpleBuffer & GetOutBuffer();
	HttpMessage & GetHttpMessage();
	
	const struct timeval & GetLastActiveTime();
	void SetLastActiveTime(const struct timeval & stTimeval);
	int GetMinHeapIndex();
	void SetMinHeapIndex(int minHeapIndex);

	void HandleEvent(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel, int revents);
	
private:
	const int m_fd;
	int m_events;
	CallbackType m_readCallback;
	CallbackType m_writeCallback;
	CallbackType m_errorCallback;
	
	SimpleBuffer m_inBuffer;
	SimpleBuffer m_outBuffer;
	HttpMessage m_httpmessage;

	struct timeval m_stLastActiveTime;
	int m_minHeapIndex;
};

#endif