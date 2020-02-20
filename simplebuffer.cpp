#include "simplebuffer.h"

const size_t SimpleBuffer::kReadOrWriteFromFdSize = 4096;

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
		if (NULL != m_buffer && 0 != m_readOffset)
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

const char * SimpleBuffer::Buffer()
{
	if (m_allocSize > m_writeOffset)
	{
		m_buffer[m_writeOffset] = '\0';
	}
	
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
		if (NULL != m_buffer && 0 != m_readOffset)
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
			m_buffer = newBuffer;
			m_allocSize = newSize;
		}
	}
	
	memmove(m_buffer + m_writeOffset, data, writeSize);
	m_writeOffset += writeSize;
}

void SimpleBuffer::Append(const char * format, ...)
{
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	int nWrite = 0;
	va_list ap;
	va_start(ap, format);
	if (0 < (nWrite = vsnprintf(buff, sizeof(buff), format, ap)))
	{
		WriteToBuffer(buff, nWrite);
	}
	va_end(ap);
}

ssize_t SimpleBuffer::Find(const char c)
{
	for (size_t i = m_readOffset; i < m_writeOffset; ++i)
	{
		if (c == m_buffer[i])
		{
			return (i - m_readOffset);
		}
	}
	return -1;
}

ssize_t SimpleBuffer::Find(const char * src)
{
	const size_t srcLen = strlen(src);
	for (size_t i = m_readOffset; i + srcLen <= m_writeOffset; ++i)
	{
		size_t j = 0;
		for (; j < srcLen; ++j)
		{
			if (src[j] != m_buffer[i+j])
			{
				break;
			}
		}
		
		if (srcLen == j)
		{
			return (i - m_readOffset);
		}
	}
	return -1;
}

bool SimpleBuffer::EndWith(const char * src)
{
	const size_t srcLen = strlen(src);
	for (size_t i = 0; i < srcLen && i < BufferSize(); ++i)
	{
		if (src[srcLen - 1 - i] != m_buffer[m_writeOffset - 1 - i])
		{
			return false;
		}
	}
	return true;
}

bool SimpleBuffer::EndWithBackEndLength(const char * src, size_t length)
{
	if (length > BufferSize())
	{
		return false;
	}
	
	const size_t srcLen = strlen(src);
	size_t bufferEnd = m_readOffset + length;
	for (size_t i = 0; i < srcLen && i < BufferSize(); ++i)
	{
		if (src[srcLen - 1 - i] != m_buffer[bufferEnd - 1 - i])
		{
			return false;
		}
	}
	return true;
}

void SimpleBuffer::DropEnd(size_t n)
{
	if (n <= BufferSize())
	{
		m_writeOffset -= n;
	}
}