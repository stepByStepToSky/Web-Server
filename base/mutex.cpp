#include "mutex.h"

Mutex::Mutex()
{
	pthread_mutex_init(&m_mutex, NULL);
}
	
Mutex::~Mutex()
{
	pthread_mutex_destroy(&m_mutex);
}
	
void Mutex::Lock()
{
	pthread_mutex_lock(&m_mutex);
}
	
void Mutex::Unlock()
{
	pthread_mutex_unlock(&m_mutex);
}
	
bool Mutex::Trylock()
{
	return EBUSY != pthread_mutex_trylock(&m_mutex);
}
	
pthread_mutex_t * Mutex::GetMutex()
{
	return &m_mutex;
}




LockGuard::LockGuard(Mutex & mutex) :m_mutex(mutex)
{
	m_mutex.Lock();
}
	
LockGuard::~LockGuard()
{
	m_mutex.Unlock();
}