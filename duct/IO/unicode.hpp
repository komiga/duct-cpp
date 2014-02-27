/**
@file IO/unicode.hpp
@brief Unicode IO operations.

@author Timothy Howard
@copyright 2010-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_IO_UNICODE_HPP_
#define DUCT_IO_UNICODE_HPP_

// TODO: Unicode read/write variants for generic iterators
// (to supply support for narrow and wide string literals)

#include "../config.hpp"
#include "../debug.hpp"
#include "../string.hpp"
#include "../detail/string_traits.hpp"
#include "../EndianUtils.hpp"
#include "./arithmetic.hpp"

#include <cstring>
#include <limits>
#include <iostream>

namespace duct {
namespace IO {

/**
	@addtogroup io
	@{
*/
/**
	@name Unicode
	@{
*/

/** @cond INTERNAL */
namespace {
template<
	class FromU
>
struct rchar_defs {
	using from_utils = FromU;
	using char_type = typename from_utils::char_type;

	enum {
		char_size = from_utils::char_size,
		BUFFER_SIZE = 6u // max UTF-8 (including invalid planes)
	};
};

template<
	class DefsT,
	std::size_t = DefsT::char_size
>
struct rchar_impl {
	static char32
	read_char(
		std::istream& stream,
		char32 const replacement,
		Endian const endian
	) {
		typename DefsT::char_type
			buffer[DefsT::BUFFER_SIZE],
			*iter = buffer;
		unsigned amt;
		char32 cp;
		IO::read_arithmetic(stream, buffer[0u], endian);
		if (!stream.good()) {
			//DUCT_DEBUG("rchar_impl<(defaults)>::read: !stream.good()");
			return replacement;
		}
		amt = DefsT::from_utils::required_first(buffer[0u]);
		if (amt) {
			IO::read_arithmetic_array(stream, buffer + 1u, amt, endian);
			if (!stream.good()) {
				//DUCT_DEBUG("rchar_impl<(defaults)>::read: 2 !stream.good()");
				return replacement;
			}
		}
		// Make amt equal the entire number of units
		++amt;
		typename DefsT::char_type const*
			next = DefsT::from_utils::decode(
				iter, buffer + amt, cp, replacement
			);
		if (next == iter) {
			// Incomplete sequence; shouldn't occur due to final
			// stream goodness check
			DUCT_DEBUG("rchar_impl<(defaults)>::read: ics; curious!");
			return replacement;
		} else {
			return cp;
		}
	}
};

// Specialize for UTF-32
template<
	class DefsT
>
struct rchar_impl<DefsT, 4u> {
	static char32
	read_char(
		std::istream& stream,
		char32 const replacement,
		Endian const endian
	) {
		char32 cp;
		IO::read_arithmetic(stream, cp, endian);
		if (!stream.good()) {
			//DUCT_DEBUG("rchar_impl<(default),4>::read: !stream.good()");
			return replacement;
		}
		if (!DUCT_UNI_IS_CP_VALID(cp)) {
			cp = replacement;
		}
		return cp;
	}
};
} // anonymous namespace
/** @endcond */ // INTERNAL

/**
	Read a single code point from a stream.

	@returns
	- The code point read from @a stream; or
	- @a replacement if either the decoded code point was invalid or
	  if there was an error reading from the stream
	  (see @c std::basic_istream::read()).
	@tparam FromU @c EncodingUtils specialization for decoding from
	the stream.
	@param stream Stream to read from.
	@param replacement Replacement code point; defaults
	to @c CHAR_SENTINEL.
	@param endian Endian to use when reading; defaults
	to @c Endian::system (no swapping).
*/
template<
	class FromU,
	class DefsT = rchar_defs<FromU>
>
inline char32
read_char(
	std::istream& stream,
	char32 const replacement = CHAR_SENTINEL,
	Endian const endian = Endian::system
) {
	return rchar_impl<DefsT>::read_char(stream, replacement, endian);
}

/** @cond INTERNAL */
namespace {
template<
	class ToU
>
struct wchar_defs {
	using to_utils = ToU;
	using char_type = typename to_utils::char_type;

