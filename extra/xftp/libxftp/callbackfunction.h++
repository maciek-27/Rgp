#ifndef __XFTP__CALLBACK_FUNCTION_H__
#define __XFTP__CALLBACK_FUNCTION_H__

#include <boost/mpl/vector.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/function/function_base.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_of.hpp> 

#include <boost/preprocessor/iterate.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/comparison/not_equal.hpp>
#include <boost/preprocessor/comparison/not_equal.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/control/expr_if.hpp>
#include <boost/thread/detail/singleton.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/int.hpp>
#include <memory>
#include <boost/function.hpp>
namespace xftp
{
	//may be set to BOOST_FUNCTION_MAX_ARGS

	// Real maximum of callback args number is
	// XFTP_CALLBACK_MAX_ARGS-1
#define XFTP_CALLBACK_MAX_ARGS 3

	
	// protected function container
	template <typename BF>
    class ProtectedFunction{
	BOOST_STATIC_ASSERT( (boost::is_base_of<boost::function_base,BF>::value) );
	protected:
		BF * f;		
	public:
		ProtectedFunction(): f(0) {}
		virtual ~ProtectedFunction(){}
		virtual BF SetFunction(BF _f) = 0;
		virtual BF GetFunction() = 0;
	};
	
	
	// forbidden class
	template <typename BF, int NA>
	class CallbackFunctionN
	{
	private:
		CallbackFunctionN ();
	}		;
	
											
#define F_ARG_T(z, n, text) typename BF::\
	BOOST_PP_EXPR_IF(BOOST_PP_NOT_EQUAL(n,0), arg##n##_type) \
	BOOST_PP_EXPR_IF(BOOST_PP_EQUAL(n,0), result_type)
	
#define F_ARG_D(z, n, text) typename \
boost::mpl::at<arg_type,boost::mpl::int_<n+1> >::type a##n

#define F_ARG_L(z, n, text) a##n


#define CallbackFunctionNMacro(z, n, data)					\
    template <typename BF>									\
    class CallbackFunctionN<BF,n>:							\
	public ProtectedFunction<BF>							\
	{														\
		using ProtectedFunction<BF>::f;						\
	protected:												\
        typedef boost::mpl::vector<							\
        BOOST_PP_ENUM_ ## z(BOOST_PP_INC(n), F_ARG_T, foo)  \
                      > arg_type;                           \
    public:													\
	typename BF::result_type								\
		operator() ( BOOST_PP_ENUM_ ## z(n,F_ARG_D,data) )	\
        {                                                   \
            (*f)(BOOST_PP_ENUM_ ## z(n,F_ARG_L,data));		\
        };													\
    };
	
	// last parameter unused;
BOOST_PP_REPEAT(3, CallbackFunctionNMacro, _)

#undef F_ARG_D
#undef F_ARG_L
#undef F_ARG_T
	template <typename BF, typename CallbackClass>
	class SingletonContainer
	{
	public:
		BF f;
	};
	
	template <typename BF>
    class CallbackFunctionBase: public  CallbackFunctionN<BF, BF::arity>
	{
	public:
		CallbackFunctionBase(){};
		virtual ~CallbackFunctionBase(){}
	};
	
	template <typename BF, typename CallbackClass = void>
	class CallbackFunction :  public CallbackFunctionBase<BF>
	{
		using ProtectedFunction<BF>::f;
	public:
		CallbackFunction()
		{
			f=&
				boost::detail::thread::singleton
					<SingletonContainer<BF,CallbackClass> >::instance ().f;
		}
		
		BF SetFunction(BF _f)
		{
			swap(_f,*f);
			return _f;
		}
		
		BF GetFunction()
		{
			return *f;
		}
		
		virtual ~CallbackFunction() {};
	};
}

#endif
