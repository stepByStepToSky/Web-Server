#pragma once

#ifndef WRITEFILE_H
#define WRITEFILE_H

#include <algorithm>
#include <string>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

class WriteFile
{
public:
	WriteFile(std::string sFilePath);
	~WriteFile();

	WriteFile(const WriteFile &) = delete;
	WriteFile & operator= (const WriteFile &) = delete;
	
	void Append(const char * data, size_t writeSize);
	void Flush();
	void Sync();
	void Close();
	
private:
	void WriteUnbuffered(const char * data, size_t writeSize);
	
private:
	static const size_t kWriteFileBufferSize = 65536;
	
	int m_fd;
	// m_buff[0, m_pos - 1] contains data to be written to m_fd.
	char m_buff[kWriteFileBufferSize];
	size_t m_pos;
};

#endif