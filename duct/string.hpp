/**
@file string.hpp
@brief String types and utilities.

@author Timothy Howard
@copyright 2010-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_STRING_HPP_
#define DUCT_STRING_HPP_

#include "./config.hpp"
#include "./aux.hpp"

namespace duct {

/**
	@addtogroup text
	@{
*/

#ifdef DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI

/**
	Make string literal UTF-8 encoded.

	For supporting compilers, takes a string literal and makes it UTF-8
	encoded; otherwise equivalent to @a x_.

	@note This is <em>almost</em> - but not entirely - guaranteed to
	make @a x_ UTF-8 encoded if #DUCT_USING_CPP11_CHAR_TYPES is defined.

	@param x_ String literal.
*/
#define DUCT_STR_U8(x_)

#else

#if false													\
	|| ((DUCT_COMPILER &  DUCT_FLAG_COMPILER_GCC) &&		\
		 DUCT_COMPILER >= DUCT_FLAG_COMPILER_GCC46)			\
	|| ((DUCT_COMPILER &  DUCT_FLAG_COMPILER_CLANG) &&		\
		 DUCT_COMPILER >= DUCT_FLAG_COMPILER_CLANG31)
//
	#define DUCT_STR_U8(x_) u8 ## x_
#else
	#define DUCT_STR_U8(x_) x_
#endif

#endif // DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI

/**
	UTF-8 string type.

	Implies UTF-8 encoded data.

	@note There is nothing special about this type; it is merely
	a @c std::basic_string<char, ...> used as a convenient container.
*/
using u8string = duct::aux::basic_string<char>;

/** @} */ // end of doc-group text

} // namespace duct

#endif // DUCT_STRING_HPP_
