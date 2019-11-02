#include <iostream>
#include "../singleton.h"

using namespace std;

int main()
{
	int & a = Singleton<int>::GetInstance();
	int & b = Singleton<int>::GetInstance();
	
	if ((&a) == (&b))
	{
		cout << "Singleton True" << endl;
	}
	else
	{
		cout << "Singleton False" << endl;
	}
}