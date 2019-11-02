#include <iostream>
#include <string>
#include "../fixedbuffer.h"

using namespace std;

template <size_t SIZE>
void printBufferInfo(FixedBuffer<SIZE> & buffer)
{
	cout << "Data = " << buffer.Data() << endl;
	cout << "Size = " << buffer.Size() << endl;
	cout << "Avail = " << buffer.Avail() << endl;
}

int main()
{
	FixedBuffer<1024> buffer;
	for (int i = 0; i < 10; ++i)
	{
		buffer.Append("xiaocai", 7);
	}
	printBufferInfo(buffer);
	
	buffer.Reset();
	buffer.Append("xiaofan", 7);
	printBufferInfo(buffer);
}