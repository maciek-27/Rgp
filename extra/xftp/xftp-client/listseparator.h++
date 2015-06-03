#ifndef ___XFTPCLIENT__REAL_APP_LISTSEPARATOR__
#define ___XFTPCLIENT__REAL_APP_LISTSEPARATOR__

namespace xftpclient
{
	
#if !defined(BOOST_MSVC) || BOOST_MSVC > 1300
	template <class Char,
	class Traits = typename std::basic_string<Char>::traits_type >
#else
	template <class Char,
	class Traits = std::basic_string<Char>::traits_type >
#endif
	class ListSeparator
	{
	private:
		boost::escaped_list_separator<Char, Traits> bs;
		typedef std::basic_string<Char, Traits> string_type;
	public:
		explicit ListSeparator (Char  e = '\\',  Char c = ',', Char  q = '\"')
		: bs (e, c, q)
		{}
		
		ListSeparator (string_type e, string_type c, string_type q)
		:bs (e, c, q)
		{ }
		
		void reset ()
		{bs.reset ();}
		
		template <typename InputIterator, typename Token>
		bool operator()(InputIterator& next, InputIterator end, Token& tok)
		{
			bool result;
			while ((result=bs (next, end, tok)) and (tok.empty ()))/*do nothing*/;
			return result;
		}
	};
	
}
#endif
