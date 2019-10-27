#include "threadpool.h"


ThreadPool::ThreadPool(int taskNum) : m_bRunning(false), m_maxTaskNum(taskNum), m_mutex(), m_notFull(m_mutex), m_notEmpty(m_mutex) 
{
	
}

ThreadPool::~ThreadPool()
{
	if (m_bRunning)
	{
		Stop();
	}
}


void ThreadPool::Start(int threadNum)
{
	if (m_bRunning)
	{
		Stop();
	}
	
	m_threads.reserve(threadNum);
	m_bRunning = true;
	for (int i = 0; i < threadNum; ++i)
	{
		m_threads.emplace_back(new Thread(std::bind(&ThreadPool::RunInThread, this)));
		m_threads[i]->Start();
	}
}


void ThreadPool::Run(CallFunc func)
{
	if (!m_bRunning)
	{
		func();
	}
	else
	{
		LockGuard lock(m_mutex);
		while(m_tasks.size() >= m_maxTaskNum)
		{
			m_notFull.Wait();
		}
		
		m_tasks.push_back(func);
		m_notEmpty.Notify();
	}
}

void ThreadPool::Stop()
{
	m_bRunning = false;
	m_notEmpty.NotifyAll();
	for (std::unique_ptr<Thread> & item : m_threads)
	{
		item->Join();
	}
	
	std::vector<std::unique_ptr<Thread>> temp;
	m_threads.swap(temp);
}

ThreadPool::CallFunc ThreadPool::Take()
{
	LockGuard lock(m_mutex);
	while(m_tasks.empty() && m_bRunning)
	{
		m_notEmpty.Wait();
	}
	
	CallFunc retFunc;
	if (m_bRunning)
	{
		retFunc = m_tasks.front();
		m_tasks.pop_front();
		m_notFull.Notify();
	}
	
	return retFunc;
}

void ThreadPool::RunInThread()
{
	while(m_bRunning)
	{
		CallFunc task = Take();
		if (task)
		{
			task();
		}
	}
}