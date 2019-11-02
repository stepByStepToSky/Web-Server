#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include "../asynclog.h"
#include "../thread.h"

using namespace std;


string str;
AsyncLog asynclog;

void WriteLog()
{
	for (long long i = 0; i < 10000; ++i)
	{
		//std::cout << __FILE__ << " " << __LINE__ << " " << __func__ << " "  << str.size() << std::endl;
		asynclog.Append(str.data(), str.size());
	}
}


int main()
{
	asynclog.Start();
	for(long long n = 1; n < 10; ++n)
	{
		for (int i = 0; i < 26; ++i)
		{
			str.push_back('a' + i);
			//asynclog.NotifyLog();
		}
	}
	str.push_back('\n');
	
	
	int threadNum = 12;
	std::vector<std::unique_ptr<Thread>> m_threads;
	m_threads.reserve(threadNum);
	for (int i = 0; i < threadNum; ++i)
	{
		m_threads.emplace_back(new Thread(WriteLog));
		m_threads[i]->Start();
	}
	
	for (int i = 0; i < threadNum; ++i)
	{
		m_threads[i]->Join();
	}
	
	asynclog.Stop();
}