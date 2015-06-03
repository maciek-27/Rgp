#ifndef __XFTP__MESSAGE_H__
#define __XFTP__MESSAGE_H__
#include <xercesc/dom/DOM.hpp>
#include "libstreamcon/commons.h++"
#include "libstreamcon/threads.h++"
#include <queue>
#include <xercesc/util/XMLUniDefs.hpp>
#include "xercesstrings.h++"
#include <boost/mpl/for_each.hpp>
#include "callbackfunction.h++"
#include "boost/type_traits/is_pointer.hpp"
#include "boost/type_traits/remove_pointer.hpp"
#include "boost/type_traits/add_reference.hpp"
#include "servercontrol.h++"
#include "clientcontrol.h++"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
namespace xftp { 
	
	class ControlThread;
	class ServerMessage;
	class ClientMessage;

	class ServerControl;
	class ClientControl;
	
	/*!
	  Class representing a deserialized XML message.
	*/
	class Message
	{
		friend class XMLStream;
	public:
		typedef
		boost::function<void (ControlThread*, Message&) > ProcessImplBaseType;
		typedef
		CallbackFunctionBase<ProcessImplBaseType> ProcessImplType;
		
		Message(size_t _ID, ProcessImplType *_PI);
		//Message(size_t _ID, _PI);
		/*!
		  Returns XML tag name of specific message (this will be used
		  as DOMelement name while serializing message.
		 */
		virtual XMLCh * GetName() = 0;
		
		/*! 
		  Returns message ID used while while serializing message. this
		  is unique identifier of message, and will be used by server while 
		  referencing this message.
		 */
		size_t GetID();
		
		/*!
		  Process the message and modify ControlThread
		  accordingly.
		 */
		void Process(ControlThread&);

		/*!
		  Each message is serialized to DOM tree, which 
		  is processed by XMLStream::PostMessage
		 */
		typedef xercesc::DOMElement *DOMSerializedMessage;
		
		/*!
		  Transform Message into DOM tree form, which will later be transformed
		  into textual XML form.
		 */
		virtual	DOMSerializedMessage Serialize(
				xercesc::DOMDocument & document) = 0;
		virtual ~Message();
	private:
		//! function adaptor, that casts arguments
		template <class T>	
		class CustomProcessImplT
		{
			T f;
		public:
			CustomProcessImplT(T f_): f(f_) {}
			void operator()(ControlThread* c, Message& m)
				{
					f(reinterpret_cast<typename T::first_argument_type >(c),
					  reinterpret_cast<typename T::second_argument_type >(m));
				}
		};

		//! set function directly
		template <typename T>
		ProcessImplBaseType SetProcessImplReal(T i,
											   boost::mpl::bool_<true>
											   ThisVersionForGenericImpl)
			{
				return ProcessImpl->SetFunction(i);

			}

		//! set function through CustomProcessImplT
		template <typename T>
		ProcessImplBaseType SetProcessImplReal(T i,
											   boost::mpl::bool_<false>
											   ThisVersionForCustomImpl)
			{
				return ProcessImpl->SetFunction(
					SetProcessImpl(
 						ProcessImplBaseType(
 							CustomProcessImplT<T>(i)
 							)
						)
					);

			}
		
		
	public:

		//! replace implementation of Process function
		template <typename T>
		ProcessImplBaseType SetProcessImpl(T i)
			{
				
				typename
				boost::mpl::if_<
				boost::is_same<T,ProcessImplBaseType>,
				boost::mpl::bool_<true>,
				boost::mpl::bool_<false>
					>::type Version;
				return SetProcessImplReal(i,Version);
			}
		

		ProcessImplBaseType GetProcessImpl();
		
	protected:
		
		DOMSerializedMessage InitializeSerialization(
				xercesc::DOMDocument & document);
		
		std::auto_ptr<ProcessImplType > ProcessImpl;
		
		//! Casts ControlThread to ClientControl
		template<typename TAG>
		class DefaultServerMessageProcessRoutine
		{
		public:
			void operator()(ControlThread* c, Message& m)
			{
			reinterpret_cast<typename boost::add_reference<typename
					boost::remove_pointer<TAG>::type>::type>(m)
				.DefaultProcessImpl(dynamic_cast<ClientControl&>(*c));
			}
		};
		
		//! Casts ControlThread to ServerControl
		template<typename TAG>
		class DefaultClientMessageProcessRoutine
		{
		public:
			void operator()(ControlThread* c, Message& m)
			{
				// no dynamic cast needed as one true type is enforced.
			reinterpret_cast<typename boost::add_reference<typename
					boost::remove_pointer<TAG>::type>::type>(m)
				.DefaultProcessImpl(dynamic_cast<ServerControl&>(*c));
			}
		};
		
		//! TAG is typename of pointer to specific message
		template<typename TAG>
		class DefaultProcessRoutine: public boost::mpl::if_<
						 boost::is_base_of<ClientMessage,
						typename boost::remove_pointer<TAG>::type >
						   , DefaultClientMessageProcessRoutine<TAG>
						    , DefaultServerMessageProcessRoutine<TAG>
															>::type 
		{
			BOOST_STATIC_ASSERT( (boost::is_base_of<Message,
			typename boost::remove_pointer<TAG>::type>::value) );
		};
		
		static streamcon::Mutex GlobalSetupLock;
		
		//! Get single message processing routine
		//! \param elem is pointer to specific message type. It does not have to be
		//!  initialized or even valid as only typename matters here. 
		template<typename TAG>
		Message::ProcessImplType * ProcessImplDefault(TAG elem)
		{
			//! assure apropriate type.
			BOOST_STATIC_ASSERT( (boost::is_base_of<Message,
			typename boost::remove_pointer<TAG>::type>::value) );
			ProcessImplType * result = 
			new  CallbackFunction<Message::ProcessImplBaseType,TAG>;
			if (result->GetFunction().empty())
			{
				// setup only if not yet initialized
				streamcon::ScopedLock L(GlobalSetupLock);
				result->SetFunction(DefaultProcessRoutine<TAG>());
			}
			return result; 
		}

	private:
				
		size_t ID; // message ID
	};


	class ClientControl;
	/*!
	  Class representing a deserialized server specific
	  XML message.	  
	 */
	class ServerMessage : public Message
	{
	public:
		ServerMessage(size_t ID, ProcessImplType *_PI);
		//virtual void Process(ControlThread& control);
		//virtual void DefaultProcessImpl(ClientControl& control) = 0;
	};

	class ServerControl;
	/*!
	  Class representing a deserialized client specific
	  XML message.	  
	 */
	class ClientMessage : public Message
	{
	public:
		ClientMessage(size_t ID, ProcessImplType *_PI);
		//virtual void Process(ControlThread& control);
		//virtual void DefaultProcessImpl(ServerControl& control) = 0;
	};
}

#endif
