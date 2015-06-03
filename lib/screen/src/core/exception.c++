#include "commons.h++"
#include <iostream>

using namespace Scr;
Exception::Exception(std::string _m)throw()
    :std::exception()    
{
	using namespace std;
	RexIOLog(LogLevelLow) << "Exception occurred. Message:\n " << _m << "\n";
    message=std::tr1::shared_ptr<std::string>(new std::string(_m));
}

Exception::Exception(const Exception& _base)throw()
    :std::exception()
	,message(_base.message)
{
    ;
}

const char* Exception::what() const throw()
{
    return message->c_str();
}

Exception::~Exception()throw()
{
    ;
}
