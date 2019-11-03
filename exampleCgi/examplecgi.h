#ifndef EXAMPLECGI_H
#define EXAMPLECGI_H

#include "../basecgi.h"

class ExampleCgi : public BaseCgi
{
	virtual void ProcessImp(int & respondCode, std::string sRespMsg, std::string & sRespBoby);
};

#endif