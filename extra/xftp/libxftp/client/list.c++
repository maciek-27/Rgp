#include "list.h++"
#include "../server/list.h++"
#include <libxftp/xmlstream.h++>
#include <libxftp/commondomfeatures.h++>
#include <libxftp/xftpcommons.h++>
#include <libxftp/allmessages.h++>
#include <libxftp/servercontrol.h++>
#include <errno.h>

using namespace xftp;
using namespace client;
using namespace xercesc;
MsgList::MsgList() : ClientMessage(-1, ProcessImplDefault(this)){recurse=0;}

MsgList::MsgList(DOMElement *elem, size_t ID) : ClientMessage (ID, ProcessImplDefault(this))
{	
	DOMNodeList* children = elem->getChildNodes();
	const  XMLSize_t nodeCount = children->getLength();
	for(XMLSize_t i = 0; i < nodeCount; ++i)
	{
		DOMElement * el = dynamic_cast<DOMElement *>(children->item(i));
		if (not el)
			continue;
		if(XMLString::equals((XMLCh[]){chLatin_d, chLatin_i, chLatin_r, chNull}, 
							 el->getTagName()))
		{
			dir.reset(new std::string(
				toNative(dynamic_cast<DOMCharacterData *>
						 (el->getFirstChild())->getData())));
		}
		else if(XMLString::equals((XMLCh[]){chLatin_r, chLatin_e, chLatin_c, chLatin_u, chLatin_r, chLatin_s, chLatin_e, chNull}, 
								  el->getTagName()))
		{
			std::string s = toNative(dynamic_cast<DOMCharacterData *>
									 (el->getFirstChild())->getData());
			if(s == "\342\210\236")
				recurse = -1;
			else
				recurse = atoi(s.c_str());
		}
		else if(XMLString::equals((XMLCh[]){chLatin_m, chLatin_e, chLatin_t, chLatin_a, chLatin_d, chLatin_a, chLatin_t, chLatin_a, chUnderscore, chLatin_r, chLatin_e, chLatin_q, chLatin_u, chLatin_e, chLatin_s, chLatin_t, chNull}, 
								  el->getTagName()))
		{
			metaDataRequest.flags.branch = false;
			metaDataRequest.flags.data = false;
			metaDataRequest.filename = false;
			metaDataRequest.filesize = false;

			DOMNodeList* children = elem->getChildNodes();
			const  XMLSize_t nodeCount = children->getLength();
			for( XMLSize_t i = 0; i < nodeCount; ++i )
			{					
				DOMElement * el = dynamic_cast<DOMElement *>(children->item(i));
				if(XMLString::equals((XMLCh[]){chLatin_f, chLatin_l, chLatin_a, chLatin_g, chLatin_s, chNull}, 
									 el->getTagName())) {
					DOMNodeList* children = elem->getChildNodes();
					const  XMLSize_t nodeCount = children->getLength();
					for( XMLSize_t i = 0; i < nodeCount; ++i )
					{					
						DOMElement * el = dynamic_cast<DOMElement *>(children->item(i));
						if(XMLString::equals((XMLCh[]){chLatin_b, chLatin_r, chLatin_a, chLatin_n, chLatin_c, chLatin_h, chNull}, 
											 el->getTagName())) {
							metaDataRequest.flags.branch = true;
						}
						else if(XMLString::equals((XMLCh[]){chLatin_d, chLatin_a, chLatin_t, chLatin_a, chNull}, 
												  el->getTagName())) {
							metaDataRequest.flags.data = true;
						}						
					}					
				}
				else if(XMLString::equals((XMLCh[]){chLatin_f, chLatin_i, chLatin_l, chLatin_e, chLatin_n, chLatin_a, chLatin_m, chLatin_e, chNull}, 
										  el->getTagName())) {
					metaDataRequest.filename = true;
				}
				else if(XMLString::equals((XMLCh[]){chLatin_f, chLatin_i, chLatin_l, chLatin_e, chLatin_s, chLatin_i, chLatin_z, chLatin_e, chNull},
										  el->getTagName())) {
					metaDataRequest.filesize = true;
				}
			}
		}
		else if(XMLString::equals((XMLCh[]){chLatin_f, chLatin_r, chLatin_o, chLatin_m, chNull}, 
								  el->getTagName()))
		{			
			range.first = atoi(toNative(dynamic_cast<DOMCharacterData *>
										(el->getFirstChild())->getData()).c_str());
		}
		else if(XMLString::equals((XMLCh[]){chLatin_t, chLatin_o, chNull}, 
								  el->getTagName()))
		{
			range.second = atoi(toNative(dynamic_cast<DOMCharacterData *>
										(el->getFirstChild())->getData()).c_str());;
		}		
	}
}

