/**
@file
@brief Arithmetic IO operations.

@author Timothy Howard
@copyright 2010-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

// TODO: Unicode read/write variants for generic iterators
// (to supply support for narrow and wide string literals)

#pragma once

#include "../config.hpp"
#include "../debug.hpp"
#include "../EndianUtils.hpp"

#include <type_traits>
#include <iostream>

namespace duct {
namespace IO {

// Forward declarations
class StreamContext;

/**
	@addtogroup io
	@{
*/

/**
	@name Raw data
	@{
*/

/**
	Read raw data from a stream.

	@warning @a dest must have a capacity of at least @a size bytes.

	@param stream Source stream.
	@param[out] dest Data destination.
	@param size Number of bytes to read.
*/
inline void
read(
	std::istream& stream,
	void* dest,
	std::size_t const size
) {
	stream.read(static_cast<char*>(dest), size);
}

/**
	Write raw data to a stream.

	@warning @a src must have a capacity of at least @a size bytes.

	@param stream Destination stream.
	@param src Data source.
	@param size Number of bytes to write.
*/
inline void
write(
	std::ostream& stream,
	void const* const src,
	std::size_t const size
) {
	stream.write(static_cast<char const*>(src), size);
}

/**
	Read arithmetic value (by-ref) from a stream.

	@tparam T Arithmetic value type; inferred from @a value.
	@param stream Source stream.
	@param[out] value Output value; result undefined if: the
	operation failed (see @c std::basic_istream::read())
	and @c endian!=Endian::system.
	@param endian Endian to use when reading; defaults
	to @c Endian::system (no swapping).
*/
template<
	typename T
>
inline void
read_arithmetic(
	std::istream& stream,
	T& value,
	Endian const endian = Endian::system
) {
	static_assert(
		std::is_arithmetic<T>::value,
		"T must be arithmetic"
	);
	stream.read(reinterpret_cast<char*>(&value), sizeof(T));
	byte_swap_ref_if(value, endian);
}

/**
	Read arithmetic value from a stream.

	@returns Output value; undefined if: the operation failed
	(see @c std::basic_istream::read())
	and @c endian!=Endian::system.
	@tparam T Arithmetic value type; inferred from @a value.
	@param stream Source stream.
	@param endian Endian to use when reading; defaults
	to @c Endian::system (no swapping).
*/
template<
	typename T
>
inline T
read_arithmetic(
	std::istream& stream,
	Endian const endian = Endian::system
) {
	static_assert(
		std::is_arithmetic<T>::value,
		"T must be arithmetic"
	);
	T value;
	stream.read(reinterpret_cast<char*>(&value), sizeof(T));
	byte_swap_ref_if(value, endian);
	return value;
}

/**
	Read arithmetic array from a stream.

	@warning @a dest must have a capacity of at least @a count
	elements.

	@tparam T Arithmetic value type; inferred from @a dest.
	@param stream Source stream.
	@param[out] dest Output array; result undefined if: the operation
	failed (see @c std::basic_istream::read())
	and @c endian!=Endian::system.
	@param count Number of elements to read.
	@param endian Endian to use when reading; defaults
	to @c Endian::system (no swapping).
*/
template<
	typename T
>
void
read_arithmetic_array(
	std::istream& stream,
	T* const dest,
	std::size_t const count,
	Endian const endian = Endian::system
) {
	static_assert(
		std::is_arithmetic<T>::value,
		"T must be arithmetic"
	);
	if (0u < count) {
		stream.read(reinterpret_cast<char*>(dest), sizeof(T) * count);
		if (Endian::system != endian && 1u != sizeof(T)) {
			for (std::size_t idx = 0u; count > idx; ++idx) {
				byte_swap_ref(dest[idx]);
			}
		}
	}
}

/**
	Write arithmetic value to a stream.

	@tparam T Arithmetic value type; inferred from @a value.
	@param stream Destination stream.
	@param value Value to write.
	@param endian Endian to use when writing; defaults
	to @c Endian::system (no swapping).
*/
template<
	typename T
>
inline void
write_arithmetic(
	std::ostream& stream,
	T value,
	Endian const endian = Endian::system
) {
	static_assert(
		std::is_arithmetic<T>::value,
		"T must be arithmetic"
	);
	byte_swap_ref_if(value, endian);
	stream.write(reinterpret_cast<char const*>(&value), sizeof(T));
}

/**
	Write arithmetic array to a stream.

	@warning @a src must have a capacity of at least @a count
	elements.

	@tparam T Arithmetic value type; inferred from @a src.
	@param stream Destination stream.
	@param src Array to write.
	@param count Number of elements to write.
	@param endian Endian to use when writing; defaults
	to @c Endian::system (no swapping).
*/
template<
	typename T
>
void
write_arithmetic_array(
	std::ostream& stream,
	T const* const src,
	std::size_t const count,
	Endian const endian = Endian::system
) {
	static_assert(
		std::is_arithmetic<T>::value,
		"T must be arithmetic"
	);
	static constexpr std::size_t const
	BUFFER_SIZE = 64u;

	T flipbuf[BUFFER_SIZE];
	if (0u < count) {
		if (Endian::system != endian && 1u < sizeof(T)) {
			unsigned chunk;
			unsigned idx;
			for (chunk = 0u; count > chunk; chunk += idx) {
				for (
					idx = 0u;
					BUFFER_SIZE > idx && count > chunk + idx;
					++idx
				) {
					flipbuf[idx] = byte_swap(src[chunk + idx]);
				}
				stream.write(
					reinterpret_cast<char const*>(flipbuf),
					sizeof(T) * idx
				);
				if (!stream.good()) {
					DUCT_DEBUG("write_arithmetic: !stream.good()");
					return;
				}
			}
		} else {
			stream.write(
				reinterpret_cast<char const*>(src),
				sizeof(T) * count
			);
		}
	}
}

/** @} */ // end of name-group Raw data
/** @} */ // end of doc-group io

} // namespace IO
} // namespace duct
