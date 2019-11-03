#include "basecgi.h"

const std::string BaseCgi::m_emptyString;

BaseCgi::BaseCgi()
{
	
}

BaseCgi::~BaseCgi()
{
	
}

void BaseCgi::Process(const HttpMessage * ptHttpMessage, int & respondCode, std::string sRespMsg, std::string & sRespBoby)
{
	m_ptHttpMessage = ptHttpMessage;
	respondCode = 200;
	sRespMsg.assign("OK", 2);
	ProcessImp(respondCode, sRespMsg, sRespBoby);
}

const std::string & BaseCgi::GetUrl()
{
	return m_ptHttpMessage->GetUrl();
}

const std::string & BaseCgi::GetBody()
{
	return m_ptHttpMessage->GetBody();
}

int32_t BaseCgi::GetInt32withDefault(const char * keyName, int32_t defaultValue /* = 0 */)
{
	const HeadMapType & headContentMap = m_ptHttpMessage->GetHeadContent();
	HeadMapTypeConstIter iter = headContentMap.find(keyName);
	if (headContentMap.end() == iter)
	{
		return defaultValue;
	}
	
	return (std::stoi(iter->second));
}

int64_t BaseCgi::GetInt64withDefault(const char * keyName, int64_t defaultValue /* = 0 */)
{
	const HeadMapType & headContentMap = m_ptHttpMessage->GetHeadContent();
	HeadMapTypeConstIter iter = headContentMap.find(keyName);
	if (headContentMap.end() == iter)
	{
		return defaultValue;
	}
	
	return (static_cast<int64_t>(std::stoll(iter->second)));
}

uint32_t BaseCgi::GetUint32withDefault(const char * keyName, uint32_t defaultValue /* = 0 */)
{
	const HeadMapType & headContentMap = m_ptHttpMessage->GetHeadContent();
	HeadMapTypeConstIter iter = headContentMap.find(keyName);
	if (headContentMap.end() == iter)
	{
		return defaultValue;
	}
	
	return (static_cast<uint32_t>(std::stoul(iter->second)));
}

uint64_t BaseCgi::GetUint64withDefault(const char * keyName, uint64_t defaultValue /* = 0 */)
{
	const HeadMapType & headContentMap = m_ptHttpMessage->GetHeadContent();
	HeadMapTypeConstIter iter = headContentMap.find(keyName);
	if (headContentMap.end() == iter)
	{
		return defaultValue;
	}
	
	return (static_cast<uint64_t>(std::stoull(iter->second)));
}

const std::string & BaseCgi::GetStringValue(const char * keyName)
{
	const HeadMapType & headContentMap = m_ptHttpMessage->GetHeadContent();
	HeadMapTypeConstIter iter = headContentMap.find(keyName);
	if (headContentMap.end() == iter)
	{
		return m_emptyString;
	}
	
	return (iter->second);
}