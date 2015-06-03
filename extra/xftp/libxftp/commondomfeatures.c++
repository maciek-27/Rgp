#include"commondomfeatures.h++"
#include<xercesc/util/PlatformUtils.hpp>
#include<xercesc/util/XMLUniDefs.hpp>
#include<iostream>
#include <libstreamcon/throw.h++>
#include <boost/thread/detail/singleton.hpp>
#include <libxftp/xercesstrings.h++>
using namespace xercesc;
xftp::xml::CommonDOMFeatures::CommonDOMFeatures()
{

	XMLPlatformUtils::Initialize();
	Implementation= // Get first registered implementation
					// that has desired properties
					// (currently only availble)
			DOMImplementationRegistry::getDOMImplementation(
						(XMLCh[]){chLatin_L,chLatin_S,chNull});
															
	Serializer.reset(
		((DOMImplementationLS*) // Serializer will be used to transform
								 // DOM trees into textual XML documents
			Implementation)->createDOMWriter());
	
    if (Serializer->canSetFeature(//basic serializer setup
		xercesc::XMLUni::fgDOMWRTDiscardDefaultContent, true))
        Serializer->setFeature(
		xercesc::XMLUni::fgDOMWRTDiscardDefaultContent, true);

    if (Serializer->canSetFeature(
		xercesc::XMLUni::fgDOMWRTFormatPrettyPrint, true))
		Serializer->setFeature(
		xercesc::XMLUni::fgDOMWRTFormatPrettyPrint, true);
				
//	std::cerr<<"xmlinit"<<std::endl;
}


std::tr1::shared_ptr<xercesc::DOMWriter> 
		xftp::xml::CommonDOMFeatures::getSerializer()
{
	return Serializer;
}
		
xftp::xml::CommonDOMFeatures::~CommonDOMFeatures()
{
//	std::cerr<<"xmlterm"<<std::endl;
	std::tr1::shared_ptr<xercesc::DOMWriter> xdw;
	Serializer=xdw;	
	XMLPlatformUtils::Terminate();
}

const XMLCh* xftp::xml::CommonDOMFeatures::GetClientNamespaceURI()
{
	static const XMLCh uri[] = {chLatin_c,chLatin_l,chLatin_i,
								chLatin_e,chLatin_n,chLatin_t,chNull};
	return uri;
}

const XMLCh* xftp::xml::CommonDOMFeatures::GetServerNamespaceURI()
{
	static const XMLCh uri[] = {chLatin_s,chLatin_e,chLatin_r,
								chLatin_v,chLatin_e,chLatin_r,chNull};
	return uri;

}

DOMDocument * xftp::xml::CommonDOMFeatures::createDocument()
{
	return Implementation->createDocument ();
}

xftp::xml::CommonDOMFeatures & xftp::xml::CommonDOMFeatures::Instance()
{
	return boost::detail::thread::singleton
				<xml::CommonDOMFeatures>::instance(); 
}
const XMLCh* xftp::xml::CommonDOMFeatures::GetElementNamespace(const xercesc::DOMElement & e)
{
	const XMLCh* NSU =  e.getNamespaceURI();
	
	const XMLCh * Result;
	const xercesc::DOMElement * pe = &e;
	//try getting xmlns for this element
	while (XMLString::equals((XMLCh[]){chNull},
			 (Result = pe->getAttributeNS(NSU,const_cast<XMLCh*>((XMLCh[])
				 {chLatin_x,chLatin_m,chLatin_l,chLatin_n,chLatin_s,chNull})) )))
	{//if failedm try parent
		pe=dynamic_cast<xercesc::DOMElement*>(pe->getParentNode());
		if (not pe) // dynamic cast fails if there is no parent element (i.e. pe
			// represents root element)
			THROW(GetNamespaceFailed);
	}
	return Result;		
}



void xftp::xml::CommonDOMFeatures::SetElementNamespace(xercesc::DOMElement & e,const XMLCh* n)
{
	e.setAttribute((XMLCh[]){chLatin_x, chLatin_m, chLatin_l, chLatin_n, chLatin_s, chNull},n);
}
		
void xftp::xml::CommonDOMFeatures::NormalizeElementNamespaces(xercesc::DOMElement & e)
{
	
	xercesc::DOMElement * pe = &e;
	const XMLCh* NSU =  e.getNamespaceURI();

	DOMNodeList* children = pe->getChildNodes();
	const  XMLSize_t nodeCount = children->getLength();
		

	for( XMLSize_t i = 0; i < nodeCount; ++i )
	{
		DOMElement* currentNode = dynamic_cast<DOMElement*>
			(children->item(i));
		if (not currentNode) continue;
		
		const XMLCh* ceNS = 
			currentNode->getAttributeNS(NSU,const_cast<XMLCh*>((XMLCh[])
				{chLatin_x,chLatin_m,chLatin_l,chLatin_n,chLatin_s,chNull}));
		const XMLCh* peNS = 
			pe->getAttributeNS(NSU,const_cast<XMLCh*>((XMLCh[])
				{chLatin_x,chLatin_m,chLatin_l,chLatin_n,chLatin_s,chNull}));
//		std::cout << "Fixing namespace " << toNative(ceNS) << ',' << toNative(peNS) << std::endl;
		if (ceNS)
		{
			if (XMLString::equals(peNS,(XMLCh[]){chNull}))
				pe->setAttribute((XMLCh[]){chLatin_x, chLatin_m, chLatin_l, chLatin_n, chLatin_s, chNull},ceNS);
			else if (not XMLString::equals(ceNS,peNS))
				continue;
			currentNode->removeAttribute((XMLCh[]){chLatin_x, chLatin_m, chLatin_l, chLatin_n, chLatin_s, chNull});
		}
	}
}

