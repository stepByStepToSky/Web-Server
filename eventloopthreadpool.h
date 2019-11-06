#pragma once

#ifndef EVENTLOOPTHREADPOOL_H
#define EVENTLOOPTHREADPOOL_H

#include <vector>
#include <memory>
#include "base/thread.h"
#include "eventloopthread.h"


class EventLoopThreadPool
{
public:
	typedef EventLoopThread::CallbackType CallbackType;
	
	explicit EventLoopThreadPool(int threadCnt);
	~EventLoopThreadPool();
	
	void SetReadCallback(CallbackType readCallback);
	void SetWriteCallback(CallbackType writeCallback);
	void SetErrorCallback(CallbackType errorCallback);
	
	void PushFd(int fd);
	void Loop();
	void Quit();
	int GetThreadCnt();
	bool Empty();
	
private:
	void RunInThread(int threadId);

private:
	int m_threadCnt;
	int m_robinIndex;
	std::vector<std::unique_ptr<Thread>> m_threads;
	std::vector<EventLoopThread *> m_eventLoopThreads;
	CallbackType m_readCallback;
	CallbackType m_writeCallback;
	CallbackType m_errorCallback;
};

#endif