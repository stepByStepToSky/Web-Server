#include "httpmessage.h"

HttpMessage::ContentTypeMapType HttpMessage::m_contentTypeMap;

void HttpMessage::InitContentTypeMap()
{
	m_contentTypeMap.insert({".html", "text/html"});
	m_contentTypeMap.insert({".avi", "video/x-msvideo"});
	m_contentTypeMap.insert({".bmp", "image/bmp"});
	m_contentTypeMap.insert({".c", "text/plain"});
	m_contentTypeMap.insert({".doc", "application/msword"});
	m_contentTypeMap.insert({".gif", "image/gif"});
	m_contentTypeMap.insert({".gz", "application/x-gzip"});
	m_contentTypeMap.insert({".htm", "text/html"});
	m_contentTypeMap.insert({".ico", "image/x-icon"});
	m_contentTypeMap.insert({".jpg", "image/jpeg"});
	m_contentTypeMap.insert({".png", "image/png"});
	m_contentTypeMap.insert({".txt", "text/plain"});
	m_contentTypeMap.insert({".js", "application/javascript"});
	m_contentTypeMap.insert({".css", "text/css"});
	m_contentTypeMap.insert({".mp3", "audio/mp3"});
	m_contentTypeMap.insert({"default", "text/html"});
}

HttpMessage::ContentTypeMapType & HttpMessage::GetContentTypeMap()
{
	return m_contentTypeMap;
}

HttpMessage::HttpMessage() : m_parseStage(ParseRequestLine), m_parseMultiFormStage(Boundary)
{
	
}

const std::string & HttpMessage::GetMethod() const
{
	return m_method;
}

const std::string & HttpMessage::GetUrl() const
{
	return m_url;
}

const std::string & HttpMessage::GetHttpVersion() const
{
	return m_httpVersion;
}

const std::string & HttpMessage::GetBody() const
{
	return m_bodyStr;
}

const HttpMessage::HeadMapType & HttpMessage::GetHeadContent() const
{
	return m_headContent;
}

HttpMessage::HeadMapType & HttpMessage::GetHeadContent()
{
	return m_headContent;
}

HttpMessage::RespondType HttpMessage::GetRespondCode() const
{
	return m_respondCode;
}

