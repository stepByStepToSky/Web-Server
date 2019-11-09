#include <fstream>
#include <string>
#include <algorithm>

#include "uploadfilecgi.h"

static void ReadFile(std::string sPath, std::string & sRespond)
{
	std::ifstream is(sPath.c_str(), std::ifstream::in);
	is.seekg(0, is.end);
	int flength = is.tellg();
	is.seekg(0, is.beg);
	flength = std::max(flength, 1);
	std::string buffer;
	buffer.resize(flength);
	is.read(&buffer[0], flength);
	sRespond.assign(buffer.data(), flength);
}

static void WriteFileToDisk(const std::string & sContent, const std::string & sFilePath)
{
	std::ofstream fout(sFilePath.c_str(), std::ios::binary);
	fout.write(sContent.c_str(), sizeof(char) * (sContent.size()));
	fout.close();
}

void UploadFileCgi::ProcessImp(int & respondCode, std::string sRespMsg, std::string & sContentType, std::string & sRespBoby)
{
	const std::string & sFileName = GetStringValue("fileName");
	
	// http://127.0.0.1:9508/cgi-bin/uploadFileCgi
	if (sFileName.empty())
	{
		ReadFile("../webFile/uploadFile/index.html", sRespBoby);
		return;
	}
	
	WriteFileToDisk(GetBody(), sFileName);
	sRespBoby = "Update file sucess";
}