#include "cgidispatch.h"

BaseFactory::Url2CgiFactoryMapType BaseFactory::m_cgiInstanceFactory;

template<typename CGIINSTANCE>
void BaseFactory::AddCgiInstanceFactory(const std::string sUrl)
{
	std::shared_ptr<BaseFactory> ptCgiInstanceFactory(new InstanceFactory<CGIINSTANCE>);
	m_cgiInstanceFactory.insert(std::make_pair(sUrl, ptCgiInstanceFactory));
}

BaseFactory::Url2CgiFactoryMapType & BaseFactory::GetCgiFactoryMap()
{
	return m_cgiInstanceFactory;
}

void BaseFactory::Init()
{
	AddCgiInstanceFactory<ExampleCgi>("/cgi-bin/example");
}