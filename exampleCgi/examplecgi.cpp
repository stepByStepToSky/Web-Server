#include "examplecgi.h"

void ExampleCgi::ProcessImp(int & respondCode, std::string sRespMsg, std::string & sRespBoby)
{	
	// GET:  http://127.0.0.1:9508/?id=2020&name=lufanliao
	// POST: curl -X POST http://127.0.0.1:9508/ -D '{"id":"2020", "name":"lufan"}'
	
	sRespBoby.append("<html><title>Cgi Respond</title>");
	sRespBoby.append("<body bgcolor=\"ffffff\">");
	sRespBoby.append("Hello World");
	sRespBoby.append("\r\n");
	sRespBoby.append("url=");
	sRespBoby.append(GetUrl());
	sRespBoby.append("\r\n");
	sRespBoby.append("id=");
	sRespBoby.append(std::to_string(GetInt32withDefault("id", 10086)));
	sRespBoby.append("\r\n");
	sRespBoby.append("name=");
	sRespBoby.append(GetStringValue("name"));
	sRespBoby.append("\r\n");
	sRespBoby.append("body=");
	sRespBoby.append(GetBody());
	sRespBoby.append("\r\n");
	
	sRespBoby.append("<hr><em> Lufan's Cgi Respond</em>\n</body></html>");
}