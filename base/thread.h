#pragma once

#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <functional>
#include <memory>

class Thread
{
public:
	typedef std::function<void ()> CallFunc;
	
	explicit Thread(CallFunc func);
	~Thread();
	
	Thread(const Thread &) = delete;
	Thread & operator= (const Thread &) = delete;
	
	void Start();
	void Join();
	
private:
	bool m_bStarted;
	bool m_bJointed;
	pthread_t m_pthreadId;
	CallFunc m_func;
};

#endif