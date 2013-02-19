/**
@file aux.hpp
@brief Auxiliary stdlib specializations.

@author Tim Howard
@copyright 2010-2012 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_AUX_HPP_
#define DUCT_AUX_HPP_

#include "./config.hpp"

#include <vector>
#include <deque>
#include <string>
#include <sstream>

namespace duct {
namespace aux {

/**
	@addtogroup aux
	@{
*/

/**
	Alias for @c std::vector<T> using the configured allocator.
*/
template<
	typename T
>
using vector=std::vector<T, DUCT_CONFIG_ALLOCATOR<T> >;

/**
	Alias for @c std::deque<T> using the configured allocator.
*/
template<
	typename T
>
using deque=std::deque<T, DUCT_CONFIG_ALLOCATOR<T> >;

/**
	Alias for @c std::basic_string<CharT, TraitsT> using the configured allocator.
*/
template<
	typename CharT,
	class TraitsT=std::char_traits<CharT>
>
using basic_string=std::basic_string<CharT, TraitsT, DUCT_CONFIG_ALLOCATOR<CharT> >;

/**
	Alias for @c std::basic_istringstream<CharT, TraitsT> using the configured allocator.
*/
template<
	typename CharT,
	class TraitsT=std::char_traits<CharT>
>
using basic_istringstream=std::basic_istringstream<CharT, TraitsT, DUCT_CONFIG_ALLOCATOR<CharT> >;

/**
	Alias for @c std::basic_ostringstream<CharT, TraitsT> using the configured allocator.
*/
template<
	typename CharT,
	class TraitsT=std::char_traits<CharT>
>
using basic_ostringstream=std::basic_ostringstream<CharT, TraitsT, DUCT_CONFIG_ALLOCATOR<CharT> >;

/**
	Alias for @c std::basic_stringstream<CharT, TraitsT> using the configured allocator.
*/
template<
	typename CharT,
	class TraitsT=std::char_traits<CharT>
>
using basic_stringstream=std::basic_stringstream<CharT, TraitsT, DUCT_CONFIG_ALLOCATOR<CharT> >;

/**	Alias for @c basic_istringstream<char>. */
typedef basic_istringstream<char> istringstream;
/**	Alias for @c basic_ostringstream<char>. */
typedef basic_ostringstream<char> ostringstream;
/**	Alias for @c basic_stringstream<char>. */
typedef basic_stringstream<char> stringstream;

/** @} */ // end of doc-group aux

} // namespace aux
} // namespace duct

#endif // DUCT_AUX_HPP_
