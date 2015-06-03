#include "serverinfo.h++"

#include<libxftp/commondomfeatures.h++>
using namespace xftp;
using namespace server;
using namespace xercesc;

MsgServerInfo::MsgServerInfo(DOMElement *elem, size_t ID)
 : ServerMessage (ID, ProcessImplDefault(this))
{	
	DOMNodeList* children = elem->getChildNodes();
	const  XMLSize_t nodeCount = children->getLength();
	for( XMLSize_t i = 0; i < nodeCount; ++i )
	{		
		DOMElement * el = dynamic_cast<DOMElement *>(children->item(i));
		if (not el)
			continue;
		if(XMLString::equals((XMLCh[]){chLatin_r, chLatin_o, chLatin_o, chLatin_t, chUnderscore, chLatin_d, chLatin_i, chLatin_r, chNull}, 
							 el->getTagName()))
		{
			if(el->getFirstChild())
				rootDir.assign(
					toNative(dynamic_cast<DOMCharacterData*>
							 (el->getFirstChild())->getData()));
		}
		else if(XMLString::equals((XMLCh[]){chLatin_s, chLatin_t, chLatin_a, chLatin_r, chLatin_t, chLatin_i, chLatin_n, chLatin_g, chUnderscore, chLatin_d, chLatin_i, chLatin_r, chNull}, 
							 el->getTagName()))
		{
			if(el->getFirstChild())
				startingDir.assign(
					toNative(dynamic_cast<DOMCharacterData*>
							 (el->getFirstChild())->getData()));
		}
		else if(XMLString::equals((XMLCh[]){chLatin_m, chLatin_e, chLatin_t, chLatin_a, chLatin_d, chLatin_a, chLatin_t, chLatin_a, chUnderscore, chLatin_i, chLatin_n, chLatin_f, chLatin_o, chNull}, 
							 el->getTagName()))
		{
			metaData.flags.branch = false;
			metaData.flags.data = false;
			metaData.filename = false;
			metaData.filesize = false;

			DOMNodeList* children = elem->getChildNodes();
			const  XMLSize_t nodeCount = children->getLength();
			for( XMLSize_t i = 0; i < nodeCount; ++i )
			{					
				DOMElement * el = dynamic_cast<DOMElement *>(children->item(i));
				if (not el)
					continue;
				if(XMLString::equals((XMLCh[]){chLatin_f, chLatin_l, chLatin_a, chLatin_g, chLatin_s, chNull}, 
									 el->getTagName())) {
					DOMNodeList* children = elem->getChildNodes();
					const  XMLSize_t nodeCount = children->getLength();
					for( XMLSize_t i = 0; i < nodeCount; ++i )
					{					
						DOMElement * el = dynamic_cast<DOMElement *>(children->item(i));
						if(XMLString::equals((XMLCh[]){chLatin_b, chLatin_r, chLatin_a, chLatin_n, chLatin_c, chLatin_h, chNull}, 
											 el->getTagName())) {
							metaData.flags.branch = true;
						}
						else if(XMLString::equals((XMLCh[]){chLatin_d, chLatin_a, chLatin_t, chLatin_a, chNull}, 
												  el->getTagName())) {
							metaData.flags.data = true;
						}						
					}					
				}
				else if(XMLString::equals((XMLCh[]){chLatin_f, chLatin_i, chLatin_l, chLatin_e, chLatin_n, chLatin_a, chLatin_m, chLatin_e, chNull}, 
										  el->getTagName())) {
					metaData.filename = true;
				}
				else if(XMLString::equals((XMLCh[]){chLatin_f, chLatin_i, chLatin_l, chLatin_e, chLatin_s, chLatin_i, chLatin_z, chLatin_e, chNull},
										  el->getTagName())) {
					metaData.filesize = true;
				}
			}
		}
	}
}

MsgServerInfo::MsgServerInfo()
 : ServerMessage (-1, ProcessImplDefault(this))
{
	rootDir="/";
	char * d = get_current_dir_name();
	startingDir=d;
	free(d);
}

XMLCh * MsgServerInfo::GetName()
{
	static XMLCh name[] = {chLatin_s, chLatin_e, chLatin_r, chLatin_v,
						   chLatin_e, chLatin_r, chUnderscore, 
						   chLatin_i, chLatin_n, chLatin_f, chLatin_o,
						   chNull};
	return name;
}

const std::string& MsgServerInfo::GetRootDir()
{
	return rootDir;
}

const std::string& MsgServerInfo::GetStartingDir()
{
	return startingDir;
}

const MetadataInfo& MsgServerInfo::GetMetaData()
{
	return metaData;
}

Message::DOMSerializedMessage
MsgServerInfo::Serialize(xercesc::DOMDocument & document)
{
	DOMSerializedMessage Result (InitializeSerialization(document));
	xml::CommonDOMFeatures::SetElementNamespace(*Result,
												xml::CommonDOMFeatures::GetServerNamespaceURI());
	
	DOMElement * DErootDir =
			document.createElement ((XMLCh[]){chLatin_r, chLatin_o, chLatin_o, chLatin_t, chUnderscore, chLatin_d, chLatin_i, chLatin_r, chNull});
	DErootDir->appendChild(
			document.createTextNode(fromNative(rootDir).c_str()));
	Result->appendChild (DErootDir);

	DOMElement * DEstartingDir =
			document.createElement ((XMLCh[]){chLatin_s, chLatin_t, chLatin_a, chLatin_r, chLatin_t, chLatin_i, chLatin_n, chLatin_g, chUnderscore, chLatin_d, chLatin_i, chLatin_r, chNull});
	DEstartingDir->appendChild(
			document.createTextNode(fromNative(startingDir).c_str()));
	Result->appendChild (DEstartingDir);
	
	DOMElement * DEmetaData =
		document.createElement ((XMLCh[]){chLatin_m, chLatin_e, chLatin_t, chLatin_a, chLatin_d, chLatin_a, chLatin_t, chLatin_a, chUnderscore, chLatin_i, chLatin_n, chLatin_f, chLatin_o, chNull});

	DOMElement * DEflags = 
		document.createElement ((XMLCh[]){chLatin_f, chLatin_l, chLatin_a, chLatin_g, chLatin_s, chNull});

	if(metaData.flags.branch)
		DEflags->appendChild(
			document.createElement((XMLCh[]){chLatin_b, chLatin_r, chLatin_a, chLatin_n, chLatin_c, chLatin_h, chNull}));
	if(metaData.flags.data)
		DEflags->appendChild(
			document.createElement((XMLCh[]){chLatin_d, chLatin_a, chLatin_t, chLatin_a, chNull}));
	DEmetaData->appendChild(DEflags);
	
	if(metaData.filename)
		DEmetaData->appendChild(
			document.createElement((XMLCh[]){chLatin_f, chLatin_i, chLatin_l, chLatin_e, chLatin_n, chLatin_a, chLatin_m, chLatin_e, chNull}));

	if(metaData.filesize)
		DEmetaData->appendChild(
			document.createElement((XMLCh[]){chLatin_f, chLatin_i, chLatin_l, chLatin_e, chLatin_s, chLatin_i, chLatin_z, chLatin_e, chNull}));
	Result->appendChild (DEmetaData);
	
	return Result;
};

void MsgServerInfo::DefaultProcessImpl(ClientControl& control)
{
	;
}
