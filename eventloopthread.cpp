#include "eventloopthread.h"
#include "base/log.h"

EventLoopThread::EventLoopThread()
{
	pipe2(m_pipeFd, O_CLOEXEC | O_NONBLOCK);
	m_listenChannel = std::make_shared<Channel>(m_pipeFd[0]);
	m_vecQue.reserve(32);
}

EventLoopThread::~EventLoopThread()
{
	Quit();
	close(m_pipeFd[0]);
	close(m_pipeFd[1]);
}

void EventLoopThread::OnReadHandler(EventLoop & eventLoop, std::shared_ptr<Channel> ptChannel)
{
	char buff[1024];
	read(m_pipeFd[0], buff, sizeof(buff));
	
	LockGuard lock(m_mutex);
	size_t queSize = m_vecQue.size();
	for (size_t i = 0; i < queSize; ++i)
	{
		ChannelPtr ptChannel = std::make_shared<Channel>(m_vecQue[i]);
		ptChannel->SetReadCallback(m_readCallback);
		ptChannel->SetWriteCallback(m_writeCallback);
		ptChannel->SetErrorCallback(m_errorCallback);
		m_eventLoop.AddChannel(ptChannel, EPOLLIN);
		m_eventLoop.AddLastActivedTime(ptChannel, m_eventLoop.GetLastActiveTime());
		DEBUGLOG("%s %s %d, EventLoopThread threadId=%d add socket fd=%d", __FILE__, __func__, __LINE__, m_threadId, m_vecQue[i]);
	}
	m_vecQue.clear();
}

void EventLoopThread::Loop()
{
	using namespace std::placeholders; 
	m_listenChannel->SetReadCallback(std::bind(&EventLoopThread::OnReadHandler, this, _1, _2));
	m_eventLoop.AddChannel(m_listenChannel, EPOLLIN);
	m_eventLoop.Loop();
}

void EventLoopThread::PushFd(int fd)
{
	LockGuard lock(m_mutex);
	m_vecQue.push_back(fd);
	write(m_pipeFd[1], " ", 1);
}

void EventLoopThread::SetReadCallback(CallbackType readCallback)
{
	m_readCallback = readCallback;
}

void EventLoopThread::SetWriteCallback(CallbackType writeCallback)
{
	m_writeCallback = writeCallback;
}

void EventLoopThread::SetErrorCallback(CallbackType errorCallback)
{
	m_errorCallback = errorCallback;
}

void EventLoopThread::Quit()
{
	m_eventLoop.Quit();
}

void EventLoopThread::SetThreadId(int threadId)
{
	m_threadId = threadId;
}