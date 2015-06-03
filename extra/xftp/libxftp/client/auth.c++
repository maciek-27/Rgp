#include "auth.h++"
#include <libxftp/commondomfeatures.h++>
#include <libxftp/server/authrefuse.h++>
#include <libxftp/server/serverinfo.h++>
#include <libxftp/servercontrol.h++>

#include "../callbackfunction.h++"
using namespace xftp;
using namespace client;
using namespace xercesc;




MsgAuth::MsgAuth(DOMElement *elem, size_t ID) : ClientMessage (ID, ProcessImplDefault(this))
{
	DOMNodeList* children = elem->getChildNodes();
	const  XMLSize_t nodeCount = children->getLength();
	for( XMLSize_t i = 0; i < nodeCount; ++i )
	{		
		DOMElement * el = dynamic_cast<DOMElement *>(children->item(i));
		if (children->item (i)-> getNodeType ()!=DOMNode:: ELEMENT_NODE )
			continue;
		if(XMLString::equals((XMLCh[]){chLatin_a, chLatin_u, chLatin_t, chLatin_h, chUnderscore, chLatin_m, chLatin_e, chLatin_t, chLatin_h, chLatin_o, chLatin_d, chNull}, 
							 el->getTagName()))
		{
			if(el->getFirstChild()) {
				authMethod.assign(
					toNative(dynamic_cast<DOMCharacterData*>
							 (el->getFirstChild())->getData()));
			}
		}
		else if(XMLString::equals((XMLCh[]){chLatin_u, chLatin_s, chLatin_e, chLatin_r, chLatin_n, chLatin_a, chLatin_m, chLatin_e, chNull}, 
							 el->getTagName()))
		{
			if(el->getFirstChild()) {
				userName.assign(
					toNative(dynamic_cast<DOMCharacterData*>
							 (el->getFirstChild())->getData()));
			}
		}
		else if(XMLString::equals((XMLCh[]){chLatin_a, chLatin_u, chLatin_t, chLatin_h, chUnderscore, chLatin_d, chLatin_a, chLatin_t, chLatin_a, chNull}, 
							 el->getTagName()))
		{
			if(el->getFirstChild()) {
				authData.assign(
					toNative(dynamic_cast<DOMCharacterData*>
							 (el->getFirstChild())->getData()));
			}
		}
	}
}

MsgAuth::MsgAuth() : ClientMessage (-1, ProcessImplDefault(this))
{
	;
}

XMLCh * MsgAuth::GetName()
{
	static XMLCh name[] = {chLatin_a, chLatin_u, chLatin_t, chLatin_h,
						   chNull};
	return name;
}

const std::string& MsgAuth::GetAuthMethod()
{
	return authMethod;
}

void MsgAuth::SetAuthMethod(const std::string& _authMethod)
{
	authMethod = _authMethod;
}

const std::string& MsgAuth::GetUserName()
{
	return userName;
}

void MsgAuth::SetUserName(const std::string& _userName)
{
	userName = _userName;
}

const std::string& MsgAuth::GetAuthData()
{
	return authData;
}

void MsgAuth::SetAuthData(const std::string& _authData)
{
	authData = _authData;
}

Message::DOMSerializedMessage MsgAuth::Serialize(xercesc::DOMDocument & document)
{
	
	DOMSerializedMessage Result (InitializeSerialization(document));
	xml::CommonDOMFeatures::SetElementNamespace(*Result,
			xml::CommonDOMFeatures::GetClientNamespaceURI());
	
	DOMElement * DEauthMethod =
		document.createElement ((XMLCh[]){chLatin_a, chLatin_u, chLatin_t, chLatin_h, chUnderscore, chLatin_m, chLatin_e, chLatin_t, chLatin_h, chLatin_o, chLatin_d, chNull});
	DEauthMethod->appendChild(
		document.createTextNode(fromNative(authMethod).c_str()));	
	Result->appendChild (DEauthMethod);

	if(userName.length()) {
		DOMElement * DEuserName =
			document.createElement ((XMLCh[]){chLatin_u, chLatin_s, chLatin_e, chLatin_r, chLatin_n, chLatin_a, chLatin_m, chLatin_e, chNull});
		DEuserName->appendChild(
			document.createTextNode(fromNative(userName).c_str()));	
		Result->appendChild (DEuserName);
	}

	if(authData.length()) {
		DOMElement * DEauthData =
			document.createElement ((XMLCh[]){chLatin_a, chLatin_u, chLatin_t, chLatin_h, chUnderscore, chLatin_d, chLatin_a, chLatin_t, chLatin_a, chNull});
		DEauthData->appendChild(
			document.createTextNode(fromNative(authData).c_str()));
		Result->appendChild (DEauthData);
	}

	return Result;
}

void MsgAuth::DefaultProcessImpl(ServerControl& control)
{
	if(GetUserName() == "user" && GetAuthData() == "os2008") {
		server::MsgServerInfo *sinfo = new server::MsgServerInfo;
		std::tr1::shared_ptr<Message> l(sinfo);

		control.xmlStream.PostMessage(l);
		control.loggedIn = true;
	}
	else {
		server::MsgAuthRefuse *aref = new server::MsgAuthRefuse;
		std::tr1::shared_ptr<Message> l(aref);
		aref->SetReason("Bad username or password!");
		control.xmlStream.PostMessage(l);
	}

}
