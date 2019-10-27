#pragma once

#ifndef CHANNEL_H
#define CHANNEL_H


#include <functional>
#include <memory>
#include <sys/epoll.h>

#include "base/log.h"

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
	
	void HandleEvent(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel, int revents);
	
private:
	const int m_fd;
	int m_events;
	CallbackType m_readCallback;
	CallbackType m_writeCallback;
	CallbackType m_errorCallback;
};

#endif