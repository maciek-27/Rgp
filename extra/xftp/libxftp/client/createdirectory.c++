#include "createdirectory.h++"
 
#include<boost/filesystem/operations.hpp>
#include<libxftp/commondomfeatures.h++>
#include <libxftp/client.h++>
#include <libxftp/servercontrol.h++>
#include <libxftp/server/error.h++>
#include <libxftp/server/acknowledgement.h++>
using namespace xftp;
using namespace client;
using namespace xercesc;

MsgCreateDirectory::MsgCreateDirectory(DOMElement *elem, size_t ID) : FilenameOperations(elem,ID, ProcessImplDefault(this))
{	   
}

MsgCreateDirectory::MsgCreateDirectory():FilenameOperations(ProcessImplDefault(this)){;}

XMLCh * MsgCreateDirectory::GetName()
{
	static XMLCh name[] = {chLatin_c, chLatin_r, chLatin_e, chLatin_a, chLatin_t
			, chLatin_e,chUnderscore, chLatin_d, chLatin_i, chLatin_r, chLatin_e
			, chLatin_c, chLatin_t, chLatin_o, chLatin_y , chNull};
	return name;
}

void MsgCreateDirectory::DefaultProcessImpl(ServerControl& control)
{
	
	try {
		boost::filesystem::path path = control.GetDir();
		path /=GetFilename();
		if (not boost::filesystem::exists(path))
			boost::filesystem::create_directory( path);
		else
		{
			server::MsgError * err = new server::MsgError;
			err-> SetPointer(GetID());
			err-> SetTextMsg(std::string("File does not exist "));
			control.xmlStream.PostMessage( std::tr1::shared_ptr<Message>(err),true);
			return;
		}
	}
	catch (std::exception & e){
		server::MsgError * err = new server::MsgError;
		err-> SetPointer(GetID());
		err-> SetTextMsg(std::string("Unexpected difficulity")+e.what());
		control.xmlStream.PostMessage( std::tr1::shared_ptr<Message>(err),true);
		return;
	}  
	server::MsgAcknowledgement * ack =
		new server::MsgAcknowledgement;
	ack-> SetPointer(GetID());
	control.xmlStream.PostMessage( std::tr1::shared_ptr<Message>(ack),true);
}
