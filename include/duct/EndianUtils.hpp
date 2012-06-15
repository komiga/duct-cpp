/**
@file EndianUtils.hpp
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

Endian utilities.
*/

#ifndef DUCT_ENDIANUTILS_HPP_
#define DUCT_ENDIANUTILS_HPP_

#include <duct/config.hpp>

#include <type_traits>
#include <algorithm>
#include <cstddef>

namespace duct {

// Forward declarations
//enum Endian : int;

/**
	@defgroup endian_swap_utils Endian-swapping utilities
	@note The byte_swap() functions:
	-# are only usable with 2, 4, and 8 byte-sized arithmetic types (integral and floating-point types; see @c std::is_arithmetic); a 1-byte immediate-return implementation is defined for convenience
	-# may be system-optimized for integral types (where @c <byteswap.h> is present)
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
		static inline T swap(T value) {
			return value;
		}
	};

	// Specialize float and double.
	// bswap_X() could probably be used safely with both type-casted floating-point types,
	// but it's safer to just swap the bytes (apparently 80-bit doubles are possible? I
	// muchly doubt their prevalence, but whatever)
	template<> struct bs_impl<float> {
		static float swap(float value) {
			char& b=reinterpret_cast<char&>(value);
			std::reverse(&b, &b+sizeof(float));
			return value;
		}
	};

	template<> struct bs_impl<double> {
		static double swap(double value) {
			char& b=reinterpret_cast<char&>(value);
			std::reverse(&b, &b+sizeof(double));
			return value;
		}
	};

	template<typename T>
	struct bs_impl<T, 2> {
		static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
		static inline T swap(T value) {
			return static_cast<T>(bswap_16(reinterpret_cast<uint16_t&>(value)));
		}
	};

	template<typename T>
	struct bs_impl<T, 4> {
		static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
		static inline T swap(T value) {
			return static_cast<T>(bswap_32(reinterpret_cast<uint32_t&>(value)));
		}
	};

	template<typename T>
	struct bs_impl<T, 8> {
		static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
		static inline T swap(T value) {
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
inline T byte_swap(T value) {
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
T byte_swap_if(T value, duct::Endian const endian) {
	if (Endian::SYSTEM!=endian) {
		return byte_swap<T>(value);
	} else {
		return value;
	}
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

/** @} */ // end of doc-group endian_swap_utils

} // namespace duct

#endif // DUCT_ENDIANUTILS_HPP_
