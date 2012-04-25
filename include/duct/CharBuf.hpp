/**
@file CharBuf.hpp
@author Tim Howard

@section LICENSE
Copyright (c) 2010-2012 Tim Howard

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

#ifndef DUCT_CHARBUF_HPP_
#define DUCT_CHARBUF_HPP_

#include <duct/config.hpp>
#include <duct/CharacterSet.hpp>

#include <unicode/unistr.h>

namespace duct {

// Forward declarations
class CharBuf;

/**
	Character buffer.
*/
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
	icu::UnicodeString const& cacheString();
	/**
		Reset the buffer.
		The internal memory buffer will not be freed, but the position and cached string will be reset.
		@returns Nothing.
	*/
	void reset();
	/**
		Compare all the characters in the buffer to the given character.
		@returns true if all characters match the given character, or false otherwise.
		@param c The character to compare against.
	*/
	bool compare(UChar32 c) const;
	/**
		Compare all the characters in the buffer with the given character set.
		@returns true if all characters match a character in the set, or false otherwise.
		@param charset The character set to compare against.
	*/
	bool compare(CharacterSet const& charset) const;
	/**
		Convert the buffer to a string.
		If conversion fails, <em>str</em> is unmodified.
		@returns true if the string was converted, or false on cache failure (likely because of an invalid surrogate pair).
		@param str The string to store the result in.
	*/
	bool toString(icu::UnicodeString& str);
	/**
		Convert the buffer to a string.
		The string returned is only a snapshot of the buffer's current state, and will be emptied upon buffer reset or caching a new buffer state, or bogus'd on cache failure.
		@returns A reference to the cached string (which will be bogus if conversion failed).
	*/
	icu::UnicodeString const& toString();
	/**
		Convert the buffer to a 32-bit integer.
		@returns The buffer as an integer, or 0 if the cached string was not a numeric value.
	*/
	int32_t toInt();
	/**
		Convert the buffer to a 32-bit integer (with error-return).
		If the conversion fails, <em>value</em> is unmodified.
		@returns true if the cached string was a numeric value (<em>value</em> is set), or false otherwise.
		@param value Output value.
	*/
	bool toInt(int32_t& value);
	/**
		Convert the buffer to a 64-bit integer.
		@returns The buffer as a long, or 0 if the cached string was not a numeric value.
	*/
	int64_t toLong();
	/**
		Convert the buffer to a long (with error-return).
		If the conversion fails, <em>value</em> is unmodified.
		@returns true if the cached string was a numeric value (<em>value</em> is set), or false otherwise.
		@param value Output value.
	*/
	bool toLong(int64_t& value);
	/**
		Convert the buffer to a float.
		@returns The buffer as a float, or 0.0 if the cached string was not a numeric value.
	*/
	float toFloat();
	/**
		Convert the buffer to a float (with error-return).
		If the conversion fails, <em>value</em> is unmodified.
		@returns true if the cached string was a numeric value (<em>value</em> is set), or false otherwise.
		@param value Output value.
	*/
	bool toFloat(float& value);
	/**
		Convert the buffer to a double.
		@returns The buffer as a double, or 0.0 if the cached string was not a numeric value.
	*/
	double toDouble();
	/**
		Convert the buffer to a double (with error-return).
		If the conversion fails, <em>value</em> is unmodified.
		@returns true if the cached string was a numeric value (<em>value</em> is set), or false otherwise.
		@param value Output value.
	*/
	bool toDouble(double& value);
	
protected:
	UChar32* m_buffer;
	size_t m_bufsize;
	size_t m_buflength;
	icu::UnicodeString m_bufstring;
	bool m_cached;
};

} // namespace duct

#endif // DUCT_CHARBUF_HPP_
