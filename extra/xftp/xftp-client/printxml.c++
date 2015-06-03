#include "printxml.h++"
#include <iostream>
using namespace xftpclient;

bool PrintXML::doPrintXML=false;

size_t  PrintXML::operator ()(boost::scoped_array<XMLByte>& b, size_t s)
{
	if (! doPrintXML)
		return s;
	if (s<800)
	{
		std::cout << "\x1b["<< c <<"m";
		std::cout.write (reinterpret_cast<const char *>(b.get ()), s);
		std::cout<< "\x1b[0m" << std::endl;
		
	}
	else
	{
		XMLByte t = b[700];
		b[700]=0;
		std::cout << "\x1b["<< c <<"m" << b.get ()
		<< "\n(...)\n"
		<< "\x1b[0m" << std::endl;
		b[700]=t;
	}
	return s;
}
