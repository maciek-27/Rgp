//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2007-2008 Damian Kaczmarek, Maciej Kaminski
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AUTOLIST_H__
#define __AUTOLIST_H__

#include <list>
#include <tr1/unordered_map>
// #ifdef __GNUC__
// #define HASH_MAP_NAMESPACE __gnu_cxx
// #include <ext/hash_map>
// #else
// #include <hash_map>
// #define HASH_MAP_NAMESPACE std
// #endif

//namespace HASH_MAP_NAMESPACE {
//}

namespace Scr
{
/*!
  \brief container combining advantages of list and hash map,
  allowing

  It is implemented using standard STL list and almost_standard hash_map.
 */
template <class T>
class AutoList
{
public:
	typedef typename std::list<T>::const_reverse_iterator const_reverse_iterator;
	typedef typename std::list<T>::reverse_iterator reverse_iterator;
	typedef typename std::list<T>::const_iterator const_iterator;
	typedef typename std::list<T>::iterator iterator;
	typedef typename std::list<T>::size_type size_type;
private:
	template <class _T>
	struct _hash{
		size_t operator() (const _T x) const {
			return reinterpret_cast<size_t>(x);
		}
	};
	std::list<T> list;

//	HASH_MAP_NAMESPACE::
	typedef
	std::tr1::unordered_map<const T, iterator,_hash<T> > hashmap_t;
	hashmap_t hashmap;
	typedef typename hashmap_t
	::iterator hashiterator;

	size_type _size;
public:

	AutoList<T>() : list(), hashmap(), _size(0) { };

	/*!
	  \param elem element to find
	  \return list iterator to specific element
	 */
	iterator operator[](T &elem) {
		hashiterator temp = hashmap.find(elem);
		if(temp == hashmap.end())
			return list.end();
		return temp->second;
	}
	iterator operator[](const T &elem) {
		hashiterator temp = hashmap.find(elem);
		if(temp == hashmap.end())
			return list.end();
		return temp->second;
	}

	/*!
	  \return list iterator to lase element
	 */
	reverse_iterator rbegin() {
		return list.rbegin();
	}
	/*!
	  \return list iterator rend() of list
	 */
	reverse_iterator rend() {
		return list.rend();
	}
	/*!
	  \return list iterator to first element
	 */
	iterator begin() {
		return list.begin();
	}
	/*!
	  \return list iterator end() of list
	 */
	iterator end() {
		return list.end();
	}
	/*!
	  \return list iterator to first element
	 */
	const_iterator begin()const {
		return list.begin();
	}
	/*!
	  \return list iterator end() of list
	 */
	const_iterator end()const {
		return list.end();
	}
	/*!
	  \return last element in the list
	 */
	const T& back() {
		return (list.back());
	}
	/*!
	  \return number of elements
	 */
	size_type size() {
		return _size;
	}
	/*!
	  \return true if _size is 0
	 */
	bool empty() {
		return _size?false:true;
	}

	/*!
	  \param i list iterator to  specific element to be erased
	 */
	void erase(iterator i) {
		_size--;
		hashmap.key_erase(*i);
		list.erase(i);
	}

	/*!
	  \param elem  specific element to be erased
	 */
	void remove(T elem) {
		_size--;
		hashiterator i = hashmap.find(elem);
		list.erase(i->second);
		hashmap.erase(i);
	}

	/*!
	  \param before where to insert
	  \param newelem what to insert
	 */
	iterator insert(const T& before, const T& newelem) {
		_size++;
		return (hashmap[newelem] = list.insert(hashmap[before], newelem));
	}

	/*!
	  \param elem what to insert
	 */
	void push_front(const T& elem) {
		_size++;
		list.push_front(elem);
		hashmap[elem] = list.front();
	}
	/*!
	  \param elem what to insert
	 */
	void push_back(const T& elem) {
		_size++;
		list.push_back(elem);
		hashmap[elem] = --list.end();
	}
  void clear()
  {
    list.clear();
    hashmap.clear();
  }
	/*!
	  \param elem1 element to be swapped w/ elem2
	  \param elem2 element to be swapped w/ elem1

	  swaps two elements in the Autolist
	 */
	void swap(const T& elem1, const T& elem2) {
		hashiterator hashiter1 = hashmap.find(elem1),
			hashiter2 = hashmap.find(elem2);
		iterator iter1 = hashiter1->second;
		iterator iter2 = hashiter2->second;
		hashiter1->second = iter2;
		hashiter2->second = iter1;

		std::iter_swap(iter1, iter2);
	}
};
}
#endif // __AUTOLIST_H__
