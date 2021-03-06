/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Endian utilities.
*/

#pragma once

#include "./config.hpp"

#include <cstddef>
#include <type_traits>
#include <algorithm>

namespace duct {

// Forward declarations
enum class Endian : unsigned;

/**
	@addtogroup endian_utils
	@{
*/

/**
	Endian types.
*/
enum class Endian : unsigned {
	/** System endian. */
	system = DUCT_BYTEORDER,
	/** Little endian. */
	little = DUCT_ENDIAN_LITTLE,
	/** Big endian. */
	big = DUCT_ENDIAN_BIG
};

/** @cond INTERNAL */
namespace {

// Can't do partial template function specialization
// (grumble grumble)
template<
	class T,
	std::size_t = sizeof(T)
>
struct bs_impl;

// Convenience
template<class T>
struct bs_impl<T, 1u> {
	static constexpr T
	swap(
		T value
	) {
		return value;
	}
};

// Specialize for floating-point types
template<>
struct bs_impl<float> {
	static float
	swap(
		float value
	) {
		char& b = reinterpret_cast<char&>(value);
		std::reverse(&b, &b + sizeof(float));
		return value;
	}
};

// *long* double, you say? Never heard of such an arcane thing!
template<>
struct bs_impl<double> {
	static double
	swap(
		double value
	) {
		char& b = reinterpret_cast<char&>(value);
		std::reverse(&b, &b + sizeof(double));
		return value;
	}
};

#if !defined(DUCT_ENDIAN_IGNORE_OLD_CASTS) && (			\
	 (DUCT_COMPILER & DUCT_FLAG_COMPILER_GCC) ||		\
	((DUCT_COMPILER & DUCT_FLAG_COMPILER_CLANG) &&		\
	  DUCT_COMPILER > DUCT_FLAG_COMPILER_CLANG33))
//
	#define DUCT_ENDIAN_IGNORE_OLD_CASTS
#endif

// NB: These cannot be constexpr; the bswap macros can potentially
// use asm instructions
template<class T>
struct bs_impl<T, 2u> {
	static T
	swap(
		T value
	) {
	#ifdef DUCT_ENDIAN_IGNORE_OLD_CASTS
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wold-style-cast"
	#endif
		return static_cast<T>(
			bswap_16(reinterpret_cast<std::uint16_t&>(value))
		);
	#ifdef DUCT_ENDIAN_IGNORE_OLD_CASTS
		#pragma GCC diagnostic pop
	#endif
	}
};

template<class T>
struct bs_impl<T, 4u> {
	static T
	swap(
		T value
	) {
	#ifdef DUCT_ENDIAN_IGNORE_OLD_CASTS
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wold-style-cast"
	#endif
		return static_cast<T>(
			bswap_32(reinterpret_cast<std::uint32_t&>(value))
		);
	#ifdef DUCT_ENDIAN_IGNORE_OLD_CASTS
		#pragma GCC diagnostic pop
	#endif
	}
};

template<class T>
struct bs_impl<T, 8u> {
	static T
	swap(
		T value
	) {
	#ifdef DUCT_ENDIAN_IGNORE_OLD_CASTS
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wold-style-cast"
	#endif
		return static_cast<T>(
			bswap_64(reinterpret_cast<std::uint64_t&>(value))
		);
	#ifdef DUCT_ENDIAN_IGNORE_OLD_CASTS
		#pragma GCC diagnostic pop
	#endif
	}
};

} // anonymous namespace
/** @endcond */ // INTERNAL

/**
	Reverse the bytes in an arithmetic value.

	@returns The byte-swapped value.
	@tparam T Arithmetic value type; inferred from @a value.
	@param value Value to swap.
*/
template<class T>
inline T
byte_swap(
	T value
) {
	static_assert(
		std::is_arithmetic<T>::value,
		"T must be an arithmetic type"
	);
	return bs_impl<T, sizeof(T)>::swap(value);
}

/**
	Reverse the bytes in an arithmetic value (by-ref).

	@tparam T Arithmetic value type; inferred from @a value.
	@param[in,out] value Value to swap; output value.
*/
template<class T>
inline void
byte_swap_ref(
	T& value
) {
	value = byte_swap<T>(value);
}

/**
	Reverse the bytes in an arithmetic value if the desired
	endian is different from the system endian.

	@returns The byte-swapped value.
	@tparam T Arithmetic value type; inferred from @a value.
	@param value Value to swap.
	@param endian Desired endian.
*/
template<class T>
inline T
byte_swap_if(
	T value,
	duct::Endian const endian
) {
	return (Endian::system != endian)
		? byte_swap<T>(value)
		: value;
}

/**
	Reverse the bytes in an arithmetic value (by-ref) if the
	desired endian is different from the system endian.

	@tparam T Arithmetic value type; inferred from @a value.
	@param[in,out] value Value to swap; output value.
	@param endian Desired endian.
*/
template<class T>
inline void
byte_swap_ref_if(
	T& value,
	duct::Endian const endian
) {
	if (Endian::system != endian) {
		byte_swap_ref<T>(value);
	}
}

/** @} */ // end of doc-group endian_utils

} // namespace duct
