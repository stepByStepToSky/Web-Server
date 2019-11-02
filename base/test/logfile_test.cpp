#include <iostream>
#include <string>
#include <unistd.h>
#include "../logfile.h"

using namespace std;

int main()
{
	LogFile logfile;
	string str;
	for (int i = 0; i < 26; ++i)
	{
		str.push_back('a' + i);
	}
	
	for (long long i = 0; i < 100000000000000000; ++i)
	{
		string sId = to_string(i);
		logfile.Append(sId.c_str(), sId.size());
		logfile.Append(" : ", 3);
		logfile.Append(str.c_str(), str.size());
		logfile.Append("\n", 1);
		usleep(1000000);
		if (10 < i)
		{
			return 0;
		}
	}
}