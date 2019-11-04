#include <fstream>

#include "logincgi.h"
#include "../httpmessage.h"

static void ReadFile(std::string sPath, std::string & sRespond)
{
	std::ifstream is(sPath.c_str(), std::ifstream::in);
	is.seekg(0, is.end);
	int flength = is.tellg();
	is.seekg(0, is.beg);
	char * buffer = new char[flength];
	is.read(buffer, flength);
	sRespond.assign(buffer, flength);
}

void LoginCgi::ProcessImp(int & respondCode, std::string sRespMsg, std::string & sContentType, std::string & sRespBoby)
{
	const std::string & sFileName = GetStringValue("fileName");
	
	// http://127.0.0.1:9508/cgi-bin/login
	if (sFileName.empty())
	{
		ReadFile("../webFile/login/login.html", sRespBoby);
		return;
	}
	
	size_t pos = sFileName.find_last_of('.');
	if (std::string::npos == pos)
	{
		ReadFile("../webFile/login/login.html", sRespBoby);
		return;
	}
	
	std::string sPostStr(sFileName.begin() + pos, sFileName.end());
	const HttpMessage::ContentTypeMapType & contentTypeMap = HttpMessage::GetContentTypeMap();
	HttpMessage::ContentTypeMapType::const_iterator iter = contentTypeMap.find(sPostStr);
	if (contentTypeMap.end() != iter)
	{
		sContentType = iter->second;
	}
	ReadFile("../webFile/login/" + sFileName, sRespBoby);
}