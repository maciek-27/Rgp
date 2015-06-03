#include "open.h++"

#include<libxftp/commondomfeatures.h++>
#include <libxftp/client.h++>
#include <libxftp/servercontrol.h++>
#include <libxftp/server/error.h++>
#include <libxftp/server/acknowledgement.h++>
#include "filename_operations.h++"
using namespace xftp;
using namespace client;
using namespace xercesc;

FilenameOperations::FilenameOperations(DOMElement *elem, size_t ID,ProcessImplType *_PI) : ClientMessage (ID, _PI)
{	   
	DOMNodeList* children = elem->getChildNodes();
	const  XMLSize_t nodeCount = children->getLength();
	for( XMLSize_t i = 0; i < nodeCount; ++i )
	{		
		DOMElement * el = dynamic_cast<DOMElement *>(children->item(i));
		if (not el)
			continue;
		if(XMLString::equals((XMLCh[]){chLatin_f, chLatin_i, chLatin_l, chLatin_e, chLatin_n, chLatin_a, chLatin_m, chLatin_e, chNull}, 
							 el->getTagName()))
		{
			filename = 
				toNative(dynamic_cast<DOMCharacterData*>
									 (el->getFirstChild())->getData());
			
		}
	}
}

FilenameOperations::FilenameOperations(ProcessImplType *_PI) : ClientMessage (-1, _PI){;}

XMLCh * FilenameOperations::GetName()
{
	static XMLCh name[] = {chLatin_o, chLatin_p, chLatin_e, chLatin_n
						   , chNull};
	return name;
}

void FilenameOperations::SetFilename(const std::string& _filename)
{
	filename = _filename;
}

const std::string& FilenameOperations::GetFilename()
{
	return filename;
}

Message::DOMSerializedMessage FilenameOperations::Serialize(xercesc::DOMDocument & document)
{
	DOMSerializedMessage Result (InitializeSerialization(document));
	
	xml::CommonDOMFeatures::SetElementNamespace(*Result,
			xml::CommonDOMFeatures::GetClientNamespaceURI());
	DOMElement * DEfilename =
		document.createElement ((XMLCh[]){chLatin_f, chLatin_i, chLatin_l, chLatin_e, chLatin_n, chLatin_a, chLatin_m, chLatin_e, chNull});
	DEfilename->appendChild(
		document.createTextNode(fromNative(filename).c_str()));	
	Result->appendChild (DEfilename);

	return Result;
};
