#pragma once

#ifndef CGIDISPATCH_H
#define CGIDISPATCH_H

#include <memory>
#include <map>
#include <string>
#include "includecgiheader.h"

class BaseFactory
{
public:
	typedef std::map<std::string, std::shared_ptr<BaseFactory>> Url2CgiFactoryMapType;

	virtual std::unique_ptr<BaseCgi> GetNewInstance() = 0;
	
	template<typename CGIINSTANCE>
	static void AddCgiInstanceFactory(const std::string sUrl);
	
	static Url2CgiFactoryMapType & GetCgiFactoryMap();
	static void Init();

private:
	 static Url2CgiFactoryMapType m_cgiInstanceFactory;
};

template<typename CGIINSTANCE>
class InstanceFactory : public BaseFactory
{
	virtual std::unique_ptr<BaseCgi> GetNewInstance()
	{
		return std::unique_ptr<CGIINSTANCE>(new CGIINSTANCE);
	}
};

#endif