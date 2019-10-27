#pragma once

#ifndef ASYNCLOG_H
#define ASYNCLOG_H

#include <vector>
#include <memory>
#include <functional>
#include <stdlib.h>
#include <unistd.h>
#include "mutex.h"
#include "condition.h"
#include "thread.h"
#include "fixedbuffer.h"
#include "logfile.h"

static const size_t kFixedBufferSize = 4096;

class AsyncLog
{
public:
	typedef FixedBuffer<kFixedBufferSize> Buffer;
	typedef std::unique_ptr<Buffer> BufferPtr;
	
	AsyncLog();
	~AsyncLog();

	AsyncLog(const AsyncLog &) = delete;
	AsyncLog & operator= (const AsyncLog &) = delete;
	
	void Start();
	void Stop();
	void Sync();
	
	void Append(const char * data, size_t writeSize);
	void ThreadFunc();
	void NotifyLog();

private:
	bool m_bRunning;
	std::unique_ptr<Mutex> m_mutex;
	std::unique_ptr<Condition> m_mainThreadRunCond;
	std::unique_ptr<Condition> m_subThreadFlushCond;
	Thread m_thread;
	BufferPtr m_curBuff;
	BufferPtr m_nextBuff;
	std::vector<BufferPtr> m_buffers;
	std::unique_ptr<LogFile> m_logFilePtr;
};

#endif