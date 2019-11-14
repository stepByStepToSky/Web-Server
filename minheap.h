#pragma once

#ifndef MINHEAP_H
#define MINHEAP_H

#include <memory>
#include <sys/time.h>

class Channel;

class MinHeap
{
public:
    typedef std::shared_ptr<Channel> ChannelPtr;
    
    MinHeap();
    ~MinHeap();

    void Push(ChannelPtr ptChannel, const struct timeval & stActiveTime);
    void Pop();
    ChannelPtr Top();
    void Change(ChannelPtr ptChannel, const struct timeval & stActiveTime);
    void Delete(ChannelPtr ptChannel);
    bool Empty();
    void LevelPrint();
    
private:
    static bool Greater(const ChannelPtr & lth, const ChannelPtr & rth);
    void ShiftUp(ChannelPtr ptChannel);
    void ShiftDown(ChannelPtr ptChannel);

private:
    static const int kMinHeapInitSize;

    // index of data start from 1
    ChannelPtr * m_data;
    int m_size;
    int m_capacity;
};

static bool TimevalGreater(const struct timeval & lth, const struct timeval & rth)
{
    return lth.tv_sec > rth.tv_sec ? true : lth.tv_sec < rth.tv_sec ? false : lth.tv_usec > rth.tv_usec ? true : false;
}

#endif