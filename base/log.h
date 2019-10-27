#pragma once

#ifndef LOG_H
#define LOG_H

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "asynclog.h"
#include "singleton.h"


class LogManager
{
public:
	LogManager();
	~LogManager();
	
	void Append(const char * data, size_t writeSize);
	
private:
	AsyncLog m_log;
};

static inline void WriteLogImpl(const char * data, size_t writeSize);

void ERRLOGImpl(const char * errMsg);

void DEBUGLOGImpl(const char * debugMsg);

void ERRLOG(const char * format, ...);

void DEBUGLOG(const char * format, ...);

#endif