const std::string & HttpMessage::GetRespondMsg() const
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
			while(-1 != paramPos && paramPos < pos)
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
				m_respondMsg = "Bad request";
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
				HeadMapType::iterator iter = m_headContent.find("Content-Length");
				if (m_headContent.end() == iter)
				{
					ERRLOG("%s %s %d, post method has no Content-Length information", __FILE__, __func__, __LINE__);
					m_respondCode = BadReq;
					m_respondMsg = "Bad request";
					return Error;
				}
				m_realDataContentLength = static_cast<size_t>(std::max(0, std::stoi(iter->second)));
				break;
			}
			else
			{
				DEBUGLOG("%s %s %d, parse request done", __FILE__, __func__, __LINE__);
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
		if (m_realDataContentLength > inBuffer.BufferSize())	// body content is imcomplete
		{
			return Again;
		}
		
		const std::string & sContentType = m_headContent["Content-Type"];
		static const char multiPartFormDataArray[] = "multipart/form-data";
		if (!(sizeof(multiPartFormDataArray) - 1 <= sContentType.size() && 0 == sContentType.compare(0, sizeof(multiPartFormDataArray) - 1, multiPartFormDataArray, sizeof(multiPartFormDataArray) - 1)))
		{
			DEBUGLOG("%s %s %d, it is not multipart/form-data request", __FILE__, __func__, __LINE__);
			m_bodyStr.assign(inBuffer.Buffer(), m_realDataContentLength);
			inBuffer.ReadFromBuffer(m_realDataContentLength);
		}
		else
		{
			DEBUGLOG("%s %s %d, it is multipart/form-data request", __FILE__, __func__, __LINE__);
			if (Boundary == m_parseMultiFormStage)
			{
				ssize_t pos = inBuffer.Find("\r\n");
				if (-1 == pos)
				{
					return Again;
				}
				std::string sBoundary(inBuffer.Buffer(), pos);
				sBoundary.append("--\r\n");
				m_headContent.insert(std::make_pair("boundary", sBoundary));
				inBuffer.ReadFromBuffer(pos + 2);
				m_realDataContentLength -= (pos + 2);
				DEBUGLOG("%s %s %d, multipart/form-data request parse boundry success, stage=%d, pos=%d, sBoundary=%s", __FILE__, __func__, __LINE__, m_parseMultiFormStage, pos, sBoundary.c_str());
				m_parseMultiFormStage = ContentDisposition;
			}
			
			if (ContentDisposition == m_parseMultiFormStage)
			{
				ssize_t pos = inBuffer.Find("\r\n");
				if (0 < pos)
				{
					inBuffer.ReadFromBuffer(pos + 2);
					m_realDataContentLength -= (pos + 2);
					pos = inBuffer.Find("\r\n");
					DEBUGLOG("%s %s %d, multipart/form-data request parse content disposition 1 success, stage=%d, pos=%d", __FILE__, __func__, __LINE__, m_parseMultiFormStage, pos);
				}
				
				if (-1 == pos)
				{
					return Again;
				}
				else if (0 < pos)
				{
					inBuffer.ReadFromBuffer(pos + 2);
					m_realDataContentLength -= (pos + 2);
					m_parseMultiFormStage = DataContentTypeEnd;
				}
				
				DEBUGLOG("%s %s %d, multipart/form-data request parse content disposition 2 success, stage=%d, pos=%d", __FILE__, __func__, __LINE__, m_parseMultiFormStage, pos);
			}
			DEBUGLOG("%s %s %d, multipart/form-data request parse content disposition success, stage=%d", __FILE__, __func__, __LINE__, m_parseMultiFormStage);
			
			if (DataContentTypeEnd == m_parseMultiFormStage)
			{
				ssize_t pos = inBuffer.Find("\r\n");	//	"\r\n"
				if (-1 == pos)
				{
					return Again;
				}
				inBuffer.ReadFromBuffer(pos + 2);
				m_realDataContentLength -= (pos + 2);
				m_parseMultiFormStage = RealBodyData;
			}
			DEBUGLOG("%s %s %d, multipart/form-data request parse content type success, stage=%d", __FILE__, __func__, __LINE__, m_parseMultiFormStage);
			
			if (RealBodyData == m_parseMultiFormStage)
			{	
				// support multipart/form-data
				bool isBoundaryEnd = inBuffer.EndWithBackEndLength("--\r\n", m_realDataContentLength);
				if (isBoundaryEnd)
				{
					const std::string & sBoundary = m_headContent["boundary"];
					isBoundaryEnd = inBuffer.EndWithBackEndLength(sBoundary.c_str(), m_realDataContentLength);
					if (isBoundaryEnd)
					{
						DEBUGLOG("%s %s %d, multipart/form-data request parse success, stage=%d, sBoundary=%s", __FILE__, __func__, __LINE__, m_parseMultiFormStage, sBoundary.c_str());
						m_parseMultiFormStage = RealDone;
						//inBuffer.DropEnd(sBoundary.size() + 2);	// drop \r\n and the final boundary
						m_bodyStr.assign(inBuffer.Buffer(), m_realDataContentLength - sBoundary.size() - 2);
						inBuffer.ReadFromBuffer(m_realDataContentLength);
						return Done;
					}
				}
				
				if (inBuffer.BufferSize() > std::stoi(m_headContent["Content-Length"]))
				{
					ERRLOG("%s %s %d, post method has no Content-Length information", __FILE__, __func__, __LINE__);
					m_respondCode = BadReq;
					m_respondMsg = "Bad request";
					return Error;
				}
				return Again;
			}
			DEBUGLOG("%s %s %d, multipart/form-data request parse real body success, stage=%d", __FILE__, __func__, __LINE__, m_parseMultiFormStage);
		}
	}
	DEBUGLOG("%s %s %d, parse request done", __FILE__, __func__, __LINE__);
	return Done;
}

void HttpMessage::Reset()
{
	m_parseStage = ParseRequestLine;
	m_parseMultiFormStage = Boundary;
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

void HttpMessage::BuildCgiRespond(int respCode, const std::string & sRespMsg, const std::string & sContentType, const std::string & sRespBody, SimpleBuffer & outBuffer)
{
	outBuffer.Append("HTTP/1.1 %d %s\r\n", respCode, sRespMsg.c_str());
	outBuffer.Append("Content-Type: %s\r\n", sContentType.c_str());
	outBuffer.Append("Connection: keep-alive\r\n");
	outBuffer.Append("Server: Lufan's Web Server\r\n");
	
	if (0 < sRespBody.size())
	{
		outBuffer.Append("Content-Length: %d\r\n", static_cast<int>(sRespBody.size()));
		outBuffer.Append("\r\n");
		outBuffer.WriteToBuffer(sRespBody.data(), sRespBody.size());
	}
}