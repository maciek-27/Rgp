
#include "supercontrol.h++"
#include <cstdlib>
#include <iostream>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
void  xftpclient::SuperControl::LList (Tokenizer &	 optns)
{
	
	using namespace std;
	using namespace boost::lambda;
	string path (".");
	int recurse=0;
	Tokenizer::iterator i =optns.begin ();
	if (i!=optns.end ())
	{
		path.assign (*i);
		if (++i!=optns.end ())
			recurse=atoi (i->c_str ());
	}
	xftp::server::MsgList list ;
	list.Fill (path, recurse);
	PrintNode (list.GetNode ());
	
}
