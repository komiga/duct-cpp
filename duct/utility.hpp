/**
@file utility.hpp
@brief Utilities.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_UTILITY_HPP_
#define DUCT_UTILITY_HPP_

#include "./config.hpp"

#include <type_traits>

namespace duct {

// Forward declarations

/**
	@addtogroup utils
	@{
*/

/**
	Make a reference to value a reference to const value.

	@tparam T Value type; deduced from @a value.

	@param value Value.
*/
template<
	typename T
>
inline typename std::add_const<T>::type const&
make_const(
	T& value
) noexcept {
	return value;
}

/**
	Cast enum to underlying or integral type.

	@tparam E Enum type; deduced from @a value.
	@tparam U Type to cast to; defaults to the underlying type.
	@param value Enum value.
*/
template<
	typename E,
	typename U = typename std::underlying_type<E>::type
>
inline constexpr U
enum_cast(
	E const value
) noexcept {
	static_assert(
		std::is_enum<E>::value,
		"E must be an enum"
	);
	return static_cast<U>(value);
}

/** @} */ // end of doc-group utils

} // namespace duct

#endif // DUCT_UTILITY_HPP_
