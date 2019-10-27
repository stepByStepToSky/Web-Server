#pragma once

#ifndef SINGLETON_H
#define SINGLETON_H


#include <pthread.h>
#include <memory>
#include <iostream>

template<typename T>
class Singleton
{
public:
	static T & GetInstance()
	{
		pthread_once(&m_once, init);
		return (*(m_ptInstance.get()));
	}
	
private:
	static void init()
	{
		m_ptInstance = std::unique_ptr<T>(new T());
		//std::cout << "pthread_once init()" << std::endl;
	}

	Singleton() = delete;
	~Singleton() = delete;
	
private:
	static std::unique_ptr<T> m_ptInstance;
	static pthread_once_t m_once;
};

template<typename T>
std::unique_ptr<T> Singleton<T>::m_ptInstance;

template<typename T>
pthread_once_t Singleton<T>::m_once = PTHREAD_ONCE_INIT;

#endif