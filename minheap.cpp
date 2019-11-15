#include <queue>
#include <iostream>
#include "minheap.h"
#include "channel.h"

const int MinHeap::kMinHeapInitSize = 8;

MinHeap::MinHeap() : m_data(nullptr), m_size(0), m_capacity(0)
{

}

MinHeap::~MinHeap()
{
    if (nullptr != m_data)
    {
        free(m_data);
    }
}

bool MinHeap::Greater(const ChannelPtr & lth, const ChannelPtr & rth)
{
    const struct timeval & lthLastTime = lth->GetLastActiveTime();
    const struct timeval & rthLastTime = rth->GetLastActiveTime();

    return TimevalGreater(lthLastTime, rthLastTime);
}

void MinHeap::ShiftUp(ChannelPtr ptChannel)
{
    int index = ptChannel->GetMinHeapIndex();
    
    // channel has been removed, ignore
    if (-1 == index)
    {
        return;
    }

    while(1 < index && Greater(m_data[index / 2], ptChannel))
    {
        m_data[index / 2]->SetMinHeapIndex(index);
        m_data[index] = m_data[index / 2];
        index /= 2;
    }
    m_data[index] = ptChannel;
    ptChannel->SetMinHeapIndex(index);
}

void MinHeap::ShiftDown(ChannelPtr ptChannel)
{
    int index = ptChannel->GetMinHeapIndex();
    
    // channel has been removed, ignore
    if (-1 == index)
    {
        return;
    }

    while(2 * index <= m_size)
    {
        int minIndex = 2 * index;
        if (minIndex < m_size && Greater(m_data[minIndex], m_data[minIndex + 1]))
        {
            ++minIndex;
        }

        if (Greater(m_data[minIndex], ptChannel))
        {
            break;
        }

        m_data[minIndex]->SetMinHeapIndex(index);
        m_data[index] = m_data[minIndex];
        index = minIndex;
    }
    m_data[index] = ptChannel;
    ptChannel->SetMinHeapIndex(index);
}

void MinHeap::Push(ChannelPtr ptChannel, const struct timeval & stActiveTime)
{
    if (-1 != ptChannel->GetMinHeapIndex())
    {
        Change(ptChannel, stActiveTime);
        return;
    }

    if (m_size + 1 >= m_capacity)
    {
        int newCapacity = (0 == m_size ? kMinHeapInitSize : (2 * m_size));
        ChannelPtr * newData = static_cast<ChannelPtr *>(realloc(m_data, sizeof(ChannelPtr) * newCapacity));
        
        // ignore this channel if realloc failed
        if (nullptr == newData)
        {
            return;
        }

        // realloc does not need free
        m_data = newData;
        m_capacity = newCapacity;
    }

    ++m_size;
	new (&m_data[m_size]) ChannelPtr(nullptr);
    m_data[m_size] = ptChannel;
    ptChannel->SetLastActiveTime(stActiveTime);
    ptChannel->SetMinHeapIndex(m_size);
    ShiftUp(m_data[m_size]);
}

void MinHeap::Pop()
{
    m_data[m_size]->SetMinHeapIndex(1);
    m_data[1]->SetMinHeapIndex(-1);
    m_data[1] = m_data[m_size];
    --m_size;
    ShiftDown(m_data[1]);
    m_data[m_size + 1] = nullptr;
}

MinHeap::ChannelPtr MinHeap::Top()
{
    return m_data[1];
}

void MinHeap::Change(ChannelPtr ptChannel, const struct timeval & stActiveTime)
{
    if (-1 == ptChannel->GetMinHeapIndex())
    {
        Push(ptChannel, stActiveTime);
        return;
    }

    const struct timeval oldTimeval = ptChannel->GetLastActiveTime();
    ptChannel->SetLastActiveTime(stActiveTime);
    if (TimevalGreater(oldTimeval, stActiveTime))
    {
        ShiftUp(ptChannel);
    }
    else
    {
        ShiftDown(ptChannel);
    }
}

void MinHeap::Delete(ChannelPtr ptChannel)
{
    int index = ptChannel->GetMinHeapIndex();
    // channel has been removed, ignore
    if (-1 == index)
    {
        return;
    }
    else if (1 == index)
    {
        Pop();
    }
    else
    {
        ptChannel->SetMinHeapIndex(-1);
        m_data[m_size]->SetMinHeapIndex(index);
        m_data[index] = m_data[m_size];
        --m_size;
        ShiftDown(m_data[index]);
        m_data[m_size + 1] = nullptr;
    }
}

bool MinHeap::Empty()
{
    return 0 == m_size;
}

void MinHeap::LevelPrint()
{
	if (0 != m_size)
	{
		std::queue<int> que;
		que.push(1);
		while (!que.empty())
		{
			int front = que.front();
			std::cout << m_data[front]->GetLastActiveTime().tv_sec << " ";
			que.pop();

			if (2 * front <= m_size)
			{
				que.push(2 * front);
				if (2 * front + 1 <= m_size)
				{
					que.push(2 * front + 1);
				}
			}
		}
		std::cout << std::endl;
	}
}