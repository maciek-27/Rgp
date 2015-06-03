#ifndef __XFTP__XML__STREAM_H__
#define __XFTP__XML__STREAM_H__

//#include "message.h++"
#include "libstreamcon/throw.h++"
#include "libstreamcon/commons.h++"
#include "libstreamcon/stream.h++"

#include "commondomfeatures.h++"
#include <queue>
#include "dictionary.h++"
#include<xercesc/framework/MemBufInputSource.hpp>
#include <boost/scoped_array.hpp>
#include<boost/function.hpp>

namespace xftp {
	using namespace streamcon;	
	class Message;
	//! xml processing of stream
	class XMLStream
	{
	public:
		/*!
		 * Local data type: smart function pointer
		 */
		typedef boost::function<size_t (boost::scoped_array<XMLByte>&,size_t)>
		FilterFunc;

	private:
		/*!
		 * Set of Xerces-provided factory classes + management of Xerces setup
		 * common for all instances of XMLStream
		 */
		xml::CommonDOMFeatures & _CommonDOMFeatures;
		
		//! FIFO for message processing - typedef
		typedef std::queue<std::tr1::shared_ptr<Message>,
				std::list<std::tr1::shared_ptr<Message> > >
						RXMessageQueue;
		
		//! FIFO for message processing - instance
		RXMessageQueue RXMessages;
		
		//! Outgoing messages DOM tree (state saved between posting if
		//! flush not forced
		xercesc::DOMDocument * TXMessages;
		
		//! real SCTP stream used to communicate (I/O)
		Stream * s;
		
		//! current filter (filter chain) for incoming XML
		FilterFunc GetFilter;
		
		//! current filter (filter chain) for outgoing XML
		FilterFunc PostFilter;
		
		//! counter of sent messages - used to apply ID to them
		unsigned int msg_ctr;
	public:
		
		//! constructor for XML stream. 
		//! \note  XMLStream doesn't manage _s as its own resource. it won't
		//! free it.
		explicit
		XMLStream(Stream * _s);
		~XMLStream();
		
		//! Send XFTP message over the network
		void PostMessage(std::tr1::shared_ptr<Message> message,
				bool FlushAfterPosting=true);
		
		//! read next incoming message (pleas refer to Programmers' manual
		//! for more details)
		std::tr1::shared_ptr<Message> GetMessage();


		/*!
		 * Set function f, that will be executed immediately after
		 * reading raw XML text from SCTP source
		 */
		void SetGetFilter(FilterFunc f);

		/*!
		 * Set function f, that will be executed immediately before
		 * writing raw XML text to SCTP output
		 */
		void SetPostFilter(FilterFunc f);
	};
}

#endif

