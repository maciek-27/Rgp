#ifndef __ALLMESSAGES__H__
#define __ALLMESSAGES__H__
#include "boost/mpl/list.hpp"

#include "server/hello.h++"
#include "server/authrefuse.h++"
#include "server/serverinfo.h++"
#include "server/list.h++"
#include "server/error.h++"
#include "server/acknowledgement.h++"

#include "client/hello.h++"
#include "client/auth.h++"
#include "client/cstreamrequest.h++"
#include "client/dstreamrequest.h++"
#include "client/chdir.h++"
#include "client/read.h++"
#include "client/write.h++"
#include "client/open.h++"
#include "client/seek.h++"
#include "client/close.h++"
#include "client/list.h++"
#include "client/createdirectory.h++"
#include "client/remove.h++"
namespace xftp
{
	namespace server
	{
		//! this is used while generating maps decoding XML blocks into
		//! message objects and vice versa
		typedef boost::mpl::list<
				MsgAcknowledgement,
				MsgAuthRefuse,
				MsgHello,
				MsgList,
				MsgError,
				MsgServerInfo
		> MessagesList;
	}
	namespace client
	{
		
		typedef boost::mpl::list<
				MsgHello,
				MsgAuth,
				MsgCStreamRequest,
				MsgDStreamRequest,
				MsgChDir,
				MsgRead,
				MsgWrite,
				MsgOpen,
				MsgSeek,
				MsgClose,
				MsgList,
				MsgCreateDirectory,
				MsgRemoveItem
		> MessagesList;
	}
}
#endif
