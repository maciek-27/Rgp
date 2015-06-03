#include "hello.h++"
#include <xercesc/dom/DOMCharacterData.hpp>
#include <boost/thread/detail/singleton.hpp>
#include<libxftp/commondomfeatures.h++>
#include "../commondomfeatures.h++"
#include "../xercesstrings.h++"
extern "C" {
#include "getpass.h"
}
#include <libxftp/client/auth.h++>
#include <libxftp/clientcontrol.h++>

using namespace xftp;
using namespace server;
using namespace xercesc;

#include <iostream>
using namespace std;

MsgHello::MsgHello (DOMElement *elem, size_t ID) : ServerMessage (ID, ProcessImplDefault(this))
{
	tlsAvailable=false;
	DOMNodeList* children = elem->getChildNodes ();
	const  XMLSize_t nodeCount = children->getLength ();
	for( XMLSize_t i = 0; i < nodeCount; ++i )
	{
		if (children->item (i)-> getNodeType ()!=DOMNode:: ELEMENT_NODE )
			continue;
		DOMElement * el = dynamic_cast<DOMElement *>(children->item (i));
		if (XMLString::equals ( (XMLCh[]){chLatin_s, chLatin_t, chLatin_a, chLatin_r, chLatin_t, chUnderscore, chLatin_t, chLatin_l, chLatin_s, chNull},
				el->getTagName ()))
			
		{
			tlsAvailable=true;
			if (el->getAttribute ( (XMLCh[]){chLatin_r, chLatin_e, chLatin_q, chLatin_u, chLatin_i, chLatin_r, chLatin_e, chLatin_d, chNull}))
				tlsRequired=true;
			else
				tlsRequired=false;
		}
		else  if (XMLString::equals ( (XMLCh[]){chLatin_s, chLatin_e, chLatin_r, chLatin_v, chLatin_e, chLatin_r, chUnderscore, chLatin_n, chLatin_a, chLatin_m, chLatin_e, chNull},
				el->getTagName ()))
		{	
			DOMCharacterData * __ServerName = 
			dynamic_cast<DOMCharacterData*>(el->getFirstChild ());
			if (__ServerName)
				serverName = toNative (__ServerName->getData ());
		}
		else if (XMLString::equals ( (XMLCh[]){chLatin_a, chLatin_u, chLatin_t, chLatin_h, chUnderscore, chLatin_m, chLatin_e, chLatin_t, chLatin_h, chLatin_o, chLatin_d, chLatin_s, chNull},
				el->getTagName ()))
		{
			DOMNodeList* children = el->getChildNodes ();
			const  XMLSize_t nodeCount = children->getLength ();
			for( XMLSize_t i = 0; i < nodeCount; ++i )
			{
				if (children->item (i)-> getNodeType ()!=DOMNode:: ELEMENT_NODE )
					continue;
				authMethods.push_back (toNative (reinterpret_cast<DOMCharacterData*>(children->item (i)->getFirstChild ())->getData ()));
			}
		}
		
	}
}

XMLCh * MsgHello::GetName ()
{
	static XMLCh name[] = {chLatin_h, chLatin_e, chLatin_l, chLatin_l,
	chLatin_o, chNull};
	return name;
}

const std::string& MsgHello::GetServerName ()
{
	return serverName;
}

void MsgHello::SetServerName(const std::string &_serverName)
{
	serverName = _serverName;
}

void MsgHello::AddAuthMethod(const std::string &method)
{
	authMethods.push_back(method);
}

const std::vector<std::string>& MsgHello::GetAuthMethods ()
{
	return authMethods;
}


void MsgHello::SetTLS (bool _tlsAvailable, bool _tlsRequired)
{
	tlsAvailable = _tlsAvailable;
	tlsRequired = _tlsRequired;
}

bool MsgHello::GetTLS ()
{
	return tlsAvailable;
}

bool MsgHello::GetTLSRequired ()
{
	return tlsRequired;
}

Message::DOMSerializedMessage MsgHello::Serialize (xercesc::DOMDocument & document)
{
	DOMSerializedMessage Result (InitializeSerialization(document));
	xml::CommonDOMFeatures::SetElementNamespace(*Result,
			xml::CommonDOMFeatures::GetServerNamespaceURI());

	if (tlsAvailable)
	{
		DOMElement * DEtlsAvailble
				= document.createElement ((XMLCh[]){chLatin_t, chLatin_l, chLatin_s, chUnderscore, chLatin_a, chLatin_v, chLatin_a, chLatin_i, chLatin_l, chLatin_b, chLatin_l, chLatin_e, chNull});
		if (tlsRequired)
			DEtlsAvailble -> setAttribute ((XMLCh[]){chLatin_r, chLatin_e, chLatin_q, chLatin_u, chLatin_i, chLatin_r, chLatin_e, chLatin_d, chNull},
					(XMLCh[]){chLatin_t, chLatin_r, chLatin_u, chLatin_e, chNull});
		Result->appendChild (DEtlsAvailble);
	}
	
	DOMElement * DEserverName =
			document.createElement ((XMLCh[]){chLatin_s, chLatin_e, chLatin_r, chLatin_v, chLatin_e, chLatin_r, chUnderscore, chLatin_n, chLatin_a, chLatin_m, chLatin_e, chNull});
	DEserverName->appendChild(
			document.createTextNode(fromNative(serverName).c_str()));
	Result->appendChild (DEserverName);
	
	
	DOMElement * DEauthMethods =
			document.createElement ((XMLCh[]){chLatin_a, chLatin_u, chLatin_t, chLatin_h, chUnderscore, chLatin_m, chLatin_e, chLatin_t, chLatin_h, chLatin_o, chLatin_d, chLatin_s, chNull});
	for (std::vector<std::string>::const_iterator i = 
		 authMethods.begin() ;  i!=authMethods.end() ; ++i)
	{
		DOMElement * DEauthMethod =
			document.createElement ((XMLCh[]){chLatin_a, chLatin_u, chLatin_t, chLatin_h, chUnderscore, chLatin_m, chLatin_e, chLatin_t, chLatin_h, chLatin_o, chLatin_d, chNull});
		DEauthMethod->appendChild(
			document.createTextNode(fromNative(*i).c_str()));
		DEauthMethods->appendChild (DEauthMethod);
		
	}
	Result->appendChild (DEauthMethods);
	
	return Result;
};

void MsgHello::DefaultProcessImpl(ClientControl& control)
{
	std::cout << "Username: ";
	std::string userName;
	std::cin >> userName;
	
	char password[25];
	getpass_r("Password: ", password, 25);

	client::MsgAuth *auth = new client::MsgAuth;
	std::tr1::shared_ptr<Message> l(auth);

	auth->SetUserName(userName);
	auth->SetAuthData(password);
	auth->SetAuthMethod("PLAIN");

	control.xmlStream.PostMessage(l);
};
