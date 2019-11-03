#pragma once

#ifndef HTTPMESSAGE_H
#define HTTPMESSAGE_H

#include <string>
#include <map>
#include <algorithm>
#include "simplebuffer.h"
#include "base/log.h"

class HttpMessage
{
public:
	typedef std::map<std::string, std::string> HeadMapType;
	
	enum ParseRequestType
	{
		ParseRequestLine = 1,
		ParseRequestHead,
		ParseRequestBody
	};
	
	typedef enum ParseState
	{
		Again = 100,	// more data need
		Error,		// parse data gets error
		Done		// parse a complete message
	} ParseState;
	
	enum RespondType
	{
		Correct = 200,
		BadReq = 400,   // bad request
		NotFound = 404, // not found
		SerError = 500, // internal server error
		MeNotIm = 501	// method not implement
	};
	
	
public:
	HttpMessage();
	const std::string & GetMethod() const;
	const std::string & GetUrl() const;
	const std::string & GetHttpVersion() const;
	const std::string & GetBody() const;
	const HeadMapType & GetHeadContent() const;
	RespondType GetRespondCode() const;
	const std::string & GetRespondMsg() const;
	ParseState Parse(SimpleBuffer & inBuffer);
	void Reset();
	void BuildErrorRespond(int errCode, const std::string & errMsg, SimpleBuffer & outBuffer);
	
	// Fix Me Content-Type json/text should change.
	void BuildCgiRespond(int respCode, const std::string & sRespMsg, const std::string & sRespBody, SimpleBuffer & outBuffer);
	
private:
	ParseRequestType m_parseStage;
	RespondType m_respondCode;
	std::string m_respondMsg;
	std::string m_method;
	std::string m_url;
	std::string m_httpVersion;
	HeadMapType m_headContent;
	std::string m_bodyStr;
};

#endif