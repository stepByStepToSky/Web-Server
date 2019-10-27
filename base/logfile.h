#pragma once

#ifndef LOGFILE
#define LOGFILE


#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <memory>
#include <string>
#include <iostream>
#include "writefile.h"

class LogFile
{
public:
	LogFile();
	~LogFile();
	
	LogFile(const LogFile &) = delete;
	LogFile & operator= (const LogFile &) = delete;
	
	void Append(const char * data, size_t writeSize);
	void Flush();
	void Sync();
	void Close();
	
private:
	void GetFileName(time_t nowTime, std::string & sLogName);
	void RollFile();
	
private:
	std::unique_ptr<WriteFile> m_ptWriteFile;
	// do m_cnt append operators, check if we need a flush to garantee the data safely, and check if we need roll the log file
	size_t m_cnt;
	size_t m_writedSize;
	time_t m_lastFlushTime;
	time_t m_lastRollTime;
	static const time_t kRollPerSeconds = 24 * 60 * 60;
	static const size_t kRollFileSize = 64 * 1024 * 1024;
	static const size_t kCheckNSteps = 1000;
	static const size_t kFlushInteralSeconds = 3;
};


#endif