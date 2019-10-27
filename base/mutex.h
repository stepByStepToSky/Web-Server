#pragma once

#ifndef MUTEX_H
#define MUTEX_H

#include <iostream>
#include <pthread.h>

class Mutex
{
public:
	Mutex();
	~Mutex();
	
	Mutex(const Mutex &) = delete;
	Mutex & operator= (const Mutex &) = delete;
	
	void Lock();
	void Unlock();
	bool Trylock();
	
	pthread_mutex_t * GetMutex();

private:
	pthread_mutex_t m_mutex;
};



class LockGuard
{
public:
	LockGuard(Mutex & mutex);
	~LockGuard();
	
private:
	Mutex & m_mutex;
};
#endif