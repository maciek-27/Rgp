#ifndef __SUM__ITERATOR__H__
#define __SUM__ITERATOR__H__
#include <algorithm>
#include <iterator>
namespace MUD { namespace Aux{
template<typename _Tp=int>
class sum_iterator : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
	_Tp sum;
public:
	sum_iterator(_Tp s = 0) : sum(s) {}
	sum_iterator& operator=(const _Tp& __value){sum += __value; return *this;}
	_Tp get_sum() {return sum;}
	sum_iterator& operator*(){ return *this; }
	sum_iterator& operator++(){ return *this; }
	sum_iterator& operator++(int){ return *this; }
};

template<typename _Tp=int>
class maximum_iterator : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
	_Tp maximum;
public:
	maximum_iterator(_Tp s = 0) : maximum(s) {}
	maximum_iterator& operator=(const _Tp& __value){maximum = std::max(maximum, __value); return *this;}
	_Tp get_maximum() {return maximum;}
	maximum_iterator& operator*(){ return *this; }
	maximum_iterator& operator++(){ return *this; }
	maximum_iterator& operator++(int){ return *this; }
};

}}
#endif
