#include "log.h"
#include <ctime>

LogManager::LogManager()
{
	m_log.Start();
}

LogManager::~LogManager()
{
	m_log.Stop();
}

void LogManager::Append(const char * data, size_t writeSize)
{
	m_log.Append(data, writeSize);
}

static inline void WriteLogImpl(const char * data, size_t writeSize)
{
	Singleton<LogManager>::GetInstance().Append(data, writeSize);
}

void ERRLOGImpl(const char * errMsg)
{
	char buff[1024];
	memcpy(buff, "ERROR: ", 7);
	size_t writeSize = 7;
	
	time_t now = time(0);
	char* dt = ctime(&now);
	size_t cTimeLen = strlen(dt);
	dt[cTimeLen - 1] = ' ';
	memcpy(buff + writeSize, dt, cTimeLen);
	writeSize += cTimeLen;
	
	size_t nErrmsg = strlen(errMsg);
	memcpy(buff + writeSize, errMsg, nErrmsg);
	writeSize += nErrmsg;
	// FIX ME may overflow
	buff[writeSize] = '\n';
	writeSize += 1;
	WriteLogImpl(buff, writeSize);
}


void DEBUGLOGImpl(const char * debugMsg)
{
	char buff[1024];
	memcpy(buff, "DEBUG: ", 7);
	size_t writeSize = 7;
	
	time_t now = time(0);
	char* dt = ctime(&now);
	size_t cTimeLen = strlen(dt);
	dt[cTimeLen - 1] = ' ';
	memcpy(buff + writeSize, dt, cTimeLen);
	writeSize += cTimeLen;
	
	size_t nErrmsg = strlen(debugMsg);
	memcpy(buff + writeSize, debugMsg, nErrmsg);
	writeSize += nErrmsg;
	// FIX ME may overflow
	buff[writeSize] = '\n';
	writeSize += 1;
	WriteLogImpl(buff, writeSize);
}


void ERRLOG(const char * format, ...)
{
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	int nWrite = 0;
	va_list ap;
	va_start(ap, format);
	if (0 < (nWrite = vsnprintf(buff, sizeof(buff), format, ap)))
	{
		ERRLOGImpl(static_cast<const char *> (buff));
	}
	va_end(ap);
}

void DEBUGLOG(const char * format, ...)
{
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	int nWrite = 0;
	va_list ap;
	va_start(ap, format);
	if (0 < (nWrite = vsnprintf(buff, sizeof(buff), format, ap)))
	{
		DEBUGLOGImpl(static_cast<const char *> (buff));
	}
	va_end(ap);
}