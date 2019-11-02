#include <iostream>
#include <vector>
#include <unistd.h>
#include "../thread.h"
#include "../mutex.h"

using namespace std;

int globalVal = 0;
Mutex mutex;

void addGlobal()
{
	int i = 0;
	while(i < 200)
	{
		LockGuard lock(mutex);
		int val = globalVal;
		val += 1;
		usleep(1);
		globalVal = val;
		++i;
	}
}

int main()
{
	vector<shared_ptr<Thread>> vec;
	for (int i = 0; i < 100; ++i)
	{
		shared_ptr<Thread> ptThread = make_shared<Thread> (addGlobal);
		ptThread->Start();
		vec.push_back(ptThread);
	}
	
	for (int i = 0; i < 100; ++i)
	{
		vec[i]->Join();
	}
	
	cout << globalVal << endl;
}