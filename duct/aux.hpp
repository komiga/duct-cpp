/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Auxiliary stdlib specializations.
*/

#pragma once

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
template<class T>
using vector = std::vector<T, DUCT_CONFIG_ALLOCATOR<T>>;

/**
	Alias for @c std::deque<T> using the configured allocator.
*/
template<class T>
using deque = std::deque<T, DUCT_CONFIG_ALLOCATOR<T>>;

/**
	Alias for @c std::basic_string<CharT, TraitsT>
	using the configured allocator.
*/
template<
	class CharT,
	class TraitsT = std::char_traits<CharT>
>
using basic_string = std::basic_string<
	CharT, TraitsT, DUCT_CONFIG_ALLOCATOR<CharT>>;

/**
	Alias for @c std::basic_istringstream<CharT, TraitsT>
	using the configured allocator.
*/
template<
	class CharT,
	class TraitsT = std::char_traits<CharT>
>
using basic_istringstream = std::basic_istringstream<
	CharT, TraitsT, DUCT_CONFIG_ALLOCATOR<CharT>
>;

/**
	Alias for @c std::basic_ostringstream<CharT, TraitsT>
	using the configured allocator.
*/
template<
	class CharT,
	class TraitsT = std::char_traits<CharT>
>
using basic_ostringstream = std::basic_ostringstream<
	CharT, TraitsT, DUCT_CONFIG_ALLOCATOR<CharT>
>;

/**
	Alias for @c std::basic_stringstream<CharT, TraitsT>
	using the configured allocator.
*/
template<
	class CharT,
	class TraitsT = std::char_traits<CharT>
>
using basic_stringstream = std::basic_stringstream<
	CharT, TraitsT, DUCT_CONFIG_ALLOCATOR<CharT>
>;

/** Alias for @c basic_istringstream<char>. */
using istringstream = basic_istringstream<char>;
/** Alias for @c basic_ostringstream<char>. */
using ostringstream = basic_ostringstream<char>;
/** Alias for @c basic_stringstream<char>. */
using stringstream  = basic_stringstream<char>;

/** @} */ // end of doc-group aux

} // namespace aux
} // namespace duct
