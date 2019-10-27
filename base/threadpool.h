#pragma once

#ifndef THREADPOOL_H
#define THREADPOOL_H


#include <vector>
#include <memory>
#include <queue>
#include <functional>
#include <pthread.h>
#include "mutex.h"
#include "thread.h"
#include "condition.h"

class ThreadPool
{
public:
	typedef std::function<void ()> CallFunc;

	explicit ThreadPool(int taskNum);
	~ThreadPool();
	
	ThreadPool(const ThreadPool &) = delete;
	ThreadPool & operator= (const ThreadPool &) = delete;
	
	void Start(int threadNum);
	void Run(CallFunc func);
	
	// FIX ME: the rest tasks in the m_tasks will not be done, if we stop
	void Stop();
	
private:
	CallFunc Take();
	void RunInThread();

private:
	bool m_bRunning;
	int m_maxTaskNum;
	Mutex m_mutex;
	Condition m_notFull;
	Condition m_notEmpty;
	std::vector<std::unique_ptr<Thread>> m_threads;
	std::deque<CallFunc> m_tasks;
};

#endif