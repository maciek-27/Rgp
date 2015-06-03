#include<libxftp/zipfilters.h++>
#include<zlib.h>

#include <iostream>
using std::size_t;
using namespace xftp::stdfilter;




size_t xftp::stdfilter::ZCompress(boost::scoped_array<XMLByte>& b,size_t size)
{
	using namespace std;
	uLong repl_size = size/4;
	boost::scoped_array<XMLByte> replacement (  new XMLByte[repl_size] );
	int result;
	while 
	((result = 
		compress ( replacement.get(), &repl_size, 
				   b.get(),size)) == Z_BUF_ERROR )
	{
		repl_size*=2;
		replacement.reset(new XMLByte[repl_size]);
	}
	assert(result==Z_OK);	
	b.swap(replacement);

	cout << "Compressed data " << size << ":" << repl_size << endl;
	return repl_size;
}

size_t xftp::stdfilter::UnZCompress(boost::scoped_array<XMLByte>& b, size_t size)
{
	using namespace std;
	uLong repl_size = size;
	boost::scoped_array<XMLByte> replacement (  new XMLByte[repl_size] );
	int result;
	while 
	((result = 
		uncompress ( replacement.get(), &repl_size, 
				   b.get(),size)) == Z_BUF_ERROR )
	{
		repl_size*=2;
		replacement.reset(new XMLByte[repl_size]);
	}
	assert(result==Z_OK);	
	
	b.swap(replacement);
	cout << "Decompressed data " << size << ":" << repl_size << endl;
	return repl_size;
	
}


size_t xftp::stdfilter::FilterChain::operator ()(boost::scoped_array<XMLByte>& 
												 b, size_t size)
{
	
	for ( std::list<XMLStream::FilterFunc>::iterator i = filters.begin();
		  i!= filters.end() ; ++i)
	{
		size= i->operator() (b,size);
	}
	return size;

}

void xftp::stdfilter::FilterChain:: AddFilter(XMLStream::FilterFunc f)
{
	filters.push_back(f);
}
