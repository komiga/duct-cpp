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

#include <unicode/unistr.h>
#include <duct/config.hpp>

namespace duct {

/**
	UnicodeString* array.
	All data (except for C-string arguments and C-string data pointers) given to the class is owned by the class.
*/
class StringArray {
public:
	/**
		Constructor.
	*/
	StringArray();
	/**
		Constructor with C-string array.
		@param data The C-string array.
		@param size The size of the array. If -1, the array must be NULL-terminated.
	*/
	StringArray(const char** data, int size=-1);
	/**
		Constructor with UnicodeString array.
		The given array data is not copied - the StringArray owns the given data pointer (barring the container itself, when static).
		@param data The UnicodeString array.
		@param size The size of the array. If -1, the array must be NULL-terminated.
		@param releasecontainer Whether to free the given container. This is false by default in the assumption that a static array will be passed. However, the pointers in the array data will still be freed (does not copy strings).
	*/
	StringArray(UnicodeString** data, int size=-1, bool releasecontainer=false);
	/**
		Destructor.
	*/
	~StringArray();
	/**
		Get the array's size.
		@returns The array's size.
	*/
	unsigned int getSize() const;
	/**
		Get the array's data.
		Don't free this.
		@returns The array's data.
		@see getSize()
	*/
	UnicodeString** get();
	/**
		Release the string's data.
		@returns Nothing.
	*/
	void release();
	/**
		Set the array data.
		The given array data is not copied - the StringArray owns the given data pointer (barring the container itself, when static).
		@returns Nothing.
		@param data The UnicodeString array.
		@param size The size of the array. If -1, the array must be NULL-terminated.
		@param releasecontainer Whether to free the given container. This is false by default in the assumption that a static array will be passed. However, the pointers in the array data will still be freed (does not copy strings).
	*/
	void set(UnicodeString** data, int size=-1, bool releasecontainer=false);
	/**
		Set the array data from the given C-string array.
		@returns Nothing.
		@param data The C-string array.
		@param size The size of the array. If -1, the array must be NULL-terminated.
	*/
	void setFromCStrings(const char** data, int size=-1);
	/**
		Set the array data from the const char* arguments in the given va_list.
		@returns Nothing.
		@param num The number of arguments in ap.
		@param ap The variadic argument list.
	*/
	void setFromVCStrings(unsigned int num, va_list ap);
	/**
		Set the array data from the given const char* arguments.
		@returns Nothing.
		@param num The number of variadic arguments.
		@param ... The variadic arguments.
	*/
	void setFromVCStrings(unsigned int num, ...);
	/**
		Set the array data from the UnicodeString* arguments in the given va_list.
		The StringArray takes ownership of the given UnicodeString pointers (no copy).
		@returns Nothing.
		@param num The number of arguments in ap.
		@param ap The variadic argument list.
	*/
	void setFromVUStrings(unsigned int num, va_list ap);
	/**
		Set the array data from the UnicodeString* arguments in the given va_list.
		The StringArray takes ownership of the given UnicodeString pointers (no copy).
		@returns Nothing.
		@param num The number of variadic arguments.
		@param ... The variadic arguments.
	*/
	void setFromVUStrings(unsigned int num, ...);
	/**
		Element access.
		The given index is not bounds-checked.
		@returns The UnicodeString pointer at the given index.
		@param index The string index.
	*/
	UnicodeString* operator[](const int index);
	const UnicodeString* operator[](const int index) const;
	/**
		Create a new array with the given const char* arguments.
		@returns The new array.
		@param num The number of variadic arguments.
		@param ... The variadic arguments.
	*/
	static StringArray* withVCStrings(unsigned int num, ...);
	/**
		Create a new array with the given UnicodeString* arguments.
		@returns The new array.
		@param num The number of variadic arguments.
		@param ... The variadic arguments.
	*/
	static StringArray* withVUStrings(unsigned int num, ...);
	
protected:
	unsigned int _size;
	UnicodeString** _data;
	bool _releasecontainer;
};

} // namespace duct

#endif // _DUCT_UNISTRARRAY_HPP

