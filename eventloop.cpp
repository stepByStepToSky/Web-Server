#include "eventloop.h"
#include "channel.h"

const int EventLoop::kActiveEventInitCnt = 4;
const int EventLoop::kMilliseconds = 10000;

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

const struct timeval & EventLoop::GetLastActiveTime()
{
	return m_lastActiveTime;
}

void EventLoop::AddLastActivedTime(ChannelPtr ptChannel, const struct timeval & stLastActiveTime)
{
	m_minHeapChannel.Push(ptChannel, stLastActiveTime);
}

void EventLoop::ChangeLastActivedTime(ChannelPtr ptChannel, const struct timeval & stLastActiveTime)
{
	m_minHeapChannel.Change(ptChannel, stLastActiveTime);
}

void EventLoop::DeleteLastActivedTime(ChannelPtr ptChannel)
{
	m_minHeapChannel.Delete(ptChannel);
}

void EventLoop::Loop()
{
	m_started = true;
	while(m_started)
	{
		int timeout = -1;
		if (!m_minHeapChannel.Empty())
		{
			struct timeval nowTime;
			gettimeofday(&nowTime, NULL);
			const struct timeval & minTime = m_minHeapChannel.Top()->GetLastActiveTime();
			timeout = (minTime.tv_sec - nowTime.tv_sec) * 1000 + ((minTime.tv_usec - nowTime.tv_usec) / 1000) + kMilliseconds;
			if (timeout < 0)
			{
				timeout = 0;
			}
		}

		m_activeEventsCnt = m_epoller.EpollDispatch(m_vecActiveEvents, timeout);
		ProcessActiveEvents();
		ProcessActiveTimeEvents();
	}
}

void EventLoop::EventLoop::Quit()
{
	m_started = false;
}

void EventLoop::ProcessActiveEvents()
{
	gettimeofday(&m_lastActiveTime, NULL);

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

void EventLoop::ProcessActiveTimeEvents()
{
	long long milliTime = m_lastActiveTime.tv_sec * 1000 + (m_lastActiveTime.tv_usec / 1000) - kMilliseconds;
	struct timeval activeExpiredTime = {milliTime / 1000, (milliTime % 1000)*1000};

	while(!m_minHeapChannel.Empty() && TimevalGreater(activeExpiredTime, m_minHeapChannel.Top()->GetLastActiveTime()))
	{
		static const char replyMsg[] = "timeout";
		const ChannelPtr ptChannel = m_minHeapChannel.Top();
		m_minHeapChannel.Pop();
		write(ptChannel->GetFd(), replyMsg, sizeof(replyMsg));
		RemoveChannel(ptChannel, EPOLLIN | EPOLLOUT);
		close(ptChannel->GetFd());
		DeleteLastActivedTime(ptChannel);
	}
}