
#include "asynclog.h"

AsyncLog::AsyncLog()
	: m_bRunning(false),
	m_mutex (new Mutex), 
	m_mainThreadRunCond(new Condition(*m_mutex)),
	m_subThreadFlushCond(new Condition(*m_mutex)), 
	m_thread(std::bind(&AsyncLog::ThreadFunc, this)), 
	m_curBuff(new Buffer), 
	m_nextBuff(new Buffer),
	m_logFilePtr(new LogFile)
{
	m_buffers.reserve(8);
}

AsyncLog::~AsyncLog()
{
	if (m_bRunning)
	{
		Stop();
	}
	Sync();
}

void AsyncLog::Start()
{
	m_bRunning = true;
	m_thread.Start();
	LockGuard lock(*m_mutex);
	m_mainThreadRunCond->Wait();
	//std::cout << "Main thread Running " << __LINE__ << std::endl;
}

void AsyncLog::Stop()
{
	m_bRunning = false;
	m_subThreadFlushCond->Notify();
	m_thread.Join();
}

void AsyncLog::Sync()
{
	m_logFilePtr->Sync();
}

void AsyncLog::NotifyLog()
{
	m_subThreadFlushCond->Notify();
}

void AsyncLog::Append(const char * data, size_t writeSize)
{
	//std::cout << "Main thread, " << __LINE__ << std::endl;
	LockGuard lock(*m_mutex);
	if (m_curBuff->Avail() > writeSize)
	{
		//std::cout << __FILE__ << " " << __LINE__ << " " << __func__ << " m_curBuff->Avail()="  << m_curBuff->Avail() << " writeSize=" << writeSize << std::endl;
		m_curBuff->Append(data, writeSize);
	}
	else
	{
		m_buffers.push_back(std::move(m_curBuff));
		if (m_nextBuff)
		{
			m_curBuff = std::move(m_nextBuff);
		}
		else
		{
			m_curBuff.reset(new Buffer);
		}
		m_curBuff->Append(data, writeSize);
		m_subThreadFlushCond->Notify();
	}
	//std::cout << "Main thread, " << __LINE__ << std::endl;
}

void AsyncLog::ThreadFunc()
{
	BufferPtr firstBuffer(new Buffer);
	BufferPtr secondBuffer(new Buffer);
	std::vector<BufferPtr> vecWriteBuffers;
	vecWriteBuffers.reserve(16);
	{
		LockGuard lock(*m_mutex);
		m_mainThreadRunCond->Notify();
	}
	//std::cout << "Sub thread, " << __LINE__ << " Running " << std::endl;
	while(m_bRunning)
	{
		{
			LockGuard lock(*m_mutex);
			if (m_buffers.empty())
			{
				//std::cout << "Sub thread, wait for Condition, " << __LINE__ << std::endl;
				m_subThreadFlushCond->WaitForSeconds(3);
				//m_subThreadFlushCond.Wait();
			}
			//std::cout << "Sub thread, " << __LINE__ << " Buffer size, " <<m_buffers.size() << std::endl;
			
			m_buffers.push_back(std::move(m_curBuff));
			m_curBuff = std::move(firstBuffer);
			m_curBuff->Reset();
			if (!m_nextBuff)
			{
				m_nextBuff = std::move(secondBuffer);
				m_nextBuff->Reset();
			}
			
			m_buffers.swap(vecWriteBuffers);
			m_buffers.clear();
		}
		
		for (size_t i = 0; i < vecWriteBuffers.size(); ++i)
		{
			m_logFilePtr->Append(vecWriteBuffers[i]->Data(), vecWriteBuffers[i]->Size());
		}
		
		firstBuffer = std::move(vecWriteBuffers[0]);
		if (2 <= vecWriteBuffers.size())
		{
			secondBuffer = std::move(vecWriteBuffers[1]);
		}
		vecWriteBuffers.clear();
		m_logFilePtr->Flush();
	}

	// push data in buffer to the log file
	LockGuard lock(*m_mutex);
	m_buffers.push_back(std::move(m_curBuff));
	for (size_t i = 0; i < m_buffers.size(); ++i)
	{
		m_logFilePtr->Append(m_buffers[i]->Data(), m_buffers[i]->Size());
	}
	m_logFilePtr->Flush();
}