	static constexpr std::size_t const
		char_size = to_utils::char_size,
		BUFFER_SIZE = to_utils::max_units
	;
};

template<
	class DefsT,
	std::size_t = DefsT::char_size
>
struct wchar_impl {
	static std::size_t
	write_char(
		std::ostream& stream,
		char32 cp,
		unsigned const num,
		char32 const replacement,
		Endian const endian
	) {
		if (!DUCT_UNI_IS_CP_VALID(cp)) {
			if (CHAR_NULL == replacement
			|| !DUCT_UNI_IS_CP_VALID(replacement)
			) {
				return 0u;
			} else {
				cp = replacement;
			}
		}
		typename DefsT::char_type
			out_buffer[DefsT::BUFFER_SIZE],
			*out_iter = out_buffer;
		out_iter = DefsT::to_utils::encode(cp, out_iter, CHAR_NULL);
		// Should not occur because both cp and replacement are
		// checked for invalidity before encoding
		if (out_iter == out_buffer) {
			DUCT_DEBUG(
				"wchar_impl<(def)>::write:"
				" out_iter == out_buffer; curious!"
			);
			return 0u;
		} else {
			unsigned amt = (out_iter - out_buffer);
			if (Endian::system != endian && 1u != DefsT::char_size) {
				for (unsigned idx = 0u; amt > idx; ++idx) {
					byte_swap_ref(out_buffer[idx]);
				}
			}
			unsigned i = num;
			while (i--) {
				// Using raw write instead of write_arithmetic_array()
				// because it would use an unnecessary touch of the
				// stack
				IO::write(stream, out_buffer, amt * DefsT::char_size);
			}
			return num * amt;
		}
	}
};

// Specialize for UTF-32
template<
	class DefsT
>
struct wchar_impl<DefsT, 4u> {
	static std::size_t
	write_char(
		std::ostream& stream, char32 cp,
		unsigned const num,
		char32 const replacement,
		Endian const endian
	) {
		if (!DUCT_UNI_IS_CP_VALID(cp)) {
			if (CHAR_NULL == replacement
			|| !DUCT_UNI_IS_CP_VALID(replacement)
			) {
				return 0u;
			} else {
				cp = replacement;
			}
		}
		unsigned i = num;
		while (i--) {
			IO::write_arithmetic(stream, cp, endian);
		}
		return num;
	}
};
} // anonymous namespace
/** @endcond */ // INTERNAL

/**
	Write a single code point to a stream.

	@returns The number of @a ToU code units written (will not be
	accurate if the stream write operation failed -
	see @c std::basic_ostream::write()).
	@tparam ToU @c EncodingUtils specialization for encoding to the
	stream.
	@param stream Destination stream.
	@param cp Code point to write.
	@param num Number of times to write @a cp; defaults to @c 1.
	@param replacement Replacement code point. If invalid or equal
	to @c CHAR_NULL (default) when @a cp is invalid, <strong>nothing
	will be written</strong> (returns @c 0).
	@param endian Endian to use when writing; defaults
	to @c Endian::system (no swapping).
*/
template<
	class ToU,
	class DefsT = wchar_defs<ToU>
>
inline std::size_t
write_char(
	std::ostream& stream,
	char32 const cp,
	unsigned const num = 1u,
	char32 const replacement = CHAR_NULL,
	Endian const endian = Endian::system
) {
	if (0u < num) {
		return wchar_impl<DefsT>
			::write_char(
				stream, cp, num, replacement, endian
			);
	} else {
		return 0u;
	}
}

/** @cond INTERNAL */
namespace {
template<
	class FromU,
	class StringT
>
struct rstr_defs {
	using string_type = StringT;
	using string_traits = detail::string_traits<string_type>;
	using from_utils = FromU;
	using to_utils = typename string_traits::encoding_utils;

	static constexpr std::size_t const
	BUFFER_SIZE = 512u;
	static constexpr std::ptrdiff_t const
	BUFFER_SIZE_S = 512;
};
} // anonymous namespace
/** @endcond */ // INTERNAL

/**
	Read sized string from a stream.

	@warning @a size is the number of code <strong>units</strong> to
	read in @a FromU's encoding, not the number of code <strong>
	points</strong>.

	@tparam FromU @c EncodingUtils specialization for decoding from
	the stream.
	@tparam StringT String type. The character size of this class will
	be used to determine its encoding.
	@param stream Source stream.
	@param size Size of the string to read, in @a FromU <strong>code
	units</strong>.
	@param[out] value Output value; result undefined if at some point
	a read operation failed (see @c std::basic_istream::read()).
	@param replacement Replacement code point. If invalid or equal
	to @c CHAR_NULL (default), invalid code points will be skipped
	rather than replaced.
	@param endian Endian to use when reading; defaults
	to @c Endian::system (no swapping).
*/
template<
	class FromU,
	class StringT,
	class DefsT = rstr_defs<FromU, StringT>
