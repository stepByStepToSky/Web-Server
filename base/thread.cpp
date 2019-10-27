#include "thread.h"

Thread::Thread(CallFunc func) : m_bStarted(false), m_bJointed(false), m_pthreadId(0), m_func(func)
{
	
}

Thread::~Thread()
{
	if (m_bStarted && !m_bJointed)
	{
		pthread_detach(m_pthreadId);
	}
}

struct ThreadData
{
	typedef typename Thread::CallFunc CallFunc;
	
	ThreadData(CallFunc func) : m_func(func) {}
	
	CallFunc m_func;
};

void * ThreadRun(void * arg)
{
	ThreadData * ptData = static_cast<ThreadData *> (arg);
	ptData->m_func();
	delete ptData;
}

void Thread::Start()
{
	if (!m_bStarted)
	{
		ThreadData * ptData = new ThreadData(m_func);
		pthread_create(&m_pthreadId, NULL, ThreadRun, static_cast<void *> (ptData));
		m_bStarted = true;
	}
}

void Thread::Join()
{
	if (!m_bJointed)
	{
		pthread_join(m_pthreadId, NULL);
		m_bJointed = true;
	}
}
