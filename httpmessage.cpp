#include "httpmessage.h"

HttpMessage::HttpMessage() : m_parseStage(ParseRequestLine)
{
	
}

const std::string & HttpMessage::GetMethod()
{
	return m_method;
}

const std::string & HttpMessage::GetUrl()
{
	return m_url;
}

const std::string & HttpMessage::GetHttpVersion()
{
	return m_httpVersion;
}

const std::string & HttpMessage::GetBody()
{
	return m_bodyStr;
}

const HttpMessage::HeadMapType & HttpMessage::GetHeadContent()
{
	return m_headContent;
}

HttpMessage::RespondType HttpMessage::GetRespondCode()
{
	return m_respondCode;
}

const std::string & HttpMessage::GetRespondMsg()
{
	return m_respondMsg;
}

HttpMessage::ParseState HttpMessage::Parse(SimpleBuffer & inBuffer)
{
	if (ParseRequestLine == m_parseStage)
	{
		ssize_t pos = inBuffer.Find("\r\n");
		if (-1 == pos)
		{
			return Again;
		}
		
		pos = inBuffer.Find(' ');
		if (-1 == pos)
		{
			ERRLOG("%s %s %d, %s is invalid", __FILE__, __func__, __LINE__, inBuffer.Buffer());
			m_respondCode = BadReq;
			m_respondMsg = "Bad request";
			return Error;
		}
		m_method.assign(inBuffer.Buffer(), pos);
		inBuffer.ReadFromBuffer(1 + pos);
		std::transform(m_method.begin(), m_method.end(), m_method.begin(),
              [](unsigned char c) { return std::toupper(c); });
		if (("GET" != m_method) && ("POST" != m_method))
		{
			ERRLOG("%s %s %d, %s is not implemented", __FILE__, __func__, __LINE__, m_method.c_str());
			m_respondCode = MeNotIm;
			m_respondMsg = "Method is not implemented";
			return Error;
		}
		
		pos = inBuffer.Find(' ');
		if (-1 == pos)
		{
			ERRLOG("%s %s %d, %s is invalid", __FILE__, __func__, __LINE__, inBuffer.Buffer());
			m_respondCode = BadReq;
			m_respondMsg = "Bad request";
			return Error;
		}
		ssize_t paramPos = inBuffer.Find('?');
		if (-1 != paramPos && paramPos < pos)	// for example. /login?wd=hhh&name=xiaofan
		{
			m_url.assign(inBuffer.Buffer(), paramPos);
			inBuffer.ReadFromBuffer(paramPos + 1);
			
			paramPos = inBuffer.Find('&');
			while(-1 != paramPos)
			{
				ssize_t keyEndPos = inBuffer.Find('=');
				if (-1 == keyEndPos || keyEndPos > paramPos)
				{
					ERRLOG("%s %s %d, %s is invalid", __FILE__, __func__, __LINE__, inBuffer.Buffer());
					m_respondCode = BadReq;
					m_respondMsg = "Bad request";
					return Error;
				}
				std::string sKey(inBuffer.Buffer(), keyEndPos);
				std::string sValue(inBuffer.Buffer() + keyEndPos + 1, paramPos - keyEndPos - 1);
				m_headContent.insert(make_pair(sKey, sValue));
				inBuffer.ReadFromBuffer(paramPos + 1);
				paramPos = inBuffer.Find('&');
			}
			
			pos = inBuffer.Find(' ');
			if (-1 == pos)
			{
				ERRLOG("%s %s %d, %s is invalid", __FILE__, __func__, __LINE__, inBuffer.Buffer());
				m_respondCode = BadReq;
				m_respondMsg = "Bad request";
				return Error;
			}
			ssize_t keyEndPos = inBuffer.Find('=');
			if (-1 == keyEndPos || keyEndPos > pos)
			{
				ERRLOG("%s %s %d, %s is invalid", __FILE__, __func__, __LINE__, inBuffer.Buffer());
				m_respondCode = BadReq;
				return Error;
			}
			std::string sKey(inBuffer.Buffer(), keyEndPos);
			std::string sValue(inBuffer.Buffer() + keyEndPos + 1, pos - keyEndPos - 1);
			m_headContent.insert(std::make_pair(sKey, sValue));
			inBuffer.ReadFromBuffer(pos + 1);
		}
		else
		{
			m_url.assign(inBuffer.Buffer(), pos);
			inBuffer.ReadFromBuffer(1 + pos);
		}
		
		pos = inBuffer.Find("\r\n");
		m_httpVersion.assign(inBuffer.Buffer(), pos);
		inBuffer.ReadFromBuffer(pos + 2);	// skip the char '\r' and '\n'
		m_parseStage = ParseRequestHead;
	}
	
	for (; ParseRequestHead == m_parseStage; )
	{
		ssize_t pos = inBuffer.Find("\r\n");
		if (-1 == pos)
		{
			return Again;
		}
		
		// check if the head has parsed done
		if (0 == pos)
		{
			inBuffer.ReadFromBuffer(pos + 2);
			// post must have body, get does not have any body
			if ("POST" == m_method)
			{
				m_parseStage = ParseRequestBody;
				break;
			}
			else
			{
				return Done;
			}
		}
		
		ssize_t keyEndPos = inBuffer.Find(':');
		if (-1 == keyEndPos || keyEndPos > pos)
		{
			ERRLOG("%s %s %d, %s is invalid", __FILE__, __func__, __LINE__, inBuffer.Buffer());
			m_respondCode = BadReq;
			m_respondMsg = "Bad request";
			return Error;
		}
		
		std::string sKey(inBuffer.Buffer(), keyEndPos);
		std::string sValue(inBuffer.Buffer() + keyEndPos + 2, pos - keyEndPos - 2); 	// +2 is to skip the char ':' and ' '
		m_headContent.insert(std::make_pair(sKey, sValue));
		inBuffer.ReadFromBuffer(pos + 2);
	}
	
	
	if (ParseRequestBody == m_parseStage)
	{
		HeadMapType::iterator iter = m_headContent.find("Content-Length");
		if (m_headContent.end() == iter)
		{
			ERRLOG("%s %s %d, post method has no Content-Length information", __FILE__, __func__, __LINE__);
			m_respondCode = BadReq;
			m_respondMsg = "Bad request";
			return Error;
		}
		
		size_t contentLength = static_cast<size_t>(std::max(0, std::stoi(iter->second)));
		if (contentLength > inBuffer.BufferSize())	// body content is imcomplete
		{
			return Again;
		}
		
		m_bodyStr.assign(inBuffer.Buffer(), contentLength);
	}
	
	return Done;
}

void HttpMessage::Reset()
{
	m_parseStage = ParseRequestLine;
	HeadMapType tempHeadMap;
	m_headContent.swap(tempHeadMap);
}

void HttpMessage::BuildErrorRespond(int errCode, const std::string & errMsg, SimpleBuffer & outBuffer)
{
	std::string sBodyContent;
	sBodyContent.append("<html><title>Something wrong with the request</title>");
	sBodyContent.append("<body bgcolor=\"ffffff\">");
	sBodyContent.append(std::to_string(errCode).c_str());
	sBodyContent.append(" ");
	sBodyContent.append(errMsg.c_str());
	sBodyContent.append("<hr><em> Lufan's Web Server</em>\n</body></html>");
	
	outBuffer.Append("HTTP/1.1 %d %s\r\n", errCode, errMsg.c_str());
	outBuffer.Append("Content-Type: text/html\r\n");
	outBuffer.Append("Connection: Close\r\n");
	outBuffer.Append("Content-Length: %d\r\n", static_cast<int>(sBodyContent.size()));
	outBuffer.Append("Server: Lufan's Web Server\r\n");
	outBuffer.Append("\r\n");
	outBuffer.Append("%s", sBodyContent.c_str());
}

