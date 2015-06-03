#include <iostream>
#include "libxftp/xmlstream.h++"
#include "libxftp/message.h++"
#include "libxftp/server/hello.h++"
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include "libxftp/commondomfeatures.h++"
#include <boost/thread/detail/singleton.hpp>
#include "libxftp/xercesstrings.h++"
using namespace std;
//simple test of CommonDOMFeatures as boost singleton 
int main ()
{
	
	{
		cout << "{";
		xftp::XMLStream xs(0),xs2(0),xs3(0);
	}
	cout << "}";
	xftp::XMLStream xs(0);
	cout << "Deserializing messages" << endl; 
	std::tr1::shared_ptr<xftp::Message> M1= xs.GetMessage();
	std::tr1::shared_ptr<xftp::Message> M2= xs.GetMessage();
	std::tr1::shared_ptr<xftp::Message> M3= xs.GetMessage();
	std::tr1::shared_ptr<xftp::Message> M4= xs.GetMessage();
	xftp::server::MsgHello * h= 
			reinterpret_cast<xftp::server::MsgHello*>(M1.get());
	cout << "server name is " << h->GetServerName() << endl;
			
	cout << "Serializing them as one sequence"<< endl;
	xs.PostMessage(M1,false);
	xs.PostMessage(M2,false);
	xs.PostMessage(M3,false);
	xs.PostMessage(M4, true);
	
	cout  << "Serializing them separately"<< endl;
	xs.PostMessage(M1);
	xs.PostMessage(M2);
	xs.PostMessage(M3);
	xs.PostMessage(M4);
}
