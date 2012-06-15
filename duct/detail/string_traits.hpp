/**
@file detail/string_traits.hpp
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

String type traits.
*/

#ifndef DUCT_DETAIL_STRING_TRAITS_HPP_
#define DUCT_DETAIL_STRING_TRAITS_HPP_

#include "./../config.hpp"
#include "./../EncodingUtils.hpp"

#include <type_traits>
#include <string>

namespace duct {
namespace detail {

/**
	@addtogroup string
	@{
*/

/**
	String type traits.
	@note @c std::wstring will get either UTF-16 or UTF-32 @c EncodingUtils depending on the system; @c u16string and @c u32string should be used when possible since they actually specify the use of these encodings.
	@tparam T String type.
*/
template<class T>
struct string_traits {
	/** String type; equivalent to @a T. */
	typedef T string_type;
	/** The string type's character type. */
	typedef typename string_type::value_type char_type;
	/** Size of @c char_type. */
	static constexpr std::size_t char_size=sizeof(char_type);
	/** Encoding utilities for the string type. */
	typedef EncodingUtils<char_size> encoding_utils;
};

/** @} */ // end of doc-group string

} // namespace detail
} // namespace duct

#endif // DUCT_DETAIL_STRING_TRAITS_HPP_
