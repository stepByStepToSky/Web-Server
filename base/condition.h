#pragma once

#ifndef CONDITION_H
#define CONDITION_H

#include <iostream>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include "mutex.h"

class Condition
{
public:
	Condition(Mutex & mutex);
	~Condition();
	
	void Wait();
	bool WaitForSeconds(double seconds);
	void Notify();
	void NotifyAll();

private:
	Mutex & m_mutex;
	pthread_cond_t m_cond;
};


#endif