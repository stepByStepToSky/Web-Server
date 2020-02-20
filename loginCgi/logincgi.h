#ifndef LOGINCGI_H
#define LOGINCGI_H

#include "../basecgi.h"

class LoginCgi : public BaseCgi
{
	virtual void ProcessImp(int & respondCode, std::string & sRespMsg, std::string & sContentType, std::string & sRespBoby);
};

#endif