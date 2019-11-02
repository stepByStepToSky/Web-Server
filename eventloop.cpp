#include "eventloop.h"
#include "channel.h"

const int EventLoop::kActiveEventInitCnt = 4;

EventLoop::EventLoop() : m_started(false)
{
	m_vecActiveEvents.resize(kActiveEventInitCnt);
}

bool EventLoop::FindChannelPtr(ChannelPtr ptChannel)
{
	return m_fd2ChannelPtrMap.end() != m_fd2ChannelPtrMap.find(ptChannel->GetFd());
}

size_t EventLoop::EventSize()
{
	return m_fd2ChannelPtrMap.size();
}

void EventLoop::AddChannel(ChannelPtr ptChannel, int addEvents)
{
	if (0 != addEvents)
	{
		if (FindChannelPtr(ptChannel))
		{
			int oldEvents = ptChannel->GetEvents();
			ptChannel->SetEvents((oldEvents | addEvents));
			m_epoller.EpollCtl(ptChannel->GetFd(), EPOLL_CTL_MOD, ptChannel->GetEvents());
		}
		else
		{
			ptChannel->SetEvents(addEvents);
			m_epoller.EpollCtl(ptChannel->GetFd(), EPOLL_CTL_ADD, ptChannel->GetEvents());
			m_fd2ChannelPtrMap.insert(make_pair(ptChannel->GetFd(), ptChannel));
		}
	}
}

void EventLoop::RemoveChannel(ChannelPtr ptChannel, int deleteEvents)
{
	if (FindChannelPtr(ptChannel))
	{
		int oldEvents = ptChannel->GetEvents();
		// oldEvents = oldEvents & (~deleteEvents);
		if (EPOLLIN & deleteEvents)
		{
			oldEvents = oldEvents & (~EPOLLIN);
		}
		if (EPOLLOUT & deleteEvents)
		{
			oldEvents = oldEvents & (~EPOLLOUT);
		}
		
		if (0 == oldEvents)
		{
			ptChannel->SetEvents(0);
			m_epoller.EpollCtl(ptChannel->GetFd(), EPOLL_CTL_DEL, 0);
			m_fd2ChannelPtrMap.erase(ptChannel->GetFd());
		}
		else
		{
			ptChannel->SetEvents(oldEvents);
			m_epoller.EpollCtl(ptChannel->GetFd(), EPOLL_CTL_MOD, ptChannel->GetEvents());
		}
	}
}

void EventLoop::Loop()
{
	m_started = true;
	while(m_started)
	{
		m_activeEventsCnt = m_epoller.EpollDispatch(m_vecActiveEvents, -1);
		ProcessActiveEvents();
	}
}

void EventLoop::EventLoop::Quit()
{
	m_started = false;
}

void EventLoop::ProcessActiveEvents()
{
	for (int i = 0; i < m_activeEventsCnt; ++i)
	{
		std::unordered_map<int, ChannelPtr>::iterator iter = m_fd2ChannelPtrMap.find(m_vecActiveEvents[i].data.fd);
		if (m_fd2ChannelPtrMap.end() != iter)
		{
			// hold this channel to guarantee this process is safe
			ChannelPtr ptChannel = iter->second;
			ptChannel->HandleEvent(*this, ptChannel, m_vecActiveEvents[i].events);	
		}
	}
	m_activeEventsCnt = 0;
}