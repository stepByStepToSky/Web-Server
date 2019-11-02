#pragma once

#ifndef SIMPLESTRING_H
#define SIMPLESTRING_H

#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
	   
class SimpleBuffer
{
public:
	SimpleBuffer();
	~SimpleBuffer();
	
	SimpleBuffer(const SimpleBuffer &) = delete;
	SimpleBuffer & operator=(const SimpleBuffer &) = delete;
	
	/* return 0, close(peerFd) or shoutdown(peerFd)
	   return -1, error on fd
	   return 1, read sucess */
	int ReadFromFd(int fd);
	
	/* return 0, have writen all data in the buffer to fd, shoule remove EPOLLOUT event
	   return -1, error on fd
	   return 1, shoule continue to write */
	int WriteToFd(int fd);
	
	const char * Buffer();
	// the index of useful data is [m_readOffset, m_writeOffset)
	size_t BufferSize();
	void ReadFromBuffer(size_t nRead);
	void WriteToBuffer(const char * data, size_t writeSize);
	
	// note that the format string will not append into the buffer if the length is larger than 1024
	void Append(const char * format, ...);
	
	// return -1, not found
	// else return the index of the char c
	ssize_t Find(const char c);
	
	// return -1, not found
	// else return the index of the first char of src
	// FIX ME, change to KMP algorithm
	ssize_t Find(const char * src);
	
private:
	static const size_t kReadOrWriteFromFdSize;
	
	char * m_buffer;
	size_t m_allocSize;
	size_t m_readOffset;
	size_t m_writeOffset;
};

#endif