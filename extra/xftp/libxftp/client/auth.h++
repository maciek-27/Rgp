#ifndef _CLIENT_AUTH_H_
#define _CLIENT_AUTH_H_

#include "message.h++"

namespace xftp
{
namespace client
{

class MsgAuth : public ClientMessage
{
private:
	std::string authMethod;
	std::string userName;
	std::string authData;
public:
	//! parse DOM tree
	MsgAuth(xercesc::DOMElement *elem, size_t ID);

	//! create empty message object
	MsgAuth();

	//! always return "auth"
	XMLCh * GetName();

	//! XML-ish param auth_method
	const std::string& GetAuthMethod();
	//! XML-ish param auth_method
	void SetAuthMethod(const std::string& _authMethod);
	//! XML-ish param username (no underscore)
	const std::string& GetUserName();
	//! XML-ish param username (no underscore)
	void SetUserName(const std::string& _userName);
	//! XML-ish param auth_data
	const std::string& GetAuthData();
	//! XML-ish param auth_data
	void SetAuthData(const std::string& _authData);

	
	Message::DOMSerializedMessage Serialize(xercesc::DOMDocument & document);
	//! current implementation accepts username "user" with auth_data "os2008"
	//! (otherwise MsgAuthRefuse is sent)
	void DefaultProcessImpl(ServerControl& control);
};
}}

#endif // _CLIENT_AUTH_H_
