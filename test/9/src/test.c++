#include<rexio/tk/toolkit.h++>
#include<iostream>
#include <fstream>
#include <sstream>
#include <rexio/glyphwidth.h++>
#include <sys/time.h>

using namespace std;
using namespace Scr;

/*! Simple profiling class. After going out of scope, prints elapsed time */
class Profile
{
private:
	std::string name;
	struct timeval tv1;
public:
	/*!
	  \param name label to print near elapsed time at going out of scope
	*/
	Profile(const std::string& _name)
		{
			name = _name;
			gettimeofday(&tv1, NULL);
		}
	~Profile()
		{
			struct timeval tv2;
			gettimeofday(&tv2, NULL);
			std::cout << name << ": " << (tv2.tv_sec - tv1.tv_sec)*1000 + (tv2.tv_usec - tv1.tv_usec)/1000
							  << "ms" << std::endl;
		}

};

int main (int argc,char ** argv)
{
	//static wchar_t bla[] = L"こんいちはhallo, źdźbłoقناة عربية إخبارية تعمل عjkhdsjkshdsdkjhksjdhsdjkhsdjkhsdkjلى مدار اليوم. يمكنك مشاهدة بث القناة من خلال الموقع.djskhsdhd";

/*	static int b = 0;
	for(Uint k = 0;k<100000;k++)
		for(Uint i =0;i < (sizeof(bla)/sizeof(wchar_t));++i)
			b += Scr::width(bla[i]);

			std::cout << b << "\n";*/
	

	int from = 0;
	int last = 0;
	wchar_t i;
	int sum = 0;

	sum = 0;
	for(i = 1;i<1000000;++i) {
		int neww = -1;
		try {
			neww = GlyphWidth::Get(i);
		}
		catch (...) {;}
		sum += neww;
		if(neww != last) {
			std::cout << last << " from " << from << " to " << i << "\n";
			from = i;
			last = neww;
		}
	}
	
	{
		Profile a("Optimized run - 100 x 1000000 symbols");

		for(int j = 0;j<100;++j) {
			for(i = 1;i<1000000;++i) {
				sum += GlyphWidth::Get(i);
			}
		}
		if(sum != 1172055*100)
			std::cerr << sum <<" Bad result!\n";
		sum = 0;
	}
	{
		Profile a("Unoptimized run - 100 x 1000000 symbols");

		for(int j = 0;j<100;++j) {
			sum = 0;
			for(i = 1;i<1000000;++i) {
				sum += Scr::width(i);;
			}		
		}
		sum = 0;
	}
	{
		Profile a("Optimized run - 1526 x 65536 symbols");

		for(int j = 0;j<1526;++j) {
			for(i = 1;i<65536;++i) {
				sum += GlyphWidth::Get(i);;
			}
		}
		if(sum != 163138556)
			std::cerr << sum <<" Bad result!\n";
		sum = 0;
	}
	{
		Profile a("Unoptimized run - 1526 x 65536 symbols");

		for(int j = 0;j<1526;++j) {
			for(i = 1;i<65536;++i) {
				sum += Scr::width(i);
			}
		}
		sum = 0;
	}
	return 0;
}
/*end of main function of program*/ 
