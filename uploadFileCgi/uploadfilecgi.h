#pragma once

#ifndef UPLOADFILECGI_H
#define UPLOADFILECGI_H

#include "../basecgi.h"

class UploadFileCgi : public BaseCgi
{
	virtual void ProcessImp(int & respondCode, std::string sRespMsg, std::string & sContentType, std::string & sRespBoby);
};

#endif