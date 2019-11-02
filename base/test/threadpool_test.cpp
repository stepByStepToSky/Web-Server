#include <iostream>
#include <unistd.h>
#include <unordered_set>
#include <time.h>
#include "../threadpool.h"

using namespace std;

Mutex mutex;
int globalCnt = 0;

void SayHelloWorld()
{
	usleep(1000);
	LockGuard lock(mutex);
	cout << (unsigned int)pthread_self() << ", Hello World" << endl;
	++globalCnt;
}

class TimeAlloc
{
public:
	TimeAlloc(time_t startTime): m_startTime(startTime) {}
	~TimeAlloc()
	{
		time_t endTime = time(NULL);
		cout << "time, " << (unsigned int)(endTime - m_startTime) << endl;
		cout << "cnt, " << globalCnt << endl;
	}
	
	time_t m_startTime;
};

int main()
{
	time_t startTime = time(NULL);
	TimeAlloc timeCnt(startTime);
	
	ThreadPool threadPool(100);
	threadPool.Start(2);
	for (int i = 0; i < 10000; ++i)
	{
		threadPool.Run(SayHelloWorld);
	}
}