#pragma once

#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <unordered_map>
#include <vector>
#include <memory>
#include <sys/time.h>

#include "epoller.h"
#include "minheap.h"

class Channel;

class EventLoop
{
public:
	typedef std::shared_ptr<Channel> ChannelPtr;
	typedef Epoller::vecActiveEventType vecActiveEventType;
	
	EventLoop();
	
	// added events are all persist, until you delete them
	void AddChannel(ChannelPtr ptChannel, int addEvents);
	void RemoveChannel(ChannelPtr ptChannel, int deleteEvents);
	
	const struct timeval & GetLastActiveTime();
	void AddLastActivedTime(ChannelPtr ptChannel, const struct timeval & stLastActiveTime);
	void ChangeLastActivedTime(ChannelPtr ptChannel, const struct timeval & stLastActiveTime);
	void DeleteLastActivedTime(ChannelPtr ptChannel);

	void Loop();
	void Quit();
	void ProcessActiveEvents();
	size_t EventSize();
	void ProcessActiveTimeEvents();

private:
	bool FindChannelPtr(ChannelPtr ptChannel);
	
private:
	static const int kActiveEventInitCnt;
	static const int kMilliseconds;
	
	bool m_started;
	std::unordered_map<int, ChannelPtr> m_fd2ChannelPtrMap;
	int m_activeEventsCnt;
	vecActiveEventType m_vecActiveEvents;
	Epoller m_epoller;
	MinHeap m_minHeapChannel;
	struct timeval m_lastActiveTime;
};

#endif