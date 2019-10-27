#include "logfile.h"

LogFile::LogFile()
{
	RollFile();
}
LogFile::~LogFile()
{
	Close();
}

void LogFile::GetFileName(time_t nowTime, std::string & sLogName)
{
	struct tm * stTime = gmtime(&nowTime);
	sLogName.resize(19);
	strftime(const_cast<char *> (sLogName.data()), sLogName.capacity(), "%Y%m%d-%H%M%S.log", stTime);
}

void LogFile::RollFile()
{
	time_t nowTime = time(NULL);
	std::string sLogName;
	GetFileName(nowTime, sLogName);
	m_ptWriteFile.reset(new WriteFile(sLogName));
	m_cnt = 0;
	m_writedSize = 0;
	m_lastFlushTime = nowTime;
	m_lastRollTime = nowTime;
}

void LogFile::Append(const char * data, size_t writeSize)
{
	// FIX ME if writeSize is larger than kRollFileSize, will cause wrong
	if (m_writedSize > kRollFileSize - writeSize)
	{
		//std::cout << "RollFile " << __FILE__ << " " << __LINE__ << " " << __func__ << " " << m_writedSize << " " << std::endl; 
		RollFile();
	}
	
	m_ptWriteFile->Append(data, writeSize);
	m_writedSize += writeSize;
	m_cnt += 1;
	
	//std::cout << __FILE__ << " " << __LINE__ << " " << __func__ << " " << m_cnt << " " << m_writedSize << std::endl; 
	
	if (kCheckNSteps < m_cnt)
	{
		time_t nowTime = time(NULL);
		time_t nextStartTime = nowTime / kRollPerSeconds * kRollPerSeconds;
		// next day shoule roll log file
		if (m_lastRollTime < nextStartTime)
		{
			//std::cout << "RollFile " << __FILE__ << " " << __LINE__ << " " << __func__ << " " << nowTime << " " << m_lastFlushTime << std::endl; 
			RollFile();
		}
		else if (kFlushInteralSeconds < nowTime - m_lastFlushTime)
		{
			//std::cout << "Flush " << __FILE__ << " " << __LINE__ << " " << __func__ << " " << nowTime << " " << m_lastFlushTime << std::endl; 
			Flush();
			m_lastFlushTime = nowTime;
		}
		
		m_cnt = 0;
	}
}

void LogFile::Flush()
{
	m_ptWriteFile->Flush();
}

void LogFile::Sync()
{
	m_ptWriteFile->Sync();
}

void LogFile::Close()
{
	m_ptWriteFile->Close();
}
