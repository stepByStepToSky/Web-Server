#include <iostream>
#include <vector>
#include "../minheap.h"
#include "../channel.h"

using namespace std;

typedef MinHeap::ChannelPtr ChannelPtr;

int main()
{
    MinHeap minHeap;
    
    vector<ChannelPtr> vec;
    vec.reserve(100);
    for (int i  = 1; 16 > i; ++i)
    {
        vec.push_back(make_shared<Channel>(-1));
        struct timeval stTime = {i, 0};
        vec.back()->SetLastActiveTime(stTime);
        minHeap.Push(vec.back(), vec.back()->GetLastActiveTime());
    }

    //minHeap.Change(vec[4], {0, 0});
    minHeap.Delete(vec[0]);
    minHeap.LevelPrint();
    
    cout << "---------------" << endl;
    while(!minHeap.Empty())
    {
        cout << minHeap.Top()->GetLastActiveTime().tv_sec << " ";
        minHeap.Pop();
    }
    cout << endl;
}