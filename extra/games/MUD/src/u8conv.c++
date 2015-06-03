#include <string>
#include <algorithm>
#include <sstream>
#include "u8conv.h++"
#include <rexio/commons.h++>
namespace Scr{void EncodeUTF8(std::ostream & o, Uint c)throw();}
using namespace std;
string MUD::Aux::to_u8(wstring text)
{
	stringstream ss1;
	for (wstring::iterator i = text.begin() ; i!=text.end() ; ++i)
		Scr::EncodeUTF8(ss1,*i);
	string str;
	ss1 >> str;
	return str;
	
}
