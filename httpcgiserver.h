#pragma once

#ifndef HTTPCGISERVER_H
#define HTTPCGISERVER_H

#include <functional>
#include <memory>
#include <stdint.h>
#include "channel.h"
#include "tcpserver.h"
#include "httpmessage.h"

class HttpCgiServer
{
public:
	HttpCgiServer(const char * serverIp, uint16_t port, int threadCnt = 0);
	
	int ReadRequestCallback(std::shared_ptr<Channel> ptChannel);
	void WriteCallback(std::shared_ptr<Channel> ptChannel);
	void ErrorCallback(std::shared_ptr<Channel> ptChannel);
	void Loop();
	
private:
	TcpServer m_tcpServer;
};

#endif