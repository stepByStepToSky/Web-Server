#pragma once

#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <unordered_map>
#include <vector>
#include <memory>

#include "epoller.h"

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
	
	void Loop();
	void Quit();
	void ProcessActiveEvents();
	size_t EventSize();
	
private:
	bool FindChannelPtr(ChannelPtr ptChannel);
	
private:
	static const int kActiveEventInitCnt;
	bool m_started;
	std::unordered_map<int, ChannelPtr> m_fd2ChannelPtrMap;
	int m_activeEventsCnt;
	vecActiveEventType m_vecActiveEvents;
	Epoller m_epoller;
};

#endif