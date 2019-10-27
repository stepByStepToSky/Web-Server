#pragma once

#ifndef EPOLLER_H
#define EPOLLER_H


#include <vector>
#include <functional>
#include <stdint.h>
#include <sys/epoll.h>
#include <string.h>
#include <errno.h>

#include "base/log.h"


class Epoller
{
public:
	typedef std::vector<struct epoll_event> vecActiveEventType;

	Epoller();
	~Epoller();
	
	int EpollCtl(int fd, int op, uint32_t events);
	// note that timeout argument specifies the number of ***milliseconds*** that epoll_wait() will block.
	int EpollDispatch(vecActiveEventType & activeEvents, int timeout);
	
private:
	int m_epfd;
};

#endif