/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Utilities.
*/

#pragma once

#include "./config.hpp"

#include <type_traits>

namespace duct {

// Forward declarations

/**
	@addtogroup utils
	@{
*/

/**
	Get an integer value with a single bit enabled.

	@warning Some compilers may not warn for a bit shift >= the width
	of @a T (in a constexpr context).

	@param index Bit index.
*/
template<
	typename T = unsigned
>
inline constexpr T
bit(
	unsigned const index
) noexcept {
	return T{1} << index;
}

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

/** @name Enum-class bitwise operators */ /// @{

namespace enum_ops {

/**
	Enum-class OR operator.
*/
template<class FlagT, class = std::enable_if<std::is_enum<FlagT>::value>>
inline constexpr FlagT operator|(FlagT const& x, FlagT const& y) noexcept {
	using U = typename std::underlying_type<FlagT>::type;
	return static_cast<FlagT>(
		static_cast<U>(x) | static_cast<U>(y)
	);
}

/**
	Enum-class AND operator.
*/
template<class FlagT, class = std::enable_if<std::is_enum<FlagT>::value>>
inline constexpr FlagT operator&(FlagT const& x, FlagT const& y) noexcept {
	using U = typename std::underlying_type<FlagT>::type;
	return static_cast<FlagT>(
		static_cast<U>(x) & static_cast<U>(y)
	);
}

/**
	Enum-class NOT operator.
*/
template<class FlagT, class = std::enable_if<std::is_enum<FlagT>::value>>
inline constexpr FlagT operator~(FlagT const& x) noexcept {
	using U = typename std::underlying_type<FlagT>::type;
	return static_cast<FlagT>(
		~static_cast<U>(x)
	);
}

/**
	Enum-class OR-assign operator.
*/
template<class FlagT, class = std::enable_if<std::is_enum<FlagT>::value>>
inline constexpr FlagT& operator|=(FlagT& x, FlagT const& y) noexcept {
	using U = typename std::underlying_type<FlagT>::type;
	return x = static_cast<FlagT>(
		static_cast<U>(x) | static_cast<U>(y)
	);
}

/**
	Enum-class AND-assign operator.
*/
template<class FlagT, class = std::enable_if<std::is_enum<FlagT>::value>>
inline constexpr FlagT& operator&=(FlagT& x, FlagT const& y) noexcept {
	using U = typename std::underlying_type<FlagT>::type;
	return x = static_cast<FlagT>(
		static_cast<U>(x) & static_cast<U>(y)
	);
}

} // namespace enum_ops
using namespace duct::enum_ops;

/** @} */ // end of name-group Enum-class bitwise operators

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

/**
	Check if a type is the same as any type in a pack.

	@tparam T Type to find.
	@tparam ...M Types to match.
*/
template<
	class T,
	class... M
>
struct is_same_any
	: public std::false_type
{};

/** @cond INTERNAL */
template<
	class T,
	class H,
	class... R
>
struct is_same_any<T, H, R...>
	: public std::integral_constant<
		bool,
		std::is_same<T, H>::value
		? true
		: is_same_any<T, R...>::value
	>
{};
/** @endcond */ // INTERNAL

/**
	Get number of elements in bounded array.
*/
template<class T, unsigned N>
inline constexpr unsigned
array_extent(T const (&)[N]) noexcept {
    return N;
}

/**
	Get number of elements in bounded array.
*/
template<class T, class U, unsigned N>
inline constexpr unsigned
array_extent(T const (U::* const)[N]) noexcept {
    return N;
}

/**
	Get sizeof type or 0 if the type is empty.
*/
template<class T>
inline constexpr unsigned
sizeof_empty() noexcept {
    return std::is_empty<T>::value ? 0 : sizeof(T);
}

/** @} */ // end of doc-group utils

} // namespace duct