>
void
read_string(
	std::istream& stream,
	StringT& value,
	std::size_t size,
	char32 const replacement = CHAR_NULL,
	Endian const endian = Endian::system
) {
	typename DefsT::from_utils::char_type
		// Extra space to easily deal with incomplete sequences
		// (0 < offset) instead of doing a bunch of subtraction
		buffer[DefsT::BUFFER_SIZE + 6u],
		*end, *iter, *next;
	typename DefsT::to_utils::char_type
		out_buffer[DefsT::BUFFER_SIZE],
		*out_iter = out_buffer;
	unsigned offset = 0u, amt;
	char32 cp;
	value.clear();
	while (0u < size) {
		amt = (DefsT::BUFFER_SIZE < size)
			? DefsT::BUFFER_SIZE
			: size;
		IO::read(stream, buffer + offset, amt * DefsT::from_utils::char_size);
		if (!stream.good()) {
			DUCT_DEBUG("read_string: !stream.good()");
			break;
		}
		end=buffer + offset + amt;
		if (Endian::system != endian && 1u != DefsT::from_utils::char_size) {
			for (iter=buffer + offset; end>iter; ++iter) {
				byte_swap_ref(*iter);
			}
		}
		offset = 0u;
		for (iter = buffer; end > iter; iter = next) {
			next = DefsT::from_utils::decode(iter, end, cp, replacement);
			if (next == iter) { // Incomplete sequence
				//offset = DefsT::from_utils::required_first_whole(*next);
				offset = end - next;
				DUCT_DEBUG("read_string: ics");
				break;
			}
			out_iter = DefsT::to_utils::encode(cp, out_iter, replacement);
			// Prevent output overrun
			if (DefsT::BUFFER_SIZE_S <= 6u + (out_iter - out_buffer)) {
				value.append(out_buffer, out_iter);
				out_iter = out_buffer;
			}
		}
		size -= amt;
		if (0u != offset) { // Handle incomplete sequence
			DUCT_DEBUGF(
				"read_string: ics - pos: %lu offset: %u iter: 0x%X left: %lu",
				static_cast<unsigned long>(next - buffer),
				offset,
				*next,
				static_cast<unsigned long>(size)
			);
			if (0u >= size) {
				// No sense pushing back if there's no more data to
				// read
				break;
			} else {
				// Push the incomplete sequence to the beginning
				std::memcpy(buffer, next, offset);
			}
		}
	}
	// Flush if there's any data left in the buffer
	if (out_buffer != out_iter) {
		DUCT_DEBUG("read_string: flush out_buffer");
		value.append(out_buffer, out_iter);
	}
	if (0u != offset && CHAR_NULL != replacement) {
		// End of specified size with a trailing incomplete sequence
		DUCT_DEBUG("read_string: eos with trailing ics");
		value.append(1u, replacement);
	} else {
		DUCT_DEBUGF("read_string: eos; offset: %u size: %lu replacement: 0x%X",
			offset, static_cast<unsigned long>(size), replacement);
	}
}

/**
	Read sized string from a stream without checking for malformed or
	incomplete sequences.

	@warning @a size is the number of code <strong>units</strong> to
	read in @a StringT's encoding, not the number of code <strong>
	points</strong>.

	@tparam StringT String type. The character size of this class will
	be used to determine its encoding.
	@param stream Source stream.
	@param size Size of the string to read, in <strong>code
	units</strong>.
	@param[out] value Output value; result undefined if at some point
	a read operation failed (see std::basic_istream::read()).
	@param endian Endian to use when reading; defaults
	to @c Endian::system (no swapping).
*/
template<
	class StringT,
	class DefsT = rstr_defs<
		typename detail::string_traits<StringT>::encoding_utils,
		StringT
	>
>
void
read_string_copy(
	std::istream& stream,
	StringT& value,
	std::size_t size,
	Endian const endian = Endian::system
) {
	typename DefsT::from_utils::char_type
		out_buffer[DefsT::BUFFER_SIZE],
		*out_iter;
	unsigned amt;
	value.clear();
	while (0u < size) {
		amt = (DefsT::BUFFER_SIZE < size)
			? DefsT::BUFFER_SIZE
			: size;
		IO::read(stream, out_buffer, amt * DefsT::from_utils::char_size);
		if (!stream.good()) {
			DUCT_DEBUG("read_string_copy: !stream.good()");
			return;
		}
		if (Endian::system != endian && 1u != DefsT::from_utils::char_size) {
			for (
				out_iter = out_buffer;
				out_buffer + amt > out_iter;
				++out_iter
			) {
				byte_swap_ref(*out_iter);
			}
		}
		value.append(out_buffer, amt);
		size -= amt;
	}
}

/** @cond INTERNAL */
namespace {
template<
	class ToU,
	class StringT
>
struct wstr_defs {
	using string_type = StringT;
	using string_traits = detail::string_traits<string_type>;
	using to_utils = ToU;
	using from_utils = typename string_traits::encoding_utils;

	static constexpr std::size_t const
	BUFFER_SIZE = 512u;
	static constexpr std::ptrdiff_t const
	BUFFER_SIZE_S = 512;
};
} // anonymous namespace
/** @endcond */ // INTERNAL

