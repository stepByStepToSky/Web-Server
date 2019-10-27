#include "epoller.h"


Epoller::Epoller()
{
	m_epfd = epoll_create1(EPOLL_CLOEXEC);
}
Epoller::~Epoller()
{
	close(m_epfd);
}

int Epoller::EpollCtl(int fd, int op, uint32_t events)
{
	struct epoll_event stEpev;
	stEpev.data.fd = fd;
	stEpev.events = events;
	
	/*
	EPOLL_CTL_ADD
			Add fd to the interest list and associate the settings
			specified in event with the internal file linked to fd
	EPOLL_CTL_MOD
			Change the settings associated with fd in the interest list to
            the new settings specified in event
	EPOLL_CTL_DEL
			Remove (deregister) the target file descriptor fd from the
            interest list.  The event argument is ignored and can be NULL
		
	*/
	if (0 == epoll_ctl(m_epfd, op, fd, &stEpev))
	{
		DEBUGLOG("%s %s %d, fd = %d, op = %d", __FILE__, __func__, __LINE__, fd, op);
		return 0;
	}
	
	// try more types
	switch(op)
	{
		case EPOLL_CTL_MOD:
			if (ENOENT == errno)
			{
				if (0 == epoll_ctl(m_epfd, EPOLL_CTL_ADD, fd, &stEpev))
				{
					DEBUGLOG("%s %s %d, Epoll MOD(%d) on %d retried as ADD; succeeded", __FILE__, __func__, __LINE__, op, fd);
					return 0;
				}
				else
				{
					ERRLOG("%s %s %d, Epoll MOD(%d) on %d retried as ADD; that failed too", __FILE__, __func__, __LINE__, op, fd);
					return -1;
				}
			}
			break;
		case EPOLL_CTL_ADD:
			if (EEXIST == errno)
			{
				if (0 == epoll_ctl(m_epfd, EPOLL_CTL_MOD, fd, &stEpev))
				{
					DEBUGLOG("%s %s %d, Epoll ADD(%d) on %d retried as MOD; succeeded", __FILE__, __func__, __LINE__, op, fd);
					return 0;
				}
				else
				{
					ERRLOG("%s %s %d, Epoll ADD(%d) on %d retried as MOD; that failed too", __FILE__, __func__, __LINE__, op, fd);
					return -1;
				}
			}
			break;
		case EPOLL_CTL_DEL:
			if (ENOENT == errno || EBADF == errno || EPERM == errno)
			{
				DEBUGLOG("%s %s %d, Epoll DEL(%d) on fd %d gave %s: DEL was unnecessary. %s", __FILE__, __func__, __LINE__, op, fd, strerror(errno));
				return 0;
			}
			break;
		default:
			break;
	}
	
	ERRLOG("%s %s %d, Epoll Ctl(%d) on %d failed", __FILE__, __func__, __LINE__, op, fd);
	return -1;
}

int Epoller::EpollDispatch(vecActiveEventType & activeEvents, int timeout)
{
	int nRet = epoll_wait(m_epfd, &activeEvents[0], activeEvents.capacity(), timeout);
	if (-1 == nRet)
	{
		if (EINTR != errno)
		{
			ERRLOG("%s %s %d, Epoll Wait failed", __FILE__, __func__);
			return -1;
		}
		return 0;
	}
	
	if (activeEvents.capacity() == nRet)
	{
		// since fd_max limits, it will never overflow
		size_t newSize = 2 * (activeEvents.capacity());
		activeEvents.resize(newSize);
		DEBUGLOG("%s %s %d, activeEvents Resize to %d", __FILE__, __func__, __LINE__, newSize);
	}
	return nRet;
}