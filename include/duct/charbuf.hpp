/**
@file charbuf.hpp
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

duct++ CharBuf class.
*/

#ifndef _DUCT_CHARBUF_HPP
#define _DUCT_CHARBUF_HPP

#include <duct/config.hpp>
#include <unicode/unistr.h>

namespace duct {

class DUCT_API CharBuf {
public:
	/**
		Constructor.
	*/
	CharBuf();
	/**
		Destructor.
	*/
	~CharBuf();
	/**
		Add a character to the buffer.
		Adding a character from the buffer will cause the cached string to be reset.
		@returns Nothing.
		@param c The character to add.
	*/
	void addChar(UChar32 c);
	/**
		Cache the character buffer as a string.
		@returns The cached string.
	*/
	const UnicodeString& cacheString();
	/**
		Reset the buffer.
		The internal memory buffer will not be freed, but the position and cached string will be reset.
		@returns Nothing.
	*/
	void reset();
	/**
		Convert the buffer to a string.
		@returns Nothing.
		@param str The string to store the result in.
	*/
	void asString(UnicodeString& str);
	/**
		Convert the buffer to a string.
		@returns A reference to the cached string.
	*/
	const UnicodeString& asString();
	
protected:
	UChar32* _buffer;
	size_t _bufsize;
	size_t _buflength;
	UnicodeString _bufstring;
	bool _cached;
};

} // namespace duct

#endif // _DUCT_CHARBUF_HPP

