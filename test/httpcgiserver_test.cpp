#include "../httpcgiserver.h"

const char * servIp = "127.0.0.1";
const int port = 9508;

int main()
{
	HttpCgiServer cgiServer(servIp, port);
	cgiServer.Loop();
}