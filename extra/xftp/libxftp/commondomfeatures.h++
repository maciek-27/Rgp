#ifndef __XFTP__COMMON_DOM_FEATURES_H__
#define __XFTP__COMMON_DOM_FEATURES_H__
#include <xercesc/dom/DOM.hpp>
#include"libstreamcon/commons.h++"
#include <libstreamcon/commons.h++>
namespace xftp { namespace xml{
	class CommonDOMFeatures 
	{
	public:
		/*!
		 * Returns single instance of global DOM serializer.
		 *  (in future behaviour of this function may change: i.e. it may 
		 *   return new instance of DOMWriter each time it is called)
		 */
		std::tr1::shared_ptr<xercesc::DOMWriter> getSerializer();
		
		/*!
		 * Returns client namespace URI
		 */
		static const XMLCh* GetClientNamespaceURI();
		
		/*!
		 * Returns server namespace URI
		 */
		static const XMLCh* GetServerNamespaceURI();
		
		/*!
		 * Create new XML document
		 */
		xercesc::DOMDocument * createDocument();
		
		/*!
		 * Returns single instance of CommonDOMFeatures as singleton
		 * type object
		 */
		static CommonDOMFeatures & Instance();
		
		
		__DE(GetNamespaceFailed,streamcon::Exception);
		/*!
		 * Returns namespace URI
		 * \throws CommonDOMFeatures::GetNamespaceFailed
		 */
		static const XMLCh* GetElementNamespace(const xercesc::DOMElement & e);
		
		
		/*!
		 *Set element namespace URI		 *
		 */
		static void SetElementNamespace(xercesc::DOMElement & e,const XMLCh* n);
		
		static void NormalizeElementNamespaces(xercesc::DOMElement & e);		
	protected:
		xercesc::DOMImplementation * Implementation;
		std::tr1::shared_ptr<xercesc::DOMWriter> Serializer;
		CommonDOMFeatures();
		~CommonDOMFeatures();
	};
	
			
}}

#endif