XMLCh * MsgList::GetName()
{
	static XMLCh name[] = {chLatin_l, chLatin_i, chLatin_s, chLatin_t,
						    chNull};
	return name;
}

const std::string& MsgList::GetDir()
{
	return *dir;
}

int MsgList::GetRecurse()
{
	return recurse;
}

std::pair<int, int> MsgList::GetRange()
{
	return range;
}

int MsgList::GetFrom()
{
	return range.first;
}

int MsgList::GetTo()
{
	return range.second;
}

const MetadataInfo& MsgList::GetMetaDataRequest()
{
	return metaDataRequest;
}

Message::DOMSerializedMessage
MsgList::Serialize(xercesc::DOMDocument & document) {
//	return std::tr1::shared_ptr<xercesc::DOMDocument >(NULL);
	DOMSerializedMessage Result (InitializeSerialization(document));
    
	{//recurse
		DOMElement * DERecurse
			= document.createElement ((XMLCh[]){chLatin_r, chLatin_e, chLatin_c, chLatin_u, chLatin_r, chLatin_s, chLatin_e, chNull});
		if (recurse==-1)
			DERecurse->appendChild (
				document.createTextNode(fromNative("\342\210\236").c_str())
			);
		else
			DERecurse->appendChild (
				document.createTextNode(fromNative(toString(recurse)).c_str())
			);
			
		Result->appendChild (DERecurse);
	}	
	
	if (dir and not dir->empty())
	{
		DOMElement * DEDir
			= document.createElement ((XMLCh[]){chLatin_d, chLatin_i, chLatin_r, chNull});
		DEDir->appendChild (
			document.createTextNode(fromNative(*dir).c_str())
		);
		Result->appendChild (DEDir);
	
	}
	xml::CommonDOMFeatures::SetElementNamespace(*Result,
			xml::CommonDOMFeatures::GetClientNamespaceURI());
	return Result;
};

void MsgList::DefaultProcessImpl(ServerControl& t)
{		
	server::MsgList *lst = new server::MsgList;
	std::tr1::shared_ptr<Message> l(lst);
	
	std::tr1::shared_ptr<std::string> newdir;
	
	if ( (not dir) or (dir->empty()) )
	{
		newdir.reset(new std::string(t.GetDir ()));
		newdir->push_back ('/');		
	}
	else if ( dir->at (0)=='/'  )
		newdir=dir;	
	else
	{
		newdir.reset(new std::string(t.GetDir ()));
		newdir->push_back ('/');
		newdir->append(*dir);
	}
		
	try
	{
		std::cout << "Creating list " << *newdir << std::endl;
		//t.ValidateDir (*newdir);
		lst->Fill (*newdir, recurse);
		
	}
	catch (...)
	{
		server::MsgError * ack = new server::MsgError ;
		ack-> SetPointer (GetID ());
		ack-> SetTextMsg (stringError (EACCES));
		t.xmlStream.PostMessage ( std::tr1::shared_ptr<Message>(ack), true);
		return;
	}
	t.xmlStream.PostMessage(l);
}
