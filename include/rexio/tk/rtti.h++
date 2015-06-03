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

/*! \file rtti.h++
  RTTI - Run Time Type Information
  This macros can expand a class to have custom RTTI capabilities.
*/

#ifndef __RTTI_H__
#define __RTTI_H__

#include <vector>
#include <string>

#define RTTI_BASE(__name) \
public: \
typedef std::vector<std::string> ClassHierarchy; \
protected: \
ClassHierarchy classHierarchy; \
public: \
virtual bool IsTypeOf(std::string _className) const \
{ \
	if(#__name == _className) \
		return true; \
	else \
		return false; \
}\
virtual const char * TypeName() const \
{ \
	return #__name; \
}\
virtual const char * ParentName() const \
{ \
	return NULL; \
}\
const ClassHierarchy& Hierarchy() \
{ \
	if(!classHierarchy.size()) \
		Hierarchy(classHierarchy); \
	return classHierarchy; \
} \
protected: \
virtual void Hierarchy(ClassHierarchy &vec) \
{ \
	vec.push_back(#__name); \
}

#define RTTI_OBJ(__name, __parent)\
public: \
virtual bool IsTypeOf(std::string _className) const \
{ \
	if(#__name == _className) \
		return true; \
	return __parent::IsTypeOf(_className); \
} \
virtual const char * TypeName() const \
{ \
	return #__name; \
}\
virtual const char * ParentName() const \
{ \
	return #__parent; \
}\
virtual void Hierarchy(ClassHierarchy &vec) \
{ \
	vec.push_back(#__name);\
	if(ParentName()) \
		__parent::Hierarchy(vec); \
}

#define RTTI_OBJ2(__name, __parent1, __parent2) \
public: \
virtual bool IsTypeOf(std::string _className) const \
{ \
	if(#__name == _className) \
		return true; \
	return __parent1::IsTypeOf(_className) || __parent2::IsTypeOf(_className); \
} \
virtual const char * TypeName() const \
{ \
	return #__name; \
} \
virtual const char * ParentName() const \
{ \
	return #__parent1; \
} \
virtual void Hierarchy(ClassHierarchy &vec) \
{ \
	vec.push_back(#__name); \
	if(ParentName()) \
		__parent1::Hierarchy(vec); \
}

#endif /* __RTTI_H__ */
