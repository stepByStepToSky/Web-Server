#include <iostream>
#include <vector>
#include <unistd.h>
#include <memory>
#include <stdint.h>
#include <cstdint>
#include "../mutex.h"

using namespace std;

vector<int> vec;
int globalInt = 0;


void * produce(void * arg)
{
	int i = 0;
	while(i < 5000)
	{
		Mutex * ptMutex = static_cast<Mutex *> (arg);
		LockGuard lock(*ptMutex);
		//ptMutex->Lock();
		int temp = globalInt;
		temp += 1;
		usleep(1);
		vec.push_back(temp);
		globalInt = temp;
		++i;
		//ptMutex->Unlock();
	}
	pthread_exit((void *)100);
}

int main()
{
	pthread_t one, another;
	Mutex mutex;
	pthread_create(&one, NULL, produce, &mutex);
	pthread_create(&another, NULL, produce, &mutex);
	void * pInt;
	pthread_join(one, (&pInt));
	cout << *((int *)(&pInt)) << endl;
	pthread_join(another, NULL);
	cout << globalInt << endl;
	
}
