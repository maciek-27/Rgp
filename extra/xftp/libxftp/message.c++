
#include "message.h++"
#include <boost/thread/detail/singleton.hpp>
#include "commondomfeatures.h++"
#include "xftpcommons.h++"
#include <iostream>
#include "servercontrol.h++"
#include "clientcontrol.h++"

using namespace xftp;


streamcon::Mutex Message::GlobalSetupLock;
		
Message::Message(size_t _ID, ProcessImplType *_PI): ProcessImpl(_PI)
{
	ID=_ID;
}

size_t Message::GetID()
{
	return ID;
}

void Message::Process(ControlThread&t)
{
	(*ProcessImpl)(&t,*this);
}
Message::~Message()
{
}

Message::DOMSerializedMessage Message::InitializeSerialization(
				xercesc::DOMDocument & document)
{		
	DOMSerializedMessage Result(document.createElement (this->GetName()));
	return Result;
}

		
Message::ProcessImplBaseType Message::GetProcessImpl()
{
	return ProcessImpl->GetFunction();
}

ServerMessage::ServerMessage(size_t ID, ProcessImplType *_PI) : Message(ID, _PI)
{
	;
}

//void ServerMessage::Process(ControlThread &control)
//{	
//	Process(dynamic_cast<ClientControl&>(control));
//}

ClientMessage::ClientMessage(size_t ID, ProcessImplType *_PI) : Message(ID,_PI)
{
	;
}
//
//void ClientMessage::Process(ControlThread &control)
//{
//	Process(dynamic_cast<ServerControl&>(control));
//}
