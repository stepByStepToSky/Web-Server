#pragma once

#ifndef NETUTIL_H
#define NETUTIL_H

struct sockaddr_in;

class NetUtil
{
public:
	// if success, return nonblock socket fd
	static int Listen(const char * serverIp, uint16_t port);
	// if success return block socket fd
	static int Connect(const char * serverIp, uint16_t port);
	
	
	static void SetReuseAddr(int fd, int optval = 1);
	static void SetNonblock(int fd);
	static void SetNoDelay(int fd, int optval = 1);
	static void SetAddr(const char * serverIp, uint16_t port, sockaddr_in & sockAddr);
};

#endif