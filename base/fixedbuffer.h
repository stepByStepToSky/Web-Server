#pragma once

#ifndef FIXEDBUFFER_H
#define FIXEDBUFFER_H

//#pragma pack(1)

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>

template <size_t SIZE>
class FixedBuffer
{
public:
	FixedBuffer() : m_pos(0) {}
	FixedBuffer(const FixedBuffer &) = delete;
	FixedBuffer & operator= (const FixedBuffer &) = delete;
	
	void Append(const char * data, size_t writeSize)
	{
		//std::cout << __FILE__ << " " << __LINE__ << " " << __func__ << " writeSize="  << writeSize << " Avail()=" << Avail() << std::endl;
		memcpy(m_buff + m_pos, data, writeSize);
		m_pos += writeSize;
	}
	
	void Append(const std::string & str)
	{
		size_t writeSize = str.size();
		Append(str.c_str(), writeSize);
	}
	
	size_t Size()
	{
		return m_pos;
	}
	
	const char * Data()
	{
		m_buff[m_pos] = '\0';
		return m_buff;
	}
	
	size_t Avail()
	{
		//std::cout << __FILE__ << " " << __LINE__ << " " << __func__ << " SIZE="  << SIZE << " m_pos=" << m_pos << std::endl;
		return (SIZE - m_pos);
	}
	
	void Reset()
	{
		m_pos = 0;
	}
	
private:
	char m_buff[SIZE];
	size_t m_pos;
};

#endif