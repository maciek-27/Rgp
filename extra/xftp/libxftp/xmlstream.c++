#include"xmlstream.h++"
#include<iostream>
#include <boost/thread/detail/singleton.hpp>
#include<xercesc/parsers/XercesDOMParser.hpp>
#include<xercesc/framework/MemBufInputSource.hpp>
#include<xercesc/util/XMLUniDefs.hpp>
#include "libxftp/commondomfeatures.h++"
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include<allmessages.h++>
#include<iostream>
#include <boost/static_assert.hpp>

#include <boost/typeof/typeof.hpp>
#include "boost/mpl/for_each.hpp"
#include "boost/mpl/transform.hpp"
#include "boost/type_traits/add_pointer.hpp"
#include "boost/type_traits/remove_pointer.hpp"
#include <boost/type_traits/is_same.hpp> 
#include <boost/static_assert.hpp>
using namespace std;
using namespace xftp;
using namespace XERCES_CPP_NAMESPACE;


//! local anonymous namespace - works as static/
namespace{
/*!
 * Function serializes DOMElement elem into new message of type MSG
 */
template <typename MSG>
	Message * DeserializeMessage(DOMElement *elem, size_t ID)
		{
	
			ID=atoi(toNative(elem->getAttribute(XercesSsie("id")
					.c_str())).c_str());
			return new MSG(elem,ID);

		}
/*!
 * Boost function template conforming DeserializeMessage interface
 */
typedef boost::function<Message *(DOMElement *elem, size_t ID)> Deserializer;


/*!
 * functor to insert messages into dictionary
 */
template <class D>
class DictionaryInserter
{
private:
	//! Target dictionary
	D & dic;
public:
	//! basic constructor 
	//! \param d target dictionary
	explicit DictionaryInserter(D&d):dic(d){}
	
	//! Function implementing insertion
    template<class T>
    void operator() (T t)
        {			
			typename boost::remove_pointer<T>::type message;
			dic.insert(message.GetName(),DeserializeMessage<
				typename boost::remove_pointer<T>::type >);
        }	
};

/*!
 * Common iterator classes and subroutines
 */
class MessageDictionaryBase : public xftp::Dictionary<Deserializer,XMLCh>
{
protected:
	//! protected constructor to disable construction other than by singleton
	//! instance() call
	MessageDictionaryBase(){}
	~MessageDictionaryBase(){}
};


/*!
 * Basic message dictionary. Template argument is boost mpl list containing
 * message classes used in constructor;
 */
template < class MSGS>
class MessageDictionary : public MessageDictionaryBase
{
protected:
	/*
	 * MPL constructor
	 */
	MessageDictionary ()
	{
		boost::mpl::for_each<
				typename boost::mpl::
				transform<MSGS, boost::add_pointer<boost::mpl::_1> >::type
			>(DictionaryInserter<MessageDictionary<MSGS> >(*this));
	}
	
	~MessageDictionary (){}
	/*!
	 * Return single instance
	 */
	static MessageDictionary<MSGS> &
	Instance ()
	{
		return boost::detail::thread::singleton
		<MessageDictionary<MSGS> >::instance ();
	}
	
public:
	
	static element_type Detect (string_type tag)
	{
		BOOST_STATIC_ASSERT( (boost::is_same<
				boost::function<Message *(DOMElement *elem, size_t ID)>,
				element_type>::value) );		
		
		element_type MsgCtor;
		iterator i = Instance ().find(tag);
		if (i.valid())
			MsgCtor=*i;
		return MsgCtor;
	}
};

/*!
 * Server message dictionary maps XMLCh* strings to apropriate message types.
 * is used while deserializing messages
 */
typedef MessageDictionary<server::MessagesList> ServerMessageDictionary;

/*!
 * Client message dictionary maps XMLCh* strings to apropriate message types.
 * is used while deserializing messages
 */
typedef MessageDictionary<client::MessagesList> ClientMessageDictionary;

}// end of local <anonymous> namespace

XMLStream::XMLStream(xftp::Stream * _s):
	_CommonDOMFeatures(boost::detail::thread::singleton
					   <xml::CommonDOMFeatures>::instance() ),
	TXMessages(0),s(_s)
{
//	cout << "xftp::XMLStream::XMLStream()" <<endl;
	msg_ctr=1;
}
	
