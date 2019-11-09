#include <iostream>
#include <string.h>

#include "../tcpserver.h"
#include "../httpmessage.h"

using namespace std;

int ReadCallback(std::shared_ptr<Channel> ptChannel);
void WriteCallback(std::shared_ptr<Channel> ptChannel);
void ErrorCallback(std::shared_ptr<Channel> ptChannel);

const char * servIp = "127.0.0.1";
const int port = 9508;

int main()
{
	TcpServer server(servIp, port);
	server.SetReadCallback(ReadCallback);
	server.SetWriteCallback(WriteCallback);
	server.SetErrorCallback(ErrorCallback);
	server.Loop();
}

int ReadCallback(std::shared_ptr<Channel> ptChannel)
{
	cout << "read from " << ptChannel->GetFd() << endl;
	SimpleBuffer & inBuffer = ptChannel->GetInBuffer();
	SimpleBuffer & outBuffer = ptChannel->GetOutBuffer();
	cout << inBuffer.Buffer();
	cout << "inBuffer.Find(' ') = " << inBuffer.Find(' ') << endl;
	cout << "inBuffer.Find(\"\\r\\n\") = " << inBuffer.Find("\r\n") << endl;
	
	HttpMessage httpmessage;
	HttpMessage::ParseState state = httpmessage.Parse(inBuffer);
	cout << "ParseState=" << state << endl;
	cout << "httpmessage.GetMethod()=" << httpmessage.GetMethod() << endl;
	cout << "httpmessage.GetUrl()=" << httpmessage.GetUrl() << endl;
	cout << "httpmessage.GetHttpVersion()=" << httpmessage.GetHttpVersion() << endl;
	
	const map<string, string> & headContent = httpmessage.GetHeadContent();
	for (map<string, string>::const_iterator iter = headContent.begin(); headContent.end() != iter; ++iter)
	{
		cout << iter->first << "=" << iter->second << endl;
	}
	cout << "httpmessage.GetBody()=" << httpmessage.GetBody() << endl;
	
	inBuffer.ReadFromBuffer(inBuffer.BufferSize());
	
	const char * respond = "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\nContent-Length:13\r\n\r\nHello World\r\n";
	outBuffer.WriteToBuffer(respond, strlen(respond));
	return 1;
}

void WriteCallback(std::shared_ptr<Channel> ptChannel)
{
	cout << "write from " << ptChannel->GetFd() << " done" << endl;
}

void ErrorCallback(std::shared_ptr<Channel> ptChannel)
{
	cout << "error from " << ptChannel->GetFd() << endl;
}