/**
	Write string to a stream.

	@returns The number of code units written in @a ToU's encoding;
	may not be accurate if @c std::basic_ostream::write() fails.
	@tparam ToU @c EncodingUtils specialization for encoding to the
	stream.
	@tparam StringT String type. The character size of this class will
	be used to determine its encoding.
	@param stream Destination stream.
	@param value String to write.
	@param replacement Replacement code point. If invalid or equal
	to @c CHAR_NULL (default), invalid code points will be skipped
	rather than replaced.
	@param endian Endian to use when writing; defaults
	to @c Endian::system (no swapping).
*/
template<
	class ToU,
	class StringT,
	class DefsT = wstr_defs<ToU, StringT>
>
std::size_t
write_string(
	std::ostream& stream,
	StringT const& value,
	char32 const replacement = CHAR_NULL,
	Endian const endian = Endian::system
) {
	typename DefsT::to_utils::char_type
		out_buffer[DefsT::BUFFER_SIZE],
		*out_iter = out_buffer;
	typename DefsT::string_type::const_iterator
		in_iter,
		in_next;
	std::size_t units_written = 0u;
	char32 cp;
	for (
		in_iter = value.cbegin();
		value.cend() != in_iter;
		in_iter = in_next
	) {
		in_next = DefsT::from_utils::decode(
			in_iter, value.cend(), cp, replacement);
		if (in_next == in_iter) { // Incomplete sequence
			DUCT_DEBUG("write_string: ics");
			break;
		}
		/*DUCT_DEBUGF(
			"write_string: in: %lu  out: %lu %p cp: %u 0x%X",
			static_cast<unsigned long>(in_next - value.cbegin()),
			static_cast<unsigned long>(out_iter - out_buffer),
			out_iter,
			cp,
			cp
		);*/
		out_iter = DefsT::to_utils::encode(cp, out_iter, replacement);
		// Prevent output overrun
		if (DefsT::BUFFER_SIZE_S <= 6u + (out_iter - out_buffer)) {
			if (Endian::system != endian && 1u != DefsT::to_utils::char_size) {
				for (auto iter = out_buffer; out_iter > iter; ++iter) {
					byte_swap_ref(*iter);
				}
			}
			IO::write(
				stream, out_buffer,
				DefsT::to_utils::char_size * (out_iter - out_buffer)
			);
			if (!stream.good()) {
				DUCT_DEBUG("write_string: !stream.good()");
				goto exit_f;
			}
			units_written += out_iter - out_buffer;
			out_iter = out_buffer;
		}
	}
	// Flush if there's any data left in the buffer
	if (out_buffer != out_iter) {
		if (Endian::system != endian && 1u != DefsT::to_utils::char_size) {
			for (auto iter = out_buffer; out_iter > iter; ++iter) {
				byte_swap_ref(*iter);
			}
		}
		IO::write(
			stream, out_buffer,
			DefsT::to_utils::char_size * (out_iter - out_buffer)
		);
		if (!stream.good()) {
			DUCT_DEBUG("write_string: 2; !stream.good()");
			goto exit_f;
		}
		units_written += out_iter-out_buffer;
	}

exit_f:
	return units_written;
}

/**
	Write string to a stream without checking for malformed or
	incomplete sequences.

	@returns The number of code units written in @a ToU's encoding;
	may not be accurate if @c std::basic_ostream::write() fails.
	@tparam StringT String type. The character size of this class will
	be used to determine its encoding.
	@param stream Destination stream.
	@param value String to write.
	@param endian Endian to use when writing; defaults
	to @c Endian::system (no swapping).
*/
template<
	class StringT,
	class DefsT = wstr_defs<
		typename detail::string_traits<StringT>::encoding_utils,
		StringT
	>
>
std::size_t
write_string_copy(
	std::ostream& stream,
	StringT const& value,
	Endian const endian = Endian::system
) {
	typename DefsT::from_utils::char_type
		out_buffer[DefsT::BUFFER_SIZE],
		*out_iter;
	unsigned amt, size = value.size();
	typename StringT::const_iterator
		str_iter = value.cbegin();
	while (0u < size) {
		amt = (DefsT::BUFFER_SIZE < size)
			? DefsT::BUFFER_SIZE
			: size;
		for (
			out_iter = out_buffer;
			out_buffer + amt > out_iter;
			++out_iter, ++str_iter
		) {
			if (
				Endian::system != endian &&
				1u != DefsT::from_utils::char_size
			) {
				*out_iter = byte_swap(*str_iter);
			} else {
				*out_iter = *str_iter;
			}
		}
		IO::write(stream, out_buffer, amt * DefsT::from_utils::char_size);
		if (!stream.good()) {
			DUCT_DEBUG("write_string_copy: !stream.good()");
			goto exit_f;
		}
		size -= amt;
	}

exit_f:
	return size - value.size();
}

/** @} */ // end of name-group Unicode
/** @} */ // end of doc-group io

} // namespace IO
} // namespace duct

#endif // DUCT_IO_UNICODE_HPP_
