#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../writefile.h"

using namespace std;

int main()
{
	WriteFile write("./temp.log");
	
	string str;
	for (int i = 0; i < 26; ++i)
	{
		str.push_back('a' + i);
	}
	
	for (int i = 0; i < 10000; ++i)
	{
		string sId = to_string(i);
		write.Append(sId.c_str(), sId.size());
		write.Append(" : ", 3);
		write.Append(str.c_str(), str.size());
		write.Append("\n", 1);
	}
	
	write.Sync();
}