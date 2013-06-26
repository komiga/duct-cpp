/**
@file detail/string_traits.hpp
@brief String type traits.

@author Tim Howard
@copyright 2010-2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_DETAIL_STRING_TRAITS_HPP_
#define DUCT_DETAIL_STRING_TRAITS_HPP_

#include "./../config.hpp"
#include "./../traits.hpp"
#include "./../EncodingUtils.hpp"

#include <type_traits>
#include <string>

namespace duct {
namespace detail {

/**
	@addtogroup text
	@{
*/

/**
	@ingroup traits

	String type traits.

	@note @c std::wstring will get either UTF-16 or
	UTF-32 @c EncodingUtils depending on the system; @c u16string
	and @c u32string should be used when possible since they actually
	specify the use of these encodings.

	@tparam T String type.
*/
template<
	class T
>
struct string_traits final
	: public traits::restrict_all
{
	/** String type; equivalent to @a T. */
	using string_type = T;

	/** The string type's character type. */
	using char_type = typename string_type::value_type;

	/** Size of @c char_type. */
	static constexpr std::size_t
		char_size = sizeof(char_type);

	/** Encoding utilities for the string type. */
	using encoding_utils = EncodingUtils<char_size>;
};

/** @} */ // end of doc-group text

} // namespace detail
} // namespace duct

#endif // DUCT_DETAIL_STRING_TRAITS_HPP_
