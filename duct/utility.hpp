/**
@file utility.hpp
@brief Utilities.

@author Timothy Howard
@copyright 2010-2014 Timothy Howard under the MIT license;
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
	Cast unsigned integral to signed integral.

	@param value Value.
*/
template<
	typename T
>
inline constexpr typename std::make_signed<T>::type
signed_cast(
	T const value
) noexcept {
	return static_cast<typename std::make_signed<T>::type>(value);
}

/**
	Cast signed integral to unsigned integral.

	@param value Value.
*/
template<
	typename T
>
inline constexpr typename std::make_unsigned<T>::type
unsigned_cast(
	T const value
) noexcept {
	return static_cast<typename std::make_unsigned<T>::type>(value);
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

/** @cond INTERNAL */

namespace {

template<
	typename U,
	typename E
>
inline constexpr U
pack_bitor(
	U const value
) noexcept {
	return value;
}

template<
	typename U,
	typename E,
	typename... Rest
>
inline constexpr U
pack_bitor(
	U const value,
	E const head,
	Rest const... rest
) noexcept {
	return pack_bitor<U, E>(
		value | static_cast<U const>(head),
		rest...
	);
}

} // anonymous namespace

/** @endcond */ // INTERNAL

/**
	Bitwise-OR enum bitflags.

	@returns Value of OR'd flags.
	@tparam E Enum type; deduced from @a first.
	@tparam U Type to cast to; defaults to the underlying type.
	@tparam Rest Rest types. Type deduction will fail if any
	type in this parameter pack is not @a E.
	@param first First flag.
	@param rest Rest of flags.
*/
template<
	typename E,
	typename U = typename std::underlying_type<E>::type,
	typename... Rest
>
inline constexpr U
enum_bitor(
	E const first,
	Rest const... rest
) noexcept {
	static_assert(
		std::is_enum<E>::value,
		"E must be an enum"
	);
	return pack_bitor<U, E>(static_cast<U const>(first), rest...);
}

/**
	Bitwise-AND enum bitflags.

	@returns Value of AND'd flags.
	@tparam E Enum type; deduced from @a first.
	@tparam U Type to cast to; defaults to the underlying type.
	@param x First flag.
	@param y Second flag.
*/
template<
	typename E,
	typename U = typename std::underlying_type<E>::type
>
inline constexpr U
enum_bitand(
	E const x,
	E const y
) noexcept {
	static_assert(
		std::is_enum<E>::value,
		"E must be an enum"
	);
	return static_cast<U const>(x) & static_cast<U const>(y);
}

/**
	Combine enum bitflags.

	Same as enum_bitor(), but casts result to @a E.

	@returns Enum value of OR'd flags.
	@tparam E Enum type; deduced from @a first.
	@tparam U Type to cast to; defaults to the underlying type.
	@tparam Rest Rest types. Type deduction will fail if any
	type in this parameter pack is not @a E.
	@param first First flag.
	@param rest Rest of flags.
*/
template<
	typename E,
	typename U = typename std::underlying_type<E>::type,
	typename... Rest
>
inline constexpr E
enum_combine(
	E const first,
	Rest const... rest
) noexcept {
	static_assert(
		std::is_enum<E>::value,
		"E must be an enum"
	);
	return static_cast<E const>(
		pack_bitor<U, E>(static_cast<U const>(first), rest...)
	);
}

/**
	Get the minimum of two values (constexpr).

	@note This expects an arithmetic type. It is intended for use in
	constexpr contexts.

	@tparam T Arithmetic type.
	@returns @a x if <code>x < y</code>, else @a y.
	@param x,y Values.
*/
template<
	typename T
>
inline constexpr T
min_ce(
	T const x,
	T const y
) noexcept {
	return
		x < y
		? x
		: y
	;
}

/**
	Get the minimum of two values (constexpr).

	@note This expects an arithmetic type. It is intended for use in
	constexpr contexts.

	@tparam T Arithmetic type.
	@returns @a x if <code>x > y</code>, else @a y.
	@param x,y Values.
*/
template<
	typename T
>
inline constexpr T
max_ce(
	T const x,
	T const y
) noexcept {
	return
		x > y
		? x
		: y
	;
}

/** @} */ // end of doc-group utils

} // namespace duct

#endif // DUCT_UTILITY_HPP_
