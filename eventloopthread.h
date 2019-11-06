#pragma once

#ifndef EVENTLOOPTHREAD_H
#define EVENTLOOPTHREAD_H

#include <vector>
#include <functional>
#include <unistd.h>
#include <arpa/inet.h>
#include "base/mutex.h"
#include "channel.h"
#include "eventloop.h"

class EventLoopThread
{
public:
	typedef std::shared_ptr<Channel> ChannelPtr;
	typedef Channel::CallbackType CallbackType;
	
	EventLoopThread();
	~EventLoopThread();
	
	EventLoopThread(const EventLoopThread &) = delete;
	EventLoopThread & operator=(const EventLoopThread &) = delete;
	
	void SetReadCallback(CallbackType readCallback);
	void SetWriteCallback(CallbackType writeCallback);
	void SetErrorCallback(CallbackType errorCallback);
	
	void PushFd(int fd);
	void Loop();
	void Quit();
	
private:
	void OnReadHandler(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel);
	
private:
	int m_pipeFd[2];	// to arouse the sub-thread to add socket fd in m_vecQue, EPOLLIN event in m_eventloop
	ChannelPtr m_listenChannel;
	Mutex m_mutex;		// protected the m_vecQue
	std::vector<int> m_vecQue;
	EventLoop m_eventLoop;
	CallbackType m_readCallback;
	CallbackType m_writeCallback;
	CallbackType m_errorCallback;
};

#endif