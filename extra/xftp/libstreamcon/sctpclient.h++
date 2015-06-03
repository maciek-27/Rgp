#ifndef _SCTPCLIENT_H_
#define _SCTPCLIENT_H_

#include "sctpconnection.h++"
#include "client.h++"

namespace streamcon {
	/*!
	  Class for creating client application classes basing on the
	  SCTP protocol.
	*/
	class SCTPClient : public ClientApp<SCTPConnection>
	{
	public:
	};
}


#endif // _SCTPCLIENT_H_
