#include "commons.h++"
#include <iostream>

using namespace streamcon;

void Time(const std::string& name)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	std::cout << name << ": " << (tv.tv_sec%10) * 1000 +(tv.tv_usec)/1000
			  << "ms" << std::endl;
	
}

Exception::Exception(std::string _m) throw()
    :std::exception()    
{
	using namespace std;
	std::cerr << "Exception occurred. Message:\n " << _m << "\n";
    message=std::tr1::shared_ptr<std::string>(new std::string(_m));
}

Exception::Exception(const Exception& _base) throw()
    :std::exception()
	,message(_base.message)
{
    ;
}

const char* Exception::what() const throw()
{
    return message->c_str();
}

Exception::~Exception() throw()
{
    ;
}
