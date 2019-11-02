#include <iostream>
#include <vector>
#include <memory>
#include <unistd.h>
#include "../condition.h"
#include "../thread.h"

using namespace std;

Mutex mutex;
Condition cond(mutex);
vector<int> vec;

void produce()
{
	while(1)
	{
		LockGuard lock(mutex);
		usleep(100);
		vec.push_back(1);
		cout << "produce one, vec.size() = " << vec.size() << endl;
		cond.Notify();
	}
}

void consume()
{
	while(1)
	{
		LockGuard lock(mutex);
		while(vec.empty())
		{
			//cond.Wait();
			time_t startTime = time(NULL);
			if (cond.WaitForSeconds(3))
			{
				time_t endTime = time(NULL);
				cout << "Time out :" << static_cast<int>(endTime - startTime) << endl;
			}
			
		}
		
		usleep(100);
		vec.pop_back();
		cout << "consume one, vec.size() = " << vec.size() << endl;
	}
}

int main()
{
	int iprod = 1, icons = 1;
	vector<shared_ptr<Thread>> prod(iprod), cons(icons);
	for (int i = 0; i < iprod || i < icons; ++i)
	{
		if (i < icons)
		{
			cons[i] = make_shared<Thread> (consume);
			cons[i]->Start();
		}
		
		if (i < iprod)
		{
			prod[i] = make_shared<Thread> (produce);
			prod[i]->Start();
		}
	}
	
	for (int i = 0; i < iprod || i < icons; ++i)
	{
		if (i < icons)
		{
			cons[i]->Join();
		}
		
		if (i < iprod)
		{
			prod[i]->Join();
		}
	}
}