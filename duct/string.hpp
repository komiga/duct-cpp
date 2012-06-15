/**
@file string.hpp
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

String types and utilities.

@defgroup string String types and utilities
*/

#ifndef DUCT_STRING_HPP_
#define DUCT_STRING_HPP_

#include "./config.hpp"

#include <string>

namespace duct {

/**
	@addtogroup string
	@{
*/

#ifdef DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI

/**
	Make string literal UTF-8 encoded.
	For supporting compilers, takes a string literal and makes it UTF-8 encoded; otherwise equivalent to @a x.
	@note This is <em>almost</em>, but not entirely guaranteed to make @a x UTF-8 encoded if #DUCT_USING_CPP11_CHAR_TYPES is defined.
	@param x String literal.
	@sa u8string
*/
#define DUCT_STR_U8(x)

#else

#if (DUCT_COMPILER & DUCT_FLAG_COMPILER_GCC) >= DUCT_FLAG_COMPILER_GCC46
	#define DUCT_STR_U8(x) u8 ## x
#else
	#define DUCT_STR_U8(x) x
#endif

#endif // DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI

/**
	UTF-8 string type.
	Implies UTF-8 encoded data.
	@note There is nothing special about this type; it is merely an @c std::string used as a convenient container.
*/
typedef std::string u8string;
/*
	UTF-32 string type.
*/
//typedef std::u32string u32string;

/** @} */ // end of doc-group string

} // namespace duct

#endif // DUCT_STRING_HPP_
