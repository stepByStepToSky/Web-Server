#include <iostream>
#include <string>
#include <unistd.h>
#include "../log.h"

using namespace std;

int main()
{
	ERRLOG("xiao cai cai");
	DEBUGLOG("xiao fan fan");
	
	ERRLOG("xiaocai %d%d%d%d", 0, 8, 1, 2);
	DEBUGLOG("xiaofan %d%d%d%d", 0, 9, 1, 4);
}