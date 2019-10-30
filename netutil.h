#pragma once

#ifndef NETUTIL_H
#define NETUTIL_H

#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>

class NetUtil
{
public:
	// if success, return nonblock socket fd
	static int Listen(const char * serverIp, uint16_t port);
	// if success, return block socket fd
	static int Connect(const char * serverIp, uint16_t port);
	
	
	static void SetReuseAddr(int fd, int optval = 1);
	static void SetNonblock(int fd);
	static void SetNoDelay(int fd, int optval = 1);
	static void SetAddr(const char * serverIp, uint16_t port, sockaddr_in & sockAddr);
};

#endif