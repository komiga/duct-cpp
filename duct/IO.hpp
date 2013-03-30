/**
@file IO.hpp
@brief I/O utilities and iostream extensions.

@author Tim Howard
@copyright 2010-2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

// TODO: Unicode read/write variants for generic iterators (to supply support
// for narrow and wide string literals)

#ifndef DUCT_IO_HPP_
#define DUCT_IO_HPP_

#include "./config.hpp"
#include "./debug.hpp"
#include "./string.hpp"
#include "./detail/string_traits.hpp"
#include "./EndianUtils.hpp"

#include <cassert>
#include <cstring>
#include <type_traits>
#include <limits>
#include <iostream>

namespace duct {
namespace IO {

// Forward declarations
template<
	typename CharT,
	typename TraitsT=std::char_traits<CharT>
>
class basic_memstreambuf;
template<
	typename CharT,
	typename TraitsT=std::char_traits<CharT>
>
class basic_imemstream;
template<
	typename CharT,
	typename TraitsT=std::char_traits<CharT>
>
class basic_omemstream;
template<
	typename CharT,
	typename TraitsT=std::char_traits<CharT>
>
class basic_memstream;
class StreamContext;

/**
	@addtogroup io
	@{
*/

/**
	@name Helper classes
	@warning All memory stream buffer classes are incapable of
	automatically growing.
	@sa StreamContext
	@{
*/

/** Memory streambuf. */
typedef basic_memstreambuf<char> memstreambuf;
/** Input memory stream. */
typedef basic_imemstream<char> imemstream;
/** Output memory stream. */
typedef basic_omemstream<char> omemstream;
/** Input/output memory stream. */
typedef basic_memstream<char> memstream;

/** @} */ // end of name-group Helper classes

/**
	@name Utilities
	@{
*/

/**
	Get the size of a stream.
	@note This requires bidirectional seeking
	(e.g. @c memstream, @c std::ifstream, @c std::istringstream).
	@warning @c stream may be in an @c std::ios_base::failbit state upon
	return (in which case the return value will be @c 0) or <strong>existing
	error states may even be removed</strong>.
	@returns The size of the stream (0 may mean failure).
	@param stream Stream to measure.
*/
template<
	typename CharT,
	class TraitsT
>
std::size_t size(
	std::basic_istream<CharT, TraitsT>& stream
) {
	if (stream.eof()) {
		// Already at eof: don't have to seek anywhere
		DUCT_DEBUG("duct::IO::size: eof() initial");
		// Get rid of all other states
		stream.setstate(std::ios_base::eofbit);
		auto const end=stream.tellg();
		if (decltype(end)(-1)!=end) {
			return static_cast<std::size_t>(end);
		} else {
			DUCT_DEBUG("duct::IO::size: -1==end");
		}
	} else {
		// Toss all states
		stream.clear();
		auto const original=stream.tellg();
		// If the stream will give position
		if (decltype(original)(-1)!=original) {
			stream.seekg(0, std::ios_base::end);
			auto const end=stream.tellg();
			if (decltype(end)(-1)!=end) {
				stream.seekg(original);
				return static_cast<std::size_t>(end);
			} else {
				DUCT_DEBUG("duct::IO::size: -1==end");
			}
		} else {
			DUCT_DEBUG("duct::IO::size: -1==original");
		}
	}
	return 0u;
}

/** @} */ // end of name-group Utilities

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
inline void read(
	std::istream& stream,
	void* dest,
	std::size_t const size
) {
	stream.read(reinterpret_cast<char*>(dest), size);
}

/**
	Write raw data to a stream.
	@warning @a src must have a capacity of at least @a size bytes.
	@param stream Destination stream.
	@param src Data source.
	@param size Number of bytes to write.
*/
inline void write(
	std::ostream& stream,
	void const* const src,
	std::size_t const size
) {
	stream.write(reinterpret_cast<char const*>(src), size);
}

/**
	Read arithmetic value (by-ref) from a stream.
	@tparam T Arithmetic value type; inferred from @a value.
	@param stream Source stream.
	@param[out] value Output value; result undefined if: the operation failed
	(see @c std::basic_istream::read()) and @c endian!=Endian::SYSTEM.
	@param endian Endian to use when writing; defaults to @c Endian::SYSTEM
	(no swapping).
*/
template<typename T>
inline void read_arithmetic(
	std::istream& stream,
	T& value,
	Endian const endian=Endian::SYSTEM
) {
	static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
	stream.read(reinterpret_cast<char*>(&value), sizeof(T));
	byte_swap_ref_if(value, endian);
}

/**
	Read arithmetic value from a stream.
	@returns Output value; undefined if: the operation failed
	(see @c std::basic_istream::read()) and @c endian!=Endian::SYSTEM.
	@tparam T Arithmetic value type; inferred from @a value.
	@param stream Source stream.
	@param endian Endian to use when writing; defaults to @c Endian::SYSTEM
	(no swapping).
*/
template<typename T>
inline T read_arithmetic(
	std::istream& stream,
	Endian const endian=Endian::SYSTEM
) {
	static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
	T value;
	stream.read(reinterpret_cast<char*>(&value), sizeof(T));
	byte_swap_ref_if(value, endian);
	return value;
}

