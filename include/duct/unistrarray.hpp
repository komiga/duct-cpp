/**
@file unistrarray.hpp
@author Tim Howard

@section LICENSE
Copyright (c) 2010 Tim Howard

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

@section DESCRIPTION

duct++ UnicodeString* array class.
*/

#ifndef _DUCT_UNISTRARRAY_HPP
#define _DUCT_UNISTRARRAY_HPP

#include <stdarg.h>
#include <unicode/unistr.h>
#include <duct/config.hpp>
#include <duct/genericarray.hpp>

namespace duct {

/**
	UnicodeString* array.
	All data (except for C-string arguments and C-string data pointers) given to the class is owned by the class.
*/
class DUCT_API StringArray : public GPArray<UnicodeString*> {
public:
	/**
		Constructor.
	*/
	StringArray();
	/**
		Constructor with single C-string.
		@param str The string to initialize with.
	*/
	StringArray(const char* str);
	/**
		Constructor with C-string array.
		@param data The C-string array.
		@param size The size of the array. If -1, the array must be NULL-terminated.
	*/
	StringArray(const char** data, int size=-1);
	/**
		Destructor.
	*/
	~StringArray();
	/**
		Set the array data to the given C-string (single element).
		@returns Nothing.
		@param str The string to use.
	*/
	void set(const char* str);
	/**
		Set the array data from the given C-string array.
		@returns Nothing.
		@param data The C-string array.
		@param size The size of the array. If -1, the array must be NULL-terminated.
	*/
	void set(const char** data, int size=-1);
	/**
		Set the array data from the const char* arguments in the given va_list.
		@returns Nothing.
		@param num The number of arguments in ap.
		@param ap The variadic argument list.
	*/
	void setVLCStrings(unsigned int num, va_list ap);
	/**
		Set the array data from the given const char* arguments.
		@returns Nothing.
		@param num The number of variadic arguments.
		@param ... The variadic arguments.
	*/
	void setVCStrings(unsigned int num, ...);
	/**
		Create a new array with the given const char* arguments.
		@returns The new array.
		@param num The number of variadic arguments.
		@param ... The variadic arguments.
	*/
	static StringArray* withCStrings(unsigned int num, ...);
};

} // namespace duct

#endif // _DUCT_UNISTRARRAY_HPP

