#ifndef BASECGI_H
#define BASECGI_H

#include <memory>
#include <string>
#include <stdint.h>
#include "httpmessage.h"

class BaseCgi
{
public:
	typedef HttpMessage::HeadMapType HeadMapType;
	typedef HeadMapType::const_iterator HeadMapTypeConstIter;
	
	BaseCgi();
	virtual ~BaseCgi();
	
	BaseCgi(const BaseCgi &) = delete;
	BaseCgi & operator= (const BaseCgi &) = delete;
	
	void Process(const HttpMessage * ptHttpMessage, int & respondCode, std::string & sRespMsg, std::string & sContentType, std::string & sRespBoby);
	virtual void ProcessImp(int & respondCode, std::string & sRespMsg, std::string & sContentType, std::string & sRespBoby) = 0;
	
	const std::string & GetUrl();
	const std::string & GetBody();
	int32_t GetInt32withDefault(const char * keyName, int32_t defaultValue = 0);
	int64_t GetInt64withDefault(const char * keyName, int64_t defaultValue = 0);
	uint32_t GetUint32withDefault(const char * keyName, uint32_t defaultValue = 0);
	uint64_t GetUint64withDefault(const char * keyName, uint64_t defaultValue = 0);
	
	// return the reference, or return a node string which is empty
	const std::string & GetStringValue(const char * keyName);
	
	void SetRespondCode(int32_t respondCode);
	void SetRespondMsg(const char * str, size_t strSize);
	void SetRespondBody(const char * str, size_t strSize);
	
	/* this one should install google protobuf 3 first
	https://developers.google.com/protocol-buffers/docs/reference/cpp/google.protobuf.util.json_util
	void SetRespondBody(const google::protobuf::Message & obj)
	{
		google::protobuf::util::JsonPrintOptions jsonPrintOption;
		jsonPrintOption.always_print_primitive_fields=true;
		google::protobuf::util::MessageToJsonString(obj, &m_sRespondBody,jsonPrintOption);
	}
	*/
	
private:
	static const std::string m_emptyString;
	const HttpMessage * m_ptHttpMessage;
};

#endif