void XMLStream::PostMessage(std::tr1::shared_ptr<Message> message,
			bool FlushAfterPosting)
{
	
	std::cout << "Sending message " << "\x1b[31m" <<			
		toNative(message->GetName()) <<  "\x1b[0m" << std::endl;

	if (not TXMessages)
	{		
		TXMessages = boost::detail::thread::singleton
		<xml::CommonDOMFeatures>::instance ().createDocument ();
		
		TXMessages->appendChild (
			TXMessages->createElement (
			(XMLCh[]){chLatin_x, chLatin_f, chLatin_t, chLatin_p, chNull}));	
	}
	
	reinterpret_cast<xercesc::DOMElement*>(
	TXMessages->getDocumentElement()->appendChild(
		message->Serialize(*TXMessages)))
		->setAttribute((XMLCh[]){chLatin_i, chLatin_d, chNull},
			fromNative(toString(msg_ctr++)).c_str());
	if (FlushAfterPosting)
	{
		
		_CommonDOMFeatures.NormalizeElementNamespaces( 
			*(TXMessages->getDocumentElement()));	
		std::tr1::shared_ptr<xercesc::DOMWriter> Writer (
		boost::detail::thread::singleton
		<xftp::xml::CommonDOMFeatures>::instance ().getSerializer());	
		
		xercesc::MemBufFormatTarget *myFormTarget = 
			new xercesc::MemBufFormatTarget();
		
		Writer->writeNode(myFormTarget,*TXMessages);
		
		size_t size = myFormTarget->getLen();
		XMLByte* tmp = new XMLByte[size+1];
		memcpy(tmp,myFormTarget->getRawBuffer(),size);

		boost::scoped_array<XMLByte> buf(tmp);

		buf[size]=0;
		if (PostFilter)
			size=PostFilter(buf,size);


		s->SendData(reinterpret_cast<char*>(&size),sizeof(unsigned int));
		
		s->SendData(reinterpret_cast<char*>(buf.get()) ,size);

		buf.reset(0);
		delete myFormTarget;
			
		delete TXMessages;
		TXMessages=0;
	}
}

std::tr1::shared_ptr<Message> XMLStream::GetMessage()
{
	while (RXMessages.empty())
	{
		unsigned int size;
		BOOST_STATIC_ASSERT(sizeof(unsigned int)==4);
		/*download 4 byte integer*/
		
		s->GetData(reinterpret_cast<char*>(&size),4);
		boost::scoped_array<XMLByte> buf( new XMLByte[size+1] );
		unsigned int nread = 0;
		while(nread != size) {
//			std::cout << "Read: " << nread << std::endl;
			nread += 
				s->GetData(reinterpret_cast<char*>(buf.get())
						   + nread,size - nread);
		}
		buf[size]=0;

		if (GetFilter)
			size=GetFilter(buf,size);
//		std::cout << buf.get() << std::endl;
		XercesDOMParser parser;
		parser.setValidationScheme(XercesDOMParser::Val_Never);
		static const char  efs_ident [] = "extracted from stream";
		MemBufInputSource  buf_src(buf.get(), size,efs_ident,false); // false = don't
		//let xerces manage our data

		// read XML stream, deserialize
		parser.parse(buf_src);
		DOMDocument * document = parser.getDocument();  // XML document as node set
		
		
		DOMElement* elementRoot = document->getDocumentElement();
		if( !elementRoot )//no elements at all - no XML
			throw(std::runtime_error( "empty XML document" ));
		
		DOMNodeList* children = elementRoot->getChildNodes();
		const  XMLSize_t nodeCount = children->getLength();
		//Deserialize XML message in correct document
		for( XMLSize_t i = 0; i < nodeCount; ++i )
			{// for each ELEMENT node (omit other)
				DOMNode* currentNode = children->item(i);
				if (currentNode -> getNodeType()!=DOMNode:: ELEMENT_NODE )
					continue;
				DOMElement * el = reinterpret_cast<DOMElement *>(currentNode);
				
				//Function used to deserialize message
				Deserializer deserializer;
				
				if (XMLString::equals(_CommonDOMFeatures.GetServerNamespaceURI(),
					_CommonDOMFeatures.GetElementNamespace(*el)))// namespace check	
					deserializer=ServerMessageDictionary
							::Detect(el->getTagName());
				else if (XMLString::equals (_CommonDOMFeatures.GetClientNamespaceURI (),
					_CommonDOMFeatures.GetElementNamespace(*el)))		
					deserializer=ClientMessageDictionary
							::Detect(el->getTagName());
				
				if (deserializer != 0)
					RXMessages.push(//execute function from apropriate 
									//constructor dictionary 
					std::tr1::shared_ptr<Message>(deserializer(el,i+1)));
				else
				{
					std::cout << "Recieved UNSUPPORTED message " << "\x1b[32m"
						<< toNative(el->getTagName()) <<  "\x1b[0m\n" 
						<< "Will ignore it."<< std::endl;
					continue;
				}				
			}
		
	}
	// Assume, that at least one message was deserialized. pop it from front of
	// the queue (if queue is empty (what means, that no messages were recieved
	// in XML), function will crash)
	std::tr1::shared_ptr<Message> result = RXMessages.front();
	RXMessages.pop();
	
	std::cout << "Recieved message " << "\x1b[32m" <<
			
		toNative(result->GetName()) <<  "\x1b[0m" << std::endl;
	
	return result;
}

XMLStream::~XMLStream()
{
//	cout << "xftp::XMLStream::~XMLStream()" <<endl;
	
}

void XMLStream::SetPostFilter(FilterFunc f)
{
	PostFilter=f;
}

void XMLStream::SetGetFilter(FilterFunc f)
{
	GetFilter=f;
}



