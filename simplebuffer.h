#pragma once

#ifndef SIMPLESTRING_H
#define SIMPLESTRING_H

#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <errno.h>

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
	
	char * Buffer();
	// the index of useful data is [m_readOffset, m_writeOffset)
	size_t BufferSize();
	void ReadFromBuffer(size_t nRead);
	void WriteToBuffer(const char * data, size_t writeSize);
	
private:
	static const size_t kReadOrWriteFromFdSize;
	
	char * m_buffer;
	size_t m_allocSize;
	size_t m_readOffset;
	size_t m_writeOffset;
};

#endif