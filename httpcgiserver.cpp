#include "httpcgiserver.h"
#include "cgidispatch.h"

#include <iostream>
using namespace std;

int HttpCgiServer::ReadRequestCallback(std::shared_ptr<Channel> ptChannel)
{
	SimpleBuffer & inBuffer = ptChannel->GetInBuffer();
	SimpleBuffer & outBuffer = ptChannel->GetOutBuffer();
	HttpMessage & httpMessage = ptChannel->GetHttpMessage();
	
	HttpMessage::ParseState state = httpMessage.Parse(inBuffer);
	if (HttpMessage::Again == state)
	{
		return 1;
	}
	else if (HttpMessage::Error == state)
	{
		httpMessage.BuildErrorRespond(httpMessage.GetRespondCode(), httpMessage.GetRespondMsg(), outBuffer);
		return 0;
	}
	
	/*
	cout << "ParseState=" << state << endl;
	cout << "httpmessage.GetMethod()=" << httpMessage.GetMethod() << endl;
	cout << "httpmessage.GetUrl()=" << httpMessage.GetUrl() << endl;
	cout << "httpmessage.GetHttpVersion()=" << httpMessage.GetHttpVersion() << endl;
	
	const map<string, string> & headContent = httpMessage.GetHeadContent();
	for (map<string, string>::const_iterator iter = headContent.begin(); headContent.end() != iter; ++iter)
	{
		cout << iter->first << "=" << iter->second << endl;
	}
	cout << "httpmessage.GetBody()=" << httpMessage.GetBody() << endl;
	httpMessage.Reset();
	//inBuffer.ReadFromBuffer(inBuffer.BufferSize());
	
	const char * respond = "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\nContent-Length:13\r\n\r\nHello World\r\n";
	outBuffer.WriteToBuffer(respond, strlen(respond));
	*/
	
	int respCode;
	std::string sRespMsg, sRespBody;
	
	BaseFactory::Init();
	BaseFactory::Url2CgiFactoryMapType url2CgiFactoryMap = BaseFactory::GetCgiFactoryMap();
	BaseFactory::Url2CgiFactoryMapType::const_iterator iter = url2CgiFactoryMap.find(httpMessage.GetUrl());
	if (url2CgiFactoryMap.end() == iter)
	{
		httpMessage.BuildErrorRespond(HttpMessage::NotFound, "Not Found This Cgi", outBuffer);
		return 0;
	}
	
	std::unique_ptr<BaseCgi> ptBaseCgi = iter->second->GetNewInstance();
	ptBaseCgi->Process(&httpMessage, respCode, sRespMsg, sRespBody);
	
	httpMessage.BuildCgiRespond(respCode, sRespMsg, sRespBody, outBuffer);
	httpMessage.Reset();
	
	return 1;
}

void HttpCgiServer::WriteCallback(std::shared_ptr<Channel> ptChannel)
{
	
}

void HttpCgiServer::ErrorCallback(std::shared_ptr<Channel> ptChannel)
{
	
}

HttpCgiServer::HttpCgiServer(const char * serverIp, uint16_t port) : m_tcpServer(serverIp, port)
{
	using namespace std::placeholders;
	m_tcpServer.SetReadCallback(std::bind(&HttpCgiServer::ReadRequestCallback, this, _1));
	m_tcpServer.SetWriteCallback(std::bind(&HttpCgiServer::WriteCallback, this, _1));
	m_tcpServer.SetErrorCallback(std::bind(&HttpCgiServer::ErrorCallback, this, _1));
}

void HttpCgiServer::Loop()
{
	m_tcpServer.Loop();
}