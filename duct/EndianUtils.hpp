/**
@file EndianUtils.hpp
@brief Endian utilities.

@author Tim Howard
@copyright 2010-2012 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_ENDIANUTILS_HPP_
#define DUCT_ENDIANUTILS_HPP_

#include "./config.hpp"

#include <cstddef>
#include <type_traits>
#include <algorithm>

namespace duct {

// Forward declarations
enum class Endian : unsigned int;

/**
	@addtogroup endian_utils
	@{
*/

/**
	Endian types.
*/
enum class Endian : unsigned int {
	/** System's endian. Equal to %DUCT_BYTEORDER (either %DUCT_ENDIAN_LITTLE or %DUCT_ENDIAN_BIG). */
	SYSTEM=DUCT_BYTEORDER,
	/** Little endian. Equal to %DUCT_ENDIAN_LITTLE. */
	LITTLE=DUCT_ENDIAN_LITTLE,
	/** Big endian. Equal to %DUCT_ENDIAN_BIG. */
	BIG=DUCT_ENDIAN_BIG
};

/** @cond INTERNAL */
namespace {
	// Can't do partial template function specialization (grumble grumble); using a struct instead for the sneaky magics
	template<typename T, std::size_t size_=sizeof(T)>
	struct bs_impl;

	// Convenience
	template<typename T>
	struct bs_impl<T, 1> {
		inline static constexpr T swap(T value) {
			return value;
		}
	};

	// Specialize for floating-point types.
	// bswap_X() could maybe sometimes be used safely with both 32-bit and 64-bit floating-point types,
	// but it's safer to just swap the bytes (esp. since the bswap macros can use assembly voodoo).
	template<> struct bs_impl<float> {
		inline static float swap(float value) {
			char& b=reinterpret_cast<char&>(value);
			std::reverse(&b, &b+sizeof(float));
			return value;
		}
	};

	// *long* double, you say? Never heard of such an arcane thing!
	template<> struct bs_impl<double> {
		inline static double swap(double value) {
			char& b=reinterpret_cast<char&>(value);
			std::reverse(&b, &b+sizeof(double));
			return value;
		}
	};

	// NB: Cannot constexpr these; bswap macros can potentially use assembly code
	template<typename T>
	struct bs_impl<T, 2> {
		static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
		inline static T swap(T value) {
			return static_cast<T>(bswap_16(reinterpret_cast<uint16_t&>(value)));
		}
	};

	template<typename T>
	struct bs_impl<T, 4> {
		static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
		inline static T swap(T value) {
			return static_cast<T>(bswap_32(reinterpret_cast<uint32_t&>(value)));
		}
	};

	template<typename T>
	struct bs_impl<T, 8> {
		static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
		inline static T swap(T value) {
			return static_cast<T>(bswap_64(reinterpret_cast<uint64_t&>(value)));
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
template<typename T>
inline constexpr T byte_swap(T value) {
	static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
	return bs_impl<T, sizeof(T)>::swap(value);
}

/**
	Reverse the bytes in an arithmetic value (by-ref).
	@tparam T Arithmetic value type; inferred from @a value.
	@param[in,out] value Value to swap; output value.
*/
template<typename T>
inline void byte_swap_ref(T& value) {
	value=byte_swap<T>(value);
}

/**
	Reverse the bytes in an arithmetic value if the desired endian is different from the system endian.
	@returns The byte-swapped value.
	@tparam T Arithmetic value type; inferred from @a value.
	@param value Value to swap.
	@param endian Desired endian.
*/
template<typename T>
inline constexpr T byte_swap_if(T value, duct::Endian const endian) {
	return (Endian::SYSTEM!=endian) ? byte_swap<T>(value) : value;
}

/**
	Reverse the bytes in an arithmetic value (by-ref) if the desired endian is different from the system endian.
	@tparam T Arithmetic value type; inferred from @a value.
	@param[in,out] value Value to swap; output value.
	@param endian Desired endian.
*/
template<typename T>
inline void byte_swap_ref_if(T& value, duct::Endian const endian) {
	if (Endian::SYSTEM!=endian) {
		byte_swap_ref<T>(value);
	}
}

/** @} */ // end of doc-group endian_utils

} // namespace duct

#endif // DUCT_ENDIANUTILS_HPP_