/**
	Read arithmetic array from a stream.
	@warning @a dest must have a capacity of at least @a count elements.
	@tparam T Arithmetic value type; inferred from @a dest.
	@param stream Source stream.
	@param[out] dest Output array; result undefined if: the operation failed
	(see @c std::basic_istream::read()) and @c endian!=Endian::SYSTEM.
	@param count Number of elements to read.
	@param endian Endian to use when writing; defaults to @c Endian::SYSTEM
	(no swapping).
*/
template<typename T>
void read_arithmetic_array(
	std::istream& stream,
	T* const dest,
	std::size_t const count,
	Endian const endian=Endian::SYSTEM
) {
	static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
	if (0<count) {
		stream.read(reinterpret_cast<char*>(dest), sizeof(T)*count);
		if (Endian::SYSTEM!=endian && 1u!=sizeof(T)) {
			for (std::size_t idx=0u; count>idx; ++idx) {
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
	@param endian Endian to use when writing; defaults to @c Endian::SYSTEM
	(no swapping).
*/
template<typename T>
inline void write_arithmetic(
	std::ostream& stream,
	T value,
	Endian const endian=Endian::SYSTEM
) {
	static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
	byte_swap_ref_if(value, endian);
	stream.write(reinterpret_cast<char const*>(&value), sizeof(T));
}

/**
	Write arithmetic array to a stream.
	@warning @a src must have a capacity of at least @a count elements.
	@tparam T Arithmetic value type; inferred from @a src.
	@param stream Destination stream.
	@param src Array to write.
	@param count Number of elements to write.
	@param endian Endian to use when writing; defaults to @c Endian::SYSTEM
	(no swapping).
*/
template<typename T>
void write_arithmetic_array(
	std::ostream& stream,
	T const* const src,
	std::size_t const count,
	Endian const endian=Endian::SYSTEM
) {
	static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
	enum {BUFFER_SIZE=64u};
	T flipbuf[BUFFER_SIZE];
	if (count) {
		if (Endian::SYSTEM!=endian && 1u!=sizeof(T)) {
			unsigned chunk;
			unsigned idx;
			for (chunk=0u; count>chunk; chunk+=idx) {
				for (idx=0u; BUFFER_SIZE>idx && count>chunk+idx; ++idx) {
					flipbuf[idx]=byte_swap(src[chunk+idx]);
				}
				stream.write(
					reinterpret_cast<char const*>(src),
					sizeof(T)*idx
				);
				if (!stream.good()) {
					DUCT_DEBUG("write_arithmetic: !stream.good()");
					return;
				}
			}
		} else {
			stream.write(
				reinterpret_cast<char const*>(src),
				sizeof(T)*count
			);
		}
	}
}

//** @} */ // end of name-group Raw data

/**
	@name Unicode
	@{
*/

/** @cond INTERNAL */
namespace {
template<class FromU>
struct rchar_defs {
	typedef FromU from_utils;
	typedef typename from_utils::char_type char_type;
	enum {
		char_size=from_utils::char_size,
		BUFFER_SIZE=6u // max UTF-8 (including invalid planes)
	};
};

template<
	class DefsT,
	std::size_t=DefsT::char_size
>
struct rchar_impl {
	static char32 read_char(
		std::istream& stream,
		char32 const replacement,
		Endian const endian
	) {
		typename DefsT::char_type
			buffer[DefsT::BUFFER_SIZE],
			*iter=buffer,
			*next;
		unsigned amt;
		char32 cp;
		IO::read_arithmetic(stream, buffer[0u], endian);
		if (!stream.good()) {
			//DUCT_DEBUG("rchar_impl<(defaults)>::read: !stream.good()");
			return replacement;
		}
		amt=DefsT::from_utils::required_first(buffer[0u]);
		if (amt) {
			IO::read_arithmetic_array(stream, buffer+1u, amt, endian);
			if (!stream.good()) {
				//DUCT_DEBUG("rchar_impl<(defaults)>::read: 2 !stream.good()");
				return replacement;
			}
		}
		// Make amt equal the entire number of units
		++amt;
		next=DefsT::from_utils::decode(iter, buffer+amt, cp, replacement);
		if (next==iter) {
			// Incomplete sequence; shouldn't occur due to final stream
			// goodness check
			DUCT_DEBUG("rchar_impl<(defaults)>::read: ics; curious!");
			return replacement;
		} else {
			return cp;
		}
	}
};

// Specialize for UTF-32
template<class DefsT>
struct rchar_impl<DefsT, 4u> {
	static char32 read_char(
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
			cp=replacement;
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
	- @a replacement if either the decoded code point was invalid or if there
	  was an error reading from the stream (see @c std::basic_istream::read()).
	@tparam FromU @c EncodingUtils specialization for decoding from the stream.
	@param stream Stream to read from.
	@param replacement Replacement code point; defaults to @c CHAR_SENTINEL.
	@param endian Endian to use when reading; defaults to @c Endian::SYSTEM
	(no swapping).
*/
template<
	class FromU,
	class DefsT=rchar_defs<FromU>
>
inline char32 read_char(
	std::istream& stream,
	char32 const replacement=CHAR_SENTINEL,
	Endian const endian=Endian::SYSTEM
) {
	return rchar_impl<DefsT>::read_char(stream, replacement, endian);
}

/** @cond INTERNAL */
namespace {
template<class ToU>
struct wchar_defs {
	typedef ToU to_utils;
	typedef typename to_utils::char_type char_type;
	enum {
		char_size=to_utils::char_size,
		BUFFER_SIZE=to_utils::max_units
	};
};

template<
	class DefsT,
	std::size_t=DefsT::char_size
>
struct wchar_impl {
	static std::size_t write_char(
		std::ostream& stream,
		char32 cp,
		unsigned const num,
		char32 const replacement,
		Endian const endian
	) {
		if (!DUCT_UNI_IS_CP_VALID(cp)) {
			if (CHAR_NULL==replacement || !DUCT_UNI_IS_CP_VALID(replacement)) {
				return 0u;
			} else {
				cp=replacement;
			}
		}
		typename DefsT::char_type
			out_buffer[DefsT::BUFFER_SIZE],
			*out_iter=out_buffer;
		out_iter=DefsT::to_utils::encode(cp, out_iter, CHAR_NULL);
		// Should not occur because both cp and replacement are checked for
		// invalidity before encoding
		if (out_iter==out_buffer) {
			DUCT_DEBUG(
				"wchar_impl<(def)>::write: out_iter==out_buffer; curious!");
			return 0u;
		} else {
			unsigned idx, amt=(out_iter-out_buffer);
			if (Endian::SYSTEM!=endian && 1u!=DefsT::char_size) {
				for (idx=0u; amt>idx; ++idx) {
					byte_swap_ref(out_buffer[idx]);
				}
			}
			unsigned i=num;
			while (i--) {
				// Using raw write instead of write_arithmetic_array() because
				// it would use an unnecessary touch of the stack
				IO::write(stream, out_buffer, amt*DefsT::char_size);
			}
			return num*amt;
		}
	}
};

// Specialize for UTF-32
template<class DefsT>
struct wchar_impl<DefsT, 4u> {
	static std::size_t write_char(
		std::ostream& stream, char32 cp,
		unsigned const num,
		char32 const replacement,
		Endian const endian
	) {
		if (!DUCT_UNI_IS_CP_VALID(cp)) {
			if (CHAR_NULL==replacement || !DUCT_UNI_IS_CP_VALID(replacement)) {
				return 0u;
			} else {
				cp=replacement;
			}
		}
		unsigned i=num;
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
	@returns The number of @a ToU code units written (will not be accurate if
	the stream write operation failed - see @c std::basic_ostream::write()).
	@tparam ToU @c EncodingUtils specialization for encoding to the stream.
	@param stream Destination stream.
	@param cp Code point to write.
	@param num Number of times to write @a cp; defaults to @c 1.
	@param replacement Replacement code point. If invalid or equal
	to @c CHAR_NULL (default) when @a cp is invalid, <strong>nothing will
	be written</strong> (returns @c 0).
	@param endian Endian to use when writing; defaults to @c Endian::SYSTEM
	(no swapping).
*/
template<
	class ToU,
	class DefsT=wchar_defs<ToU>
>
inline std::size_t write_char(
	std::ostream& stream,
	char32 const cp,
	unsigned const num=1u,
	char32 const replacement=CHAR_NULL,
	Endian const endian=Endian::SYSTEM
) {
	if (0u<num) {
		return wchar_impl<DefsT>
			::write_char(stream, cp, num, replacement, endian);
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
	typedef StringT string_type;
	typedef detail::string_traits<string_type> string_traits;
	typedef FromU from_utils;
	typedef typename string_traits::encoding_utils to_utils;
	enum {BUFFER_SIZE=512u};
};
} // anonymous namespace
/** @endcond */ // INTERNAL

/**
	Read sized string from a stream.
	@warning @a size is the number of code <strong>units</strong> to read
	in @a FromU's encoding, not the number of code <strong>points</strong>.
	@tparam FromU @c EncodingUtils specialization for decoding from the stream.
	@tparam StringT String type. The character size of this class will be used
	to determine its encoding.
	@param stream Source stream.
	@param size Size of the string to read, in @a FromU <strong>code
	units</strong>.
	@param[out] value Output value; result undefined if at some point a
	read operation failed (see @c std::basic_istream::read()).
	@param replacement Replacement code point. If invalid or equal
	to @c CHAR_NULL (default), invalid code points will be skipped rather
	than replaced.
	@param endian Endian to use when reading; defaults to @c Endian::SYSTEM
	(no swapping).
*/
template<
	class FromU,
	class StringT,
	class DefsT=rstr_defs<FromU, StringT>
>
void read_string(
	std::istream& stream,
	StringT& value,
	std::size_t size,
	char32 const replacement=CHAR_NULL,
	Endian const endian=Endian::SYSTEM
) {
	typename DefsT::from_utils::char_type
		// Extra space to easily deal with incomplete sequences (0<offset)
		// instead of doing a bunch of subtraction
		buffer[DefsT::BUFFER_SIZE+6u],
		*end, *iter, *next;
	typename DefsT::to_utils::char_type
		out_buffer[DefsT::BUFFER_SIZE],
		*out_iter=out_buffer;
	unsigned offset=0u, amt;
	char32 cp;
	value.clear();
	while (0u<size) {
		amt=DefsT::BUFFER_SIZE<size ? DefsT::BUFFER_SIZE : size;
		IO::read(stream, buffer+offset, amt*DefsT::from_utils::char_size);
		if (!stream.good()) {
			DUCT_DEBUG("read_string: !stream.good()");
			break;
		}
		end=buffer+offset+amt;
		if (Endian::SYSTEM!=endian && 1u!=DefsT::from_utils::char_size) {
			for (iter=buffer+offset; end>iter; ++iter) {
				byte_swap_ref(*iter);
			}
		}
		offset=0u;
		for (iter=buffer; end>iter; iter=next) {
			next=DefsT::from_utils::decode(iter, end, cp, replacement);
			if (next==iter) { // Incomplete sequence
				//offset=DefsT::from_utils::required_first_whole(*next);
				offset=end-next;
				DUCT_DEBUG("read_string: ics");
				break;
			}
			out_iter=DefsT::to_utils::encode(cp, out_iter, replacement);
			// Prevent output overrun
			if (DefsT::BUFFER_SIZE<=6u+(out_iter-out_buffer)) {
				value.append(out_buffer, out_iter);
				out_iter=out_buffer;
			}
		}
		size-=amt;
		if (0u!=offset) { // Handle incomplete sequence
			DUCT_DEBUGF(
				"read_string: ics - pos: %lu offset: %u iter: 0x%X left: %lu",
				static_cast<unsigned long>(next-buffer),
				offset,
				*next,
				static_cast<unsigned long>(size)
			);
			if (0u>=size) {
				// No sense pushing back if there's no more data to read
				break;
			} else {
				// Push the incomplete sequence to the beginning
				std::memcpy(buffer, next, offset);
			}
		}
	}
	// Flush if there's any data left in the buffer
	if (out_buffer!=out_iter) {
		DUCT_DEBUG("read_string: flush out_buffer");
		value.append(out_buffer, out_iter);
	}
	if (0u!=offset && CHAR_NULL!=replacement) {
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
	@warning @a size is the number of code <strong>units</strong> to read
	in @a StringT's encoding, not the number of code <strong>points</strong>.
	@tparam StringT String type. The character size of this class will be used
	to determine its encoding.
	@param stream Source stream.
	@param size Size of the string to read, in <strong>code units</strong>.
	@param[out] value Output value; result undefined if at some point a read
	operation failed (see std::basic_istream::read()).
	@param endian Endian to use when reading; defaults to @c Endian::SYSTEM
	(no swapping).
*/
template<
	class StringT,
	class DefsT=rstr_defs<typename
		detail::string_traits<StringT>::encoding_utils, StringT
	>
>
void read_string_copy(
	std::istream& stream,
	StringT& value,
	std::size_t size,
	Endian const endian=Endian::SYSTEM
) {
	typename DefsT::from_utils::char_type
		out_buffer[DefsT::BUFFER_SIZE],
		*out_iter;
	unsigned amt;
	value.clear();
	while (0u<size) {
		amt=DefsT::BUFFER_SIZE<size ? DefsT::BUFFER_SIZE : size;
		IO::read(stream, out_buffer, amt*DefsT::from_utils::char_size);
		if (!stream.good()) {
			DUCT_DEBUG("read_string_copy: !stream.good()");
			return;
		}
		if (Endian::SYSTEM!=endian && 1u!=DefsT::from_utils::char_size) {
			for (out_iter=out_buffer; out_buffer+amt>out_iter; ++out_iter) {
				byte_swap_ref(*out_iter);
			}
		}
		value.append(out_buffer, amt);
		size-=amt;
	}
}

/** @cond INTERNAL */
namespace {
template<
	class ToU,
	class StringT
>
struct wstr_defs {
	typedef StringT string_type;
	typedef detail::string_traits<string_type> string_traits;
	typedef ToU to_utils;
	typedef typename string_traits::encoding_utils from_utils;
	enum {BUFFER_SIZE=512u};
};
} // anonymous namespace
/** @endcond */ // INTERNAL

/**
	Write string to a stream.
	@returns The number of code units written in @a ToU's encoding; may not be
	accurate if @c std::basic_ostream::write() fails.
	@tparam ToU @c EncodingUtils specialization for encoding to the stream.
	@tparam StringT String type. The character size of this class will be used
	to determine its encoding.
	@param stream Destination stream.
	@param value String to write.
	@param replacement Replacement code point. If invalid or equal
	to @c CHAR_NULL (default), invalid code points will be skipped rather
	than replaced.
	@param endian Endian to use when writing; defaults to @c Endian::SYSTEM
	(no swapping).
*/
template<
	class ToU,
	class StringT,
	class DefsT=wstr_defs<ToU, StringT>
>
std::size_t write_string(
	std::ostream& stream,
	StringT const& value,
	char32 const replacement=CHAR_NULL,
	Endian const endian=Endian::SYSTEM
) {
	typename DefsT::to_utils::char_type
		out_buffer[DefsT::BUFFER_SIZE],
		*out_iter=out_buffer;
	typename DefsT::string_type::const_iterator
		in_iter, in_next;
	std::size_t units_written=0u;
	char32 cp;
	for (in_iter=value.cbegin(); value.cend()!=in_iter; in_iter=in_next) {
		in_next=DefsT::from_utils::decode(
			in_iter, value.cend(), cp, replacement);
		if (in_next==in_iter) { // Incomplete sequence
			DUCT_DEBUG("write_string: ics");
			break;
		}
		/*DUCT_DEBUGF(
			"write_string: in: %lu  out: %lu %p cp: %u 0x%X",
			static_cast<unsigned long>(in_next-value.cbegin()),
			static_cast<unsigned long>(out_iter-out_buffer),
			out_iter,
			cp,
			cp
		);*/
		out_iter=DefsT::to_utils::encode(cp, out_iter, replacement);
		// Prevent output overrun
		if (DefsT::BUFFER_SIZE<=6u+(out_iter-out_buffer)) {
			if (Endian::SYSTEM!=endian && 1u!=DefsT::to_utils::char_size) {
				for (auto iter=out_buffer; out_iter>iter; ++iter) {
					byte_swap_ref(*iter);
				}
			}
			IO::write(
				stream, out_buffer,
				DefsT::to_utils::char_size*(out_iter-out_buffer)
			);
			if (!stream.good()) {
				DUCT_DEBUG("write_string: !stream.good()");
				goto exit_f;
			}
			units_written+=out_iter-out_buffer;
			out_iter=out_buffer;
		}
	}
	// Flush if there's any data left in the buffer
	if (out_buffer!=out_iter) {
		if (Endian::SYSTEM!=endian && 1u!=DefsT::to_utils::char_size) {
			for (auto iter=out_buffer; out_iter>iter; ++iter) {
				byte_swap_ref(*iter);
			}
		}
		IO::write(
			stream, out_buffer,
			DefsT::to_utils::char_size*(out_iter-out_buffer)
		);
		if (!stream.good()) {
			DUCT_DEBUG("write_string: 2; !stream.good()");
			goto exit_f;
		}
		units_written+=out_iter-out_buffer;
	}

exit_f:
	return units_written;
}

/**
	Write string to a stream without checking for malformed or incomplete
	sequences.
	@returns The number of code units written in @a ToU's encoding; may not
	be accurate if @c std::basic_ostream::write() fails.
	@tparam StringT String type. The character size of this class will be used
	to determine its encoding.
	@param stream Destination stream.
	@param value String to write.
	@param endian Endian to use when writing; defaults to @c Endian::SYSTEM
	(no swapping).
*/
template<
	class StringT,
	class DefsT=wstr_defs<typename
		detail::string_traits<StringT>::encoding_utils, StringT
	>
>
std::size_t write_string_copy(
	std::ostream& stream,
	StringT const& value,
	Endian const endian=Endian::SYSTEM
) {
	typename DefsT::from_utils::char_type
		out_buffer[DefsT::BUFFER_SIZE],
		*out_iter;
	unsigned amt, size=value.size();
	typename StringT::const_iterator str_iter=value.cbegin();
	while (0u<size) {
		amt=DefsT::BUFFER_SIZE<size ? DefsT::BUFFER_SIZE : size;
		for (
			out_iter=out_buffer;
			out_buffer+amt>out_iter;
			++out_iter, ++str_iter
		) {
			if (Endian::SYSTEM!=endian && 1u!=DefsT::from_utils::char_size) {
				*out_iter=byte_swap(*str_iter);
			} else {
				*out_iter=*str_iter;
			}
		}
		IO::write(stream, out_buffer, amt*DefsT::from_utils::char_size);
		if (!stream.good()) {
			DUCT_DEBUG("write_string_copy: !stream.good()");
			goto exit_f;
		}
		size-=amt;
	}

exit_f:
	return size-value.size();
}

/** @} */ // end of name-group Unicode

/**
	Generic memory streambuf.
	@warning Reassigning the buffer will not clear stream state.
*/
template<typename CharT, typename TraitsT>
class basic_memstreambuf /*final*/
	: public std::basic_streambuf<CharT, TraitsT> {
private:
	typedef std::basic_streambuf<CharT, TraitsT> base_type;

public:
/** @name Types */ /// @{
	/** Character type. */
	typedef CharT char_type;
	/** Traits type. */
	typedef TraitsT traits_type;
	/** @c traits_type::int_type. */
	typedef typename traits_type::int_type int_type;
	/** @c traits_type::pos_type. */
	typedef typename traits_type::pos_type pos_type;
	/** @c traits_type::off_type. */
	typedef typename traits_type::off_type off_type;
/// @}

private:
	std::ios_base::openmode m_mode;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor (deleted). */
	basic_memstreambuf()=delete;
	/**
		Constructor with input buffer.
		@param buffer Data buffer.
		@param size Size of @a buffer.
		@param mode @c openmode for the streambuf; defaults to and
		forces @c std::ios_base::in; removes @c std::ios_base::out.
	*/
	basic_memstreambuf(
		void const* const buffer,
		std::size_t const size,
		std::ios_base::openmode const mode=std::ios_base::in
	)
		: base_type{}
		, m_mode{(mode&~std::ios_base::out)|std::ios_base::in}
	{
		assign(const_cast<void*>(buffer), size);
	}
	/**
		Constructor with input/output buffer.
		@param buffer Data buffer.
		@param size Size of @a buffer.
		@param mode @c openmode for the streambuf; defaults to and
		forces @c std::ios_base::out.
	*/
	basic_memstreambuf(
		void* const buffer,
		std::size_t const size,
		std::ios_base::openmode const mode=std::ios_base::out
	)
		: base_type{}
		, m_mode{mode|std::ios_base::out}
	{
		assign(buffer, size);
	}
	/** Copy constructor (deleted). */
	basic_memstreambuf(basic_memstreambuf const&)=delete;
	/** Move constructor. */
	basic_memstreambuf(basic_memstreambuf&&)=default;
	/** Destructor. */
	~basic_memstreambuf() override=default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator (deleted). */
	basic_memstreambuf& operator=(basic_memstreambuf const&)=delete;
	/** Move assignment operator. */
	basic_memstreambuf& operator=(basic_memstreambuf&&)=default;
/// @}

	/**
		Assign the streambuf's get and put areas.
		@note The current get and put positions will be reset.
		@param buffer Data buffer.
		@param size Size of @a buffer.
	*/
	void assign(void* const buffer, std::size_t const size) {
		if (nullptr==buffer) {
			this->setg(nullptr, nullptr, nullptr);
			this->setp(nullptr, nullptr);
		} else {
			char_type* const cbuf=reinterpret_cast<char_type*>(buffer);
			if (m_mode&std::ios_base::in) {
				this->setg(cbuf, cbuf, cbuf+size);
			}
			if (m_mode&std::ios_base::out) {
				this->setp(cbuf, cbuf+size);
			}
		}
	}

protected:
	/** @cond INTERNAL */
	pos_type seekoff(
		off_type off,
		std::ios_base::seekdir way,
		std::ios_base::openmode which=std::ios_base::in|std::ios_base::out
	) override {
		bool const
			do_in =m_mode&which&std::ios_base::in,
			do_out=m_mode&which&std::ios_base::out;
		char_type *beg=nullptr, *cur, *end;
		// NB: memstreambuf will only ever point to a single buffer.
		// m_mode reflects the active modes: if either do_in or do_out are
		// true, their respective pointers are non-null, so it doesn't
		// matter which one is picked.
		if (do_in) {
			// eback? stdlib is schizo
			beg=this->eback(); cur=this->gptr(); end=this->egptr();
		} else if (do_out) {
			beg=this->pbase(); cur=this->pptr(); end=this->epptr();
		}
		if (nullptr!=beg) {
			// std::ios_base::beg is just beg+off
			if 		(std::ios_base::cur==way) { off+=cur-beg; }
			else if (std::ios_base::end==way) { off+=end-beg; }
			if (0<=off && (end-beg)>=off) {
				if (do_in) { this->setg(beg, beg+off, end); }
				if (do_out) { setp_all(beg, beg+off, end); }
				return pos_type{off};
			}
		}
		return pos_type{off_type{-1}};
	}

	pos_type seekpos(
		pos_type pos,
		std::ios_base::openmode which=std::ios_base::in|std::ios_base::out
	) override {
		// pos_type should be std::streampos, which should be std::fpos<>,
		// which stores an off_type, which should be std::streamoff.
		// Pass to seekoff() instead of duplicating code.
		return seekoff(static_cast<off_type>(pos), std::ios_base::beg, which);
	}
	/** @endcond */ // INTERNAL

private:
	// The setp that should have been (for some arcane reason, setp
	// doesn't take a new current pointer like setg does).
	// Also, the stdlib hates us. pbump just _has to_ take an int
	// instead of off_type (it's even signed for you! come on!).
	void setp_all(char_type* const beg, char_type* cur, char_type* const end) {
		// May the hammermaestro have mercy on your femur(s)
		assert(beg<=cur && end>=cur);
		this->setp(beg, end);
		std::ptrdiff_t off=cur-beg;
		// std::ptrdiff_t can be 64 bits of signed goodness
		// while int could be only 32, which means we have
		// to chip away at it.
		while (std::numeric_limits<signed>::max()<off) {
			this->pbump(std::numeric_limits<signed>::max());
			off-=std::numeric_limits<signed>::max();
		}
		this->pbump(static_cast<signed>(off));
	}
};

/**
	Input memory stream.
	@sa basic_omemstream, basic_memstream, basic_memstreambuf
*/
template<typename CharT, typename TraitsT>
class basic_imemstream /*final*/
	: public std::basic_istream<CharT, TraitsT> {
private:
	typedef std::basic_istream<CharT, TraitsT> base_type;

public:
/** @name Types */ /// @{
	/** Character type. */
	typedef CharT char_type;
	/** Traits type. */
	typedef TraitsT traits_type;
	/** @c traits_type::int_type. */
	typedef typename traits_type::int_type int_type;
	/** @c traits_type::pos_type. */
	typedef typename traits_type::pos_type pos_type;
	/** @c traits_type::off_type. */
	typedef typename traits_type::off_type off_type;
	/** Memory buffer type. */
	typedef basic_memstreambuf<char_type, traits_type> membuf_type;
/// @}

private:
	membuf_type m_membuf;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor (deleted). */
	basic_imemstream()=delete;
	/**
		Constructor with buffer.
		@param buffer Data buffer.
		@param size Size of @c buffer.
		@param mode @c openmode for the stream; defaults to and
		forces @c std::ios_base::in; removes @c std::ios_base::out.
	*/
	basic_imemstream(
		void const* const buffer,
		std::size_t const size,
		std::ios_base::openmode const mode=std::ios_base::in
	)
		: base_type{}
		, m_membuf{buffer, size, (mode&~std::ios_base::out)|std::ios_base::in}
	{ this->init(&m_membuf); }
	/** Copy constructor (deleted). */
	basic_imemstream(basic_imemstream const&)=delete;
	/** Move constructor. */
	basic_imemstream(basic_imemstream&&)=default;
	/** Destructor. */
	~basic_imemstream() override=default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator (deleted). */
	basic_imemstream& operator=(basic_imemstream const&)=delete;
	/** Move assignment operator. */
	basic_imemstream& operator=(basic_imemstream&&)=default;
/// @}

	/**
		Get streambuf.
		@returns Pointer to the stream's streambuf (never @c nullptr).
	*/
	membuf_type* rdbuf() const { return const_cast<membuf_type*>(&m_membuf); }
};

/**
	Output memory stream.
	@sa basic_imemstream, basic_memstream, basic_memstreambuf
*/
template<typename CharT, typename TraitsT>
class basic_omemstream /*final*/
	: public std::basic_ostream<CharT, TraitsT> {
private:
	typedef std::basic_ostream<CharT, TraitsT> base_type;

public:
/** @name Types */ /// @{
	/** Character type. */
	typedef CharT char_type;
	/** Traits type. */
	typedef TraitsT traits_type;
	/** @c traits_type::int_type. */
	typedef typename traits_type::int_type int_type;
	/** @c traits_type::pos_type. */
	typedef typename traits_type::pos_type pos_type;
	/** @c traits_type::off_type. */
	typedef typename traits_type::off_type off_type;
	/** Memory buffer type. */
	typedef basic_memstreambuf<char_type, traits_type> membuf_type;
/// @}

private:
	membuf_type m_membuf;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor (deleted). */
	basic_omemstream()=delete;
	/**
		Constructor with buffer.
		@param buffer Data buffer.
		@param size Size of @c buffer.
		@param mode @c openmode for the stream; defaults to and
		forces @c std::ios_base::out; removes @c std::ios_base::in.
	*/
	basic_omemstream(
		void* const buffer,
		std::size_t const size,
		std::ios_base::openmode const mode=std::ios_base::out
	)
		: base_type{}
		, m_membuf{buffer, size, (mode&~std::ios_base::in)|std::ios_base::out}
	{ this->init(&m_membuf); }
	/** Copy constructor (deleted). */
	basic_omemstream(basic_omemstream const&)=delete;
	/** Move constructor. */
	basic_omemstream(basic_omemstream&&)=default;
	/** Destructor. */
	~basic_omemstream() override=default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator (deleted). */
	basic_omemstream& operator=(basic_omemstream const&)=delete;
	/** Move assignment operator. */
	basic_omemstream& operator=(basic_omemstream&&)=default;
/// @}

	/**
		Get streambuf.
		@returns Pointer to the stream's streambuf (never @c nullptr).
	*/
	membuf_type* rdbuf() const { return const_cast<membuf_type*>(&m_membuf); }
};

/**
	Input/output memory stream.
	@sa basic_imemstream, basic_omemstream, basic_memstreambuf
*/
template<typename CharT, typename TraitsT>
class basic_memstream /*final*/
	: public std::basic_iostream<CharT, TraitsT> {
private:
	typedef std::basic_iostream<CharT, TraitsT> base_type;

public:
/** @name Types */ /// @{
	/** Character type. */
	typedef CharT char_type;
	/** Traits type. */
	typedef TraitsT traits_type;
	/** @c traits_type::int_type. */
	typedef typename traits_type::int_type int_type;
	/** @c traits_type::pos_type. */
	typedef typename traits_type::pos_type pos_type;
	/** @c traits_type::off_type. */
	typedef typename traits_type::off_type off_type;
	/** Memory buffer type. */
	typedef basic_memstreambuf<char_type, traits_type> membuf_type;
/// @}

private:
	membuf_type m_membuf;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor (deleted). */
	basic_memstream()=delete;
	/**
		Constructor with buffer.
		@param buffer Data buffer.
		@param size Size of @c buffer.
		@param mode @c openmode for the stream; defaults to and
		forces @c std::ios_base::in and @c std::ios_base::out.
	*/
	basic_memstream(
		void* const buffer,
		std::size_t const size,
		std::ios_base::openmode const mode=std::ios_base::in|std::ios_base::out
	)
		: base_type{}
		, m_membuf{buffer, size, mode|std::ios_base::in|std::ios_base::out}
	{ this->init(&m_membuf); }
	/** Copy constructor (deleted). */
	basic_memstream(basic_memstream const&)=delete;
	/** Move constructor. */
	basic_memstream(basic_memstream&&)=default;
	/** Destructor. */
	~basic_memstream() override=default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator (deleted). */
	basic_memstream& operator=(basic_memstream const&)=delete;
	/** Move assignment operator. */
	basic_memstream& operator=(basic_memstream&&)=default;
/// @}

	/**
		Get streambuf.
		@returns Pointer to the stream's streambuf (never @c nullptr).
	*/
	membuf_type* rdbuf() const { return const_cast<membuf_type*>(&m_membuf); }
};

/**
	Encoding and endian stream context.
	@note Documentation is identical to the static functions. The default
	constructor will use the UTF-8 encoding and the system endian.
*/
class StreamContext /*final*/ {
private:
	Encoding m_encoding{Encoding::UTF8};
	Endian m_endian{Endian::SYSTEM};

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	StreamContext()=default;
	/**
		Constructor with encoding and default system endian.
		@param encoding An encoding.
	*/
	explicit StreamContext(Encoding const encoding)
		: m_encoding{encoding}
	{}
	/**
		Constructor with endian and default UTF-8 encoding.
		@param endian An endian.
	*/
	explicit StreamContext(Endian const endian)
		: m_endian{endian}
	{}
	/**
		Constructor with encoding and endian.
		@param encoding An encoding.
		@param endian An endian.
	*/
	StreamContext(Encoding const encoding, Endian const endian)
		: m_encoding{encoding}
		, m_endian{endian}
	{}
	/** Copy constructor. */
	StreamContext(StreamContext const&)=default;
	/** Move constructor. */
	StreamContext(StreamContext&&)=default;
	/** Destructor. */
	~StreamContext()=default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator. */
	StreamContext& operator=(StreamContext const&)=default;
	/** Move assignment operator. */
	StreamContext& operator=(StreamContext&&)=default;
/// @}

/** @name Properties */ /// @{
	/**
		Set encoding and endian.
		@param encoding An encoding.
		@param endian An endian.
	*/
	void set_properties(Encoding const encoding, Endian const endian) {
		m_encoding=encoding;
		m_endian=endian;
	}
	/**
		Set encoding and endian from another context.
		@param ctx StreamContext to copy.
	*/
	void set_properties(StreamContext const& ctx) {
		m_encoding=ctx.m_encoding;
		m_endian=ctx.m_endian;
	}
	/**
		Set encoding.
		@param encoding An encoding.
	*/
	void set_encoding(Encoding const encoding) { m_encoding=encoding; }
	/**
		Get encoding.
		@returns The current encoding.
	*/
	Encoding get_encoding() const { return m_encoding; }
	/**
		Set endian.
		@param endian An endian.
	*/
	void set_endian(Endian const endian) { m_endian=endian; }
	/**
		Get endian.
		@returns The current endian.
	*/
	Endian get_endian() const { return m_endian; }
/// @}

/** @name Raw data */ /// @{
	/** See @c duct::IO::read_arithmetic(). */
	template<typename T>
	void read_arithmetic(std::istream& stream, T& value) const {
		::duct::IO::read_arithmetic<T>(
			stream, value, m_endian);
	}
	/** See @c duct::IO::read_arithmetic(). */
	template<typename T>
	T read_arithmetic(std::istream& stream) const {
		return ::duct::IO::read_arithmetic<T>(
			stream, m_endian);
	}
	/** See @c duct::IO::read_arithmetic_array(). */
	template<typename T>
	void read_arithmetic_array(
		std::istream& stream,
		T* dest,
		std::size_t const count
	) const {
		::duct::IO::read_arithmetic_array<T>(
			stream, dest, count, m_endian);
	}

	/** See @c duct::IO::write_arithmetic(). */
	template<typename T>
	void write_arithmetic(std::ostream& stream, T const value) const {
		::duct::IO::write_arithmetic<T>(
			stream, value, m_endian);
	}
	/** See @c duct::IO::write_arithmetic_array(). */
	template<typename T>
	void write_arithmetic_array(
		std::ostream& stream,
		T const* const src,
		std::size_t const count
	) const {
		::duct::IO::write_arithmetic_array<T>(
			stream, src, count, m_endian);
	}
/// @}

/** @name Unicode */ /// @{
	/** See @c duct::IO::read_char(). */
	char32 read_char(
		std::istream& stream,
		char32 const replacement=CHAR_SENTINEL
	) const {
		switch (m_encoding) {
		case Encoding::UTF8:
			return ::duct::IO::read_char<UTF8Utils>(
				stream, replacement, m_endian);
		case Encoding::UTF16:
			return ::duct::IO::read_char<UTF16Utils>(
				stream, replacement, m_endian);
		case Encoding::UTF32:
			return ::duct::IO::read_char<UTF32Utils>(
				stream, replacement, m_endian);
		default:
			DUCT_DEBUG_ASSERT(false,
				"Somehow the context has an invalid encoding; shame on you!");
			return replacement;
		}
	}
	/** See @c duct::IO::write_char(). */
	std::size_t write_char(
		std::ostream& stream,
		char32 const cp,
		unsigned const num=1u,
		char32 const replacement=CHAR_NULL
	) const {
		switch (m_encoding) {
		case Encoding::UTF8:
			return ::duct::IO::write_char<UTF8Utils>(
				stream, cp, num, replacement, m_endian);
		case Encoding::UTF16:
			return ::duct::IO::write_char<UTF16Utils>(
				stream, cp, num, replacement, m_endian);
		case Encoding::UTF32:
			return ::duct::IO::write_char<UTF32Utils>(
				stream, cp, num, replacement, m_endian);
		default:
			DUCT_DEBUG_ASSERT(false,
				"Somehow the context has an invalid encoding; shame on you!");
			return 0;
		}
	}

	/** See @c duct::IO::read_string(). */
	template<class StringT>
	void read_string(
		std::istream& stream,
		StringT& value,
		std::size_t size,
		char32 const replacement=CHAR_NULL
	) const {
		switch (m_encoding) {
		case Encoding::UTF8:
			::duct::IO::read_string<UTF8Utils>(
				stream, value, size, replacement, m_endian);
			return;
		case Encoding::UTF16:
			::duct::IO::read_string<UTF16Utils>(
				stream, value, size, replacement, m_endian);
			return;
		case Encoding::UTF32:
			::duct::IO::read_string<UTF32Utils>(
				stream, value, size, replacement, m_endian);
			return;
		default:
			DUCT_DEBUG_ASSERT(false,
				"Somehow the context has an invalid encoding; shame on you!");
			return;
		}
	}
	/** See @c duct::IO::write_string(). */
	template<class StringT>
	std::size_t write_string(
		std::ostream& stream,
		StringT const& value,
		char32 const replacement=CHAR_NULL
	) const {
		switch (m_encoding) {
		case Encoding::UTF8:
			return ::duct::IO::write_string<UTF8Utils>(
				stream, value, replacement, m_endian);
		case Encoding::UTF16:
			return ::duct::IO::write_string<UTF16Utils>(
				stream, value, replacement, m_endian);
		case Encoding::UTF32:
			return ::duct::IO::write_string<UTF32Utils>(
				stream, value, replacement, m_endian);
		default:
			DUCT_DEBUG_ASSERT(false,
				"Somehow the context has an invalid encoding; shame on you!");
			return 0;
		}
	}
/// @}
};

/** @} */ // end of doc-group io

} // namespace IO
} // namespace duct

#endif // DUCT_IO_HPP_
