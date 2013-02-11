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
	Template for @c std::vector using the configured allocator.
*/
template<typename T>
struct vector {
	/** The full type. */
	typedef std::vector<T, DUCT_CONFIG_ALLOCATOR<T> > type;
};

/**
	Template for @c std::deque using the configured allocator.
*/
template<typename T>
struct deque {
	/** The full type. */
	typedef std::deque<T, DUCT_CONFIG_ALLOCATOR<T> > type;
};

/**
	Template for @c std::basic_string using the configured allocator.
*/
template<typename charT, class traitsT=std::char_traits<charT> >
struct basic_string {
	/** The full type. */
	typedef std::basic_string<charT, traitsT, DUCT_CONFIG_ALLOCATOR<charT> > type;
};

/**
	Template for @c std::basic_istringstream using the configured allocator.
*/
template<typename charT, class traitsT=std::char_traits<charT> >
struct basic_istringstream {
	/** The full type. */
	typedef std::basic_istringstream<charT, traitsT, DUCT_CONFIG_ALLOCATOR<charT> > type;
};

/**
	Template for @c std::basic_ostringstream using the configured allocator.
*/
template<typename charT, class traitsT=std::char_traits<charT> >
struct basic_ostringstream {
	/** The full type. */
	typedef std::basic_ostringstream<charT, traitsT, DUCT_CONFIG_ALLOCATOR<charT> > type;
};

/**
	Template for @c std::basic_stringstream using the configured allocator.
*/
template<typename charT, class traitsT=std::char_traits<charT> >
struct basic_stringstream {
	/** The full type. */
	typedef std::basic_stringstream<charT, traitsT, DUCT_CONFIG_ALLOCATOR<charT> > type;
};

/**	Alias for @c basic_istringstream<char>. */
typedef basic_istringstream<char>::type istringstream;
/**	Alias for @c basic_ostringstream<char>. */
typedef basic_ostringstream<char>::type ostringstream;
/**	Alias for @c basic_stringstream<char>. */
typedef basic_stringstream<char>::type stringstream;

/** @} */ // end of doc-group aux

} // namespace aux
} // namespace duct

#endif // DUCT_AUX_HPP_
