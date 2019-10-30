#include "simplebuffer.h"

const size_t SimpleBuffer::kReadOrWriteFromFdSize = 1024;

SimpleBuffer::SimpleBuffer() : m_buffer(NULL), m_allocSize(0), m_readOffset(0), m_writeOffset(0)
{
	
}

SimpleBuffer::~SimpleBuffer()
{
	if (NULL != m_buffer)
	{
		free(m_buffer);
	}
}

int SimpleBuffer::ReadFromFd(int fd)
{
	// can not directly write kReadOrWriteFromFdSize bytes in the buffer
	if (kReadOrWriteFromFdSize > m_allocSize - m_writeOffset)
	{
		// if move data to head can satisfy to write kReadOrWriteFromFdSize bytes data to buffer, 
		// we do not malloc memory, just by moving data to the head
		if (NULL != m_buffer)
		{
			memmove(m_buffer, m_buffer + m_readOffset, m_writeOffset - m_readOffset);
			m_writeOffset -= m_readOffset;
			m_readOffset = 0;
		}
		
		// else need to realloc memory
		if (kReadOrWriteFromFdSize > m_allocSize - m_writeOffset)
		{
			size_t newSize = 2 * m_allocSize + kReadOrWriteFromFdSize;
			char * newBuffer = static_cast<char *>(realloc(m_buffer, newSize));
			if (NULL == newBuffer)
			{
				return - 1;
			}
			free(m_buffer);
			m_buffer = newBuffer;
			m_allocSize = newSize;
		}
	}
	
	int nRead = read(fd, m_buffer + m_writeOffset, kReadOrWriteFromFdSize);
	if (0 > nRead)
	{
		if (EINTR == errno || EAGAIN == errno)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	else if (0 == nRead)
	{
		return 0;
	}
	
	m_writeOffset += nRead;
	return 1;
}

int SimpleBuffer::WriteToFd(int fd)
{
	char * buff = m_buffer + m_readOffset;
	size_t nToWrite = m_writeOffset - m_readOffset;
	if (0 < nToWrite)
	{
		int nWrite = write(fd, buff, nToWrite);
		if (0 > nWrite)
		{
			if (EINTR == errno || EAGAIN == errno)
			{
				return 1;
			}
			else
			{
				return -1;
			}
		}
		m_readOffset += nWrite;
	}
	
	if (m_readOffset == m_writeOffset)
	{
		m_readOffset = m_writeOffset = 0;
		return 0;
	}
	return 1;
}

char * SimpleBuffer::Buffer()
{
	m_buffer[m_writeOffset] = '\0';
	return (m_buffer + m_readOffset);
}

size_t SimpleBuffer::BufferSize()
{
	return (m_writeOffset - m_readOffset);
}

void SimpleBuffer::ReadFromBuffer(size_t nRead)
{
	m_readOffset += nRead;
}

void SimpleBuffer::WriteToBuffer(const char * data, size_t writeSize)
{
	// can not directly write writeSize bytes in the buffer
	if (writeSize > m_allocSize - m_writeOffset)
	{
		// if move data to head can satisfy to write writeSize bytes data to buffer, 
		// we do not malloc memory, just by moving data to the head
		if (NULL != m_buffer)
		{
			memmove(m_buffer, m_buffer + m_readOffset, m_writeOffset - m_readOffset);
			m_writeOffset -= m_readOffset;
			m_readOffset = 0;
		}
		
		// else need to realloc memory
		if (writeSize > m_allocSize - m_writeOffset)
		{
			size_t newSize = 2 * m_allocSize + writeSize;
			char * newBuffer = static_cast<char *>(realloc(m_buffer, newSize));
			if (NULL == newBuffer)
			{
				return;
			}
			free(m_buffer);
			m_buffer = newBuffer;
			m_allocSize = newSize;
		}
	}
	
	memmove(m_buffer + m_writeOffset, data, writeSize);
	m_writeOffset += writeSize;
}