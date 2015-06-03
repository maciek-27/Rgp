#ifndef _CLIENTCONTROL_H_
#define _CLIENTCONTROL_H_

#include "xftpcommons.h++"

namespace xftp{
	class ClientControl : public ControlThread
	{
	public:
		ClientControl(ClientBase& client, 
					  streamcon::Stream& _stream) :
			ControlThread(client, _stream) {;}	
	};
}

#endif // _CLIENTCONTROL_H_
