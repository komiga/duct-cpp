/**
@file IO.hpp
@brief I/O utilities and Standard Library iostream extensions.

@author Tim Howard
@copyright 2010-2012 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

// TODO: Unicode read/write variants for generic iterators (to supply support for narrow and wide string literals)

#ifndef DUCT_IO_HPP_
#define DUCT_IO_HPP_

#include "./config.hpp"
#include "./debug.hpp"
#include "./string.hpp"
#include "./detail/string_traits.hpp"
#include "./EndianUtils.hpp"

#include <cstring>
#include <type_traits>
#include <limits>
#include <iostream>

namespace duct {
namespace IO {

// Forward declarations
template<typename charT, typename traitsT> class basic_memstreambuf;
template<typename charT, typename traitsT> class basic_imemstream;
template<typename charT, typename traitsT> class basic_omemstream;
template<typename charT, typename traitsT> class basic_memstream;
class StreamContext;

/**
	@defgroup io I/O utilities and Standard Library iostream extensions
	@details
	@warning All I/O functions defined here are "unsafe" in that they do not check stream state when executing operations; they assume a @c stream.good()==true state on entry and cannot guarantee it on return.
	@c ios::badbit and/or @c ios::failbit may be set on the stream after the operation has executed; see std::basic_istream::read() and std::basic_ostream::write().
	@{
*/

/**
	@name Utilities
	@{
*/

/**
	Get the size of a stream.
	@note This requires bidirectional seeking (e.g. @c memstream, @c std::ifstream, @c std::istringstream).
	@warning @c stream may be in an @c std::ios_base::failbit state upon return (in which case the return value will be @c 0) or <strong>existing error states may even be removed</strong>.
	@returns The size of the stream, 0 may mean failure.
	@param stream Stream to size up.
*/
template<typename charT, class traitsT>
std::size_t size(std::basic_istream<charT, traitsT>& stream) {
	if (stream.eof()) { // Already at eof: don't have to seek anywhere
		DUCT_DEBUG("duct::IO::size: eof() initial");
		stream.setstate(std::ios_base::eofbit); // Get rid of all other states
		auto const end=stream.tellg();
		if (decltype(end)(-1)!=end) {
			return static_cast<std::size_t>(end);
		} else {
			DUCT_DEBUG("duct::IO::size: -1==end");
		}
	} else {
		stream.clear(); // Get rid of all states
		auto const original=stream.tellg();
		if (decltype(original)(-1)!=original) { // If the stream will give its position
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
	return 0;
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
inline void read(std::istream& stream, void* dest, std::size_t const size) {
	stream.read(reinterpret_cast<char*>(dest), size);
}

/**
	Write raw data to a stream.
	@warning @a src must have a capacity of at least @a size bytes.
	@param stream Destination stream.
	@param src Data source.
	@param size Number of bytes to write.
*/
inline void write(std::ostream& stream, void const* const src, std::size_t const size) {
	stream.write(reinterpret_cast<char const*>(src), size);
}

/**
	Read arithmetic value (by-ref) from a stream.
	@tparam T Arithmetic value type; inferred from @a value.
	@param stream Source stream.
	@param[out] value Output value; result undefined if: the operation failed (see @c std::basic_istream::read()) and @c endian!=Endian::SYSTEM.
	@param endian Endian to use when writing; defaults to @c Endian::SYSTEM (no swapping).
*/
template<typename T>
void read_arithmetic(std::istream& stream, T& value, Endian const endian=Endian::SYSTEM) {
	static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
	stream.read(reinterpret_cast<char*>(&value), sizeof(T));
	byte_swap_ref_if(value, endian);
}

/**
	Read arithmetic value from a stream.
	@returns Output value; undefined if: the operation failed (see @c std::basic_istream::read()) and @c endian!=Endian::SYSTEM.
	@tparam T Arithmetic value type; inferred from @a value.
	@param stream Source stream.
	@param endian Endian to use when writing; defaults to @c Endian::SYSTEM (no swapping).
*/
template<typename T>
T read_arithmetic(std::istream& stream, Endian const endian=Endian::SYSTEM) {
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
	@param[out] dest Output array; result undefined if: the operation failed (see @c std::basic_istream::read()) and @c endian!=Endian::SYSTEM.
	@param count Number of elements to read.
	@param endian Endian to use when writing; defaults to @c Endian::SYSTEM (no swapping).
*/
template<typename T>
void read_arithmetic_array(std::istream& stream, T* dest, std::size_t const count, Endian const endian=Endian::SYSTEM) {
	static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
	if (0<count) {
		stream.read(reinterpret_cast<char*>(dest), sizeof(T)*count);
		if (Endian::SYSTEM!=endian && 1!=sizeof(T)) {
			for (unsigned int idx=0; count>idx; ++idx) {
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
	@param endian Endian to use when writing; defaults to @c Endian::SYSTEM (no swapping).
*/
template<typename T>
void write_arithmetic(std::ostream& stream, T value, Endian const endian=Endian::SYSTEM) {
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
	@param endian Endian to use when writing; defaults to @c Endian::SYSTEM (no swapping).
*/
template<typename T>
void write_arithmetic_array(std::ostream& stream, T const* src, std::size_t const count, Endian const endian=Endian::SYSTEM) {
	static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
	enum {BUFFER_SIZE=32u};
	T flipbuf[BUFFER_SIZE];
	if (count) {
		if (Endian::SYSTEM!=endian && 1!=sizeof(T)) {
			unsigned int chunk, idx;
			for (chunk=0; count>chunk; chunk+=idx) {
				for (idx=0; BUFFER_SIZE>idx && count>chunk+idx; ++idx) {
					flipbuf[idx]=byte_swap(src[chunk+idx]);
				}
				stream.write(reinterpret_cast<char const*>(src), sizeof(T)*idx);
				if (!stream.good()) {
					DUCT_DEBUG("write_arithmetic: !stream.good()");
					return;
				}
			}
		} else {
			stream.write(reinterpret_cast<char const*>(src), sizeof(T)*count);
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
template<class fromU>
struct rchar_defs {
	typedef fromU from_utils;
	typedef typename from_utils::char_type char_type;
	enum {
		char_size=from_utils::char_size,
		BUFFER_SIZE=6u // max UTF-8 (including invalid planes)
	};
};

template<class defsT, std::size_t _size=defsT::char_size>
struct rchar_impl {
	static char32 read_char(std::istream& stream, char32 const replacement, Endian const endian) {
		typename defsT::char_type
			buffer[defsT::BUFFER_SIZE],
			*iter=buffer,
			*next;
		unsigned int amt;
		char32 cp;
		IO::read_arithmetic(stream, buffer[0], endian);
		if (!stream.good()) {
			//DUCT_DEBUG("rchar_impl<(defaults)>::read: !stream.good()");
			return replacement;
		}
		amt=defsT::from_utils::required_first(buffer[0]);
		if (amt) {
			IO::read_arithmetic_array(stream, buffer+1u, amt, endian);
			if (!stream.good()) {
				//DUCT_DEBUG("rchar_impl<(defaults)>::read: 2 !stream.good()");
				return replacement;
			}
		}
		++amt; // Make amt equal the entire number of units
		next=defsT::from_utils::decode(iter, buffer+amt, cp, replacement);
		if (next==iter) { // Incomplete sequence; shouldn't occur due to final stream goodness check
			DUCT_DEBUG("rchar_impl<(defaults)>::read: ics; curious!");
			return replacement;
		} else {
			return cp;
		}
	}
};

// Specialize for UTF-32
template<class defsT>
struct rchar_impl<defsT, 4> {
	static char32 read_char(std::istream& stream, char32 const replacement, Endian const endian) {
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
	@returns The code point read from @a stream; or @a replacement if either the decoded code point was invalid or if there was an error reading from the stream (see @c std::basic_istream::read()).
	@tparam fromU @c EncodingUtils specialization for decoding from the stream.
	@param stream Stream to read from.
	@param replacement Replacement code point; defaults to @c CHAR_SENTINEL.
	@param endian Endian to use when reading; defaults to @c Endian::SYSTEM (no swapping).
*/
template<class fromU, class defsT=rchar_defs<fromU> >
char32 read_char(std::istream& stream, char32 const replacement=CHAR_SENTINEL, Endian const endian=Endian::SYSTEM) {
	return rchar_impl<defsT>::read_char(stream, replacement, endian);
}

/** @cond INTERNAL */
namespace {
template<class toU>
struct wchar_defs {
	typedef toU to_utils;
	typedef typename to_utils::char_type char_type;
	enum {
		char_size=to_utils::char_size,
		BUFFER_SIZE=to_utils::max_units
	};
};

template<class defsT, std::size_t _size=defsT::char_size>
struct wchar_impl {
	static std::size_t write_char(std::ostream& stream, char32 cp, unsigned int const num, char32 const replacement, Endian const endian) {
		if (!DUCT_UNI_IS_CP_VALID(cp)) {
			if (CHAR_NULL==replacement || !DUCT_UNI_IS_CP_VALID(replacement)) {
				return 0;
			} else {
				cp=replacement;
			}
		}
		typename defsT::char_type
			out_buffer[defsT::BUFFER_SIZE],
			*out_iter=out_buffer;
		out_iter=defsT::to_utils::encode(cp, out_iter, CHAR_NULL);
		if (out_iter==out_buffer) { // Should not occur because both cp and replacement are checked for invalidity before encoding
			DUCT_DEBUG("wchar_impl<(defaults)>::write: out_iter==out_buffer; curious!");
			return 0;
		} else {
			unsigned int idx, amt=(out_iter-out_buffer);
			if (Endian::SYSTEM!=endian && 1u!=defsT::char_size) {
				for (idx=0; amt>idx; ++idx) {
					byte_swap_ref(out_buffer[idx]);
				}
			}
			unsigned int i=num;
			while (i--) {
				IO::write(stream, out_buffer, amt*defsT::char_size); // Using raw write instead of write_arithmetic_array() because it would use an unnecessary touch of the stack
			}
			return num*amt;
		}
	}
};

// Specialize for UTF-32
template<class defsT>
struct wchar_impl<defsT, 4> {
	static std::size_t write_char(std::ostream& stream, char32 cp, unsigned int const num, char32 const replacement, Endian const endian) {
		if (!DUCT_UNI_IS_CP_VALID(cp)) {
			if (CHAR_NULL==replacement || !DUCT_UNI_IS_CP_VALID(replacement)) {
				return 0;
			} else {
				cp=replacement;
			}
		}
		unsigned int i=num;
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
	@returns The number of @a toU code units written (will not be accurate if the stream write operation failed - see @c std::basic_ostream::write()).
	@tparam toU @c EncodingUtils specialization for encoding to the stream.
	@param stream Destination stream.
	@param cp Code point to write.
	@param num Number of times to write @a cp; defaults to @c 1.
	@param replacement Replacement code point. If invalid or equal to @c CHAR_NULL (default) when @a cp is invalid, <strong>nothing will be written</strong> (returns @c 0).
	@param endian Endian to use when writing; defaults to @c Endian::SYSTEM (no swapping).
*/
template<class toU, class defsT=wchar_defs<toU> >
std::size_t write_char(std::ostream& stream, char32 const cp, unsigned int const num=1, char32 const replacement=CHAR_NULL, Endian const endian=Endian::SYSTEM) {
	if (0<num) {
		return wchar_impl<defsT>::write_char(stream, cp, num, replacement, endian);
	} else {
		return 0;
	}
}

/** @cond INTERNAL */
namespace {
template<class fromU, class stringT>
struct rstr_defs {
	typedef stringT string_type;
	typedef detail::string_traits<string_type> string_traits;
	typedef fromU from_utils;
	typedef typename string_traits::encoding_utils to_utils;
	enum {BUFFER_SIZE=512u};
};
} // anonymous namespace
/** @endcond */ // INTERNAL

/**
	Read sized string from a stream.
	@warning @a size is the number of code <strong>units</strong> to read in @a fromU's encoding, not the number of code <strong>points</strong>.
	@tparam fromU @c EncodingUtils specialization for decoding from the stream.
	@tparam stringT String type. The character size of this class will be used to determine its encoding.
	@param stream Source stream.
	@param size Size of the string to read, in @a fromU <strong>code units</strong>.
	@param[out] value Output value; result undefined if at some point a read operation failed (see @c std::basic_istream::read()).
	@param replacement Replacement code point. If invalid or equal to @c CHAR_NULL (default), invalid code points will be skipped rather than replaced.
	@param endian Endian to use when reading; defaults to @c Endian::SYSTEM (no swapping).
*/
template<class fromU, class stringT, class defsT=rstr_defs<fromU, stringT> >
void read_string(std::istream& stream, stringT& value, std::size_t size, char32 const replacement=CHAR_NULL, Endian const endian=Endian::SYSTEM) {
	typename defsT::from_utils::char_type
		buffer[defsT::BUFFER_SIZE+6], // Extra space to easily deal with incomplete sequences (0<offset) instead of doing a bunch of subtraction
		*end, *iter, *next;
	typename defsT::to_utils::char_type
		out_buffer[defsT::BUFFER_SIZE],
		*out_iter=out_buffer;
	unsigned int offset=0, amt;
	char32 cp;
	value.clear();
	while (0<size) {
		amt=defsT::BUFFER_SIZE<size ? defsT::BUFFER_SIZE : size;
		IO::read(stream, buffer+offset, amt*defsT::from_utils::char_size);
		if (!stream.good()) {
			DUCT_DEBUG("read_string: !stream.good()");
			break;
		}
		end=buffer+offset+amt;
		if (Endian::SYSTEM!=endian && 1!=defsT::from_utils::char_size) {
			for (iter=buffer+offset; end>iter; ++iter) {
				byte_swap_ref(*iter);
			}
		}
		offset=0;
		for (iter=buffer; end>iter; iter=next) {
			next=defsT::from_utils::decode(iter, end, cp, replacement);
			if (next==iter) { // Incomplete sequence
				//offset=defsT::from_utils::required_first_whole(*next);
				offset=end-next;
				DUCT_DEBUG("read_string: ics");
				break;
			}
			out_iter=defsT::to_utils::encode(cp, out_iter, replacement);
			if (defsT::BUFFER_SIZE<=6+(out_iter-out_buffer)) { // Prevent output overrun
				value.append(out_buffer, out_iter);
				out_iter=out_buffer;
			}
		}
		size-=amt;
		if (0!=offset) { // Handle incomplete sequence
			DUCT_DEBUGF("read_string: ics - pos: %lu offset: %u iter: 0x%X left: %lu", static_cast<unsigned long>(next-buffer), offset, *next, static_cast<unsigned long>(size));
			if (0>=size) { // No sense pushing back if there's no more data to read
				break;
			} else {
				std::memcpy(buffer, next, offset); // Push the incomplete sequence to the beginning
			}
		}
	}
	if (out_buffer!=out_iter) { // Flush if there's any data left in the buffer
		DUCT_DEBUG("read_string: flush out_buffer");
		value.append(out_buffer, out_iter);
	}
	if (0!=offset && CHAR_NULL!=replacement) { // End of specified size with a trailing incomplete sequence
		DUCT_DEBUG("read_string: eos with trailing ics");
		value.append(1, replacement);
	} else {
		DUCT_DEBUGF("read_string: eos; offset: %u size: %lu replacement: 0x%X", offset, static_cast<unsigned long>(size), replacement);
	}
}

/**
	Read sized string from a stream without checking for malformed or incomplete sequences.
	@warning @a size is the number of code <strong>units</strong> to read in @a stringT's encoding, not the number of code <strong>points</strong>.
	@tparam stringT String type. The character size of this class will be used to determine its encoding.
	@param stream Source stream.
	@param size Size of the string to read, in <strong>code units</strong>.
	@param[out] value Output value; result undefined if at some point a read operation failed (see std::basic_istream::read()).
	@param endian Endian to use when reading; defaults to @c Endian::SYSTEM (no swapping).
*/
template<class stringT, class defsT=rstr_defs<typename detail::string_traits<stringT>::encoding_utils, stringT> >
void read_string_copy(std::istream& stream, stringT& value, std::size_t size, Endian const endian=Endian::SYSTEM) {
	typename defsT::from_utils::char_type
		out_buffer[defsT::BUFFER_SIZE],
		*out_iter;
	unsigned int amt;
	value.clear();
	while (0<size) {
		amt=defsT::BUFFER_SIZE<size ? defsT::BUFFER_SIZE : size;
		IO::read(stream, out_buffer, amt*defsT::from_utils::char_size);
		if (!stream.good()) {
			DUCT_DEBUG("read_string_copy: !stream.good()");
			return;
		}
		if (Endian::SYSTEM!=endian && 1!=defsT::from_utils::char_size) {
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
template<class toU, class stringT>
struct wstr_defs {
	typedef stringT string_type;
	typedef detail::string_traits<string_type> string_traits;
	typedef toU to_utils;
	typedef typename string_traits::encoding_utils from_utils;
	enum {BUFFER_SIZE=512u};
};
} // anonymous namespace
/** @endcond */ // INTERNAL

/**
	Write string to a stream.
	@returns The number of code units written in @a toU's encoding; may not be accurate if std::basic_ostream::write() fails.
	@tparam toU @c EncodingUtils specialization for encoding to the stream.
	@tparam stringT String type. The character size of this class will be used to determine its encoding.
	@param stream Destination stream.
	@param value String to write.
	@param replacement Replacement code point. If invalid or equal to @c CHAR_NULL (default), invalid code points will be skipped rather than replaced.
	@param endian Endian to use when writing; defaults to @c Endian::SYSTEM (no swapping).
*/
template<class toU, class stringT, class defsT=wstr_defs<toU, stringT> >
std::size_t write_string(std::ostream& stream, stringT const& value, char32 const replacement=CHAR_NULL, Endian const endian=Endian::SYSTEM) {
	typename defsT::to_utils::char_type
		out_buffer[defsT::BUFFER_SIZE],
		*out_iter=out_buffer;
	typename defsT::string_type::const_iterator
		in_iter, in_next;
	std::size_t units_written=0;
	char32 cp;
	for (in_iter=value.cbegin(); value.cend()!=in_iter; in_iter=in_next) {
		in_next=defsT::from_utils::decode(in_iter, value.cend(), cp, replacement);
		if (in_next==in_iter) { // Incomplete sequence
			DUCT_DEBUG("write_string: ics");
			break;
		}
		//DUCT_DEBUGF("write_string: in: %lu  out: %lu %p cp: %u 0x%X", static_cast<unsigned long>(in_next-value.cbegin()), static_cast<unsigned long>(out_iter-out_buffer), out_iter, cp, cp);
		out_iter=defsT::to_utils::encode(cp, out_iter, replacement);
		if (defsT::BUFFER_SIZE<=6+(out_iter-out_buffer)) { // Prevent output overrun
			if (Endian::SYSTEM!=endian && 1!=defsT::to_utils::char_size) {
				for (auto iter=out_buffer; out_iter>iter; ++iter) {
					byte_swap_ref(*iter);
				}
			}
			IO::write(stream, out_buffer, defsT::to_utils::char_size*(out_iter-out_buffer));
			if (!stream.good()) {
				DUCT_DEBUG("write_string: !stream.good()");
				goto exit_f;
			}
			units_written+=out_iter-out_buffer;
			out_iter=out_buffer;
		}
	}
	if (out_buffer!=out_iter) { // Flush if there's any data left in the buffer
		if (Endian::SYSTEM!=endian && 1!=defsT::to_utils::char_size) {
			for (auto iter=out_buffer; out_iter>iter; ++iter) {
				byte_swap_ref(*iter);
			}
		}
		IO::write(stream, out_buffer, defsT::to_utils::char_size*(out_iter-out_buffer));
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
	Write string to a stream without checking for malformed or incomplete sequences.
	@returns The number of code units written in @a toU's encoding; may not be accurate if std::basic_ostream::write() fails.
	@tparam stringT String type. The character size of this class will be used to determine its encoding.
	@param stream Destination stream.
	@param value String to write.
	@param endian Endian to use when writing; defaults to @c Endian::SYSTEM (no swapping).
*/
template<class stringT, class defsT=wstr_defs<typename detail::string_traits<stringT>::encoding_utils, stringT> >
std::size_t write_string_copy(std::ostream& stream, stringT const& value, Endian const endian=Endian::SYSTEM) {
	typename defsT::from_utils::char_type
		out_buffer[defsT::BUFFER_SIZE],
		*out_iter;
	unsigned int amt, size=value.size();
	typename stringT::const_iterator str_iter=value.cbegin();
	while (0<size) {
		amt=defsT::BUFFER_SIZE<size ? defsT::BUFFER_SIZE : size;
		for (out_iter=out_buffer; out_buffer+amt>out_iter; ++out_iter, ++str_iter) {
			if (Endian::SYSTEM!=endian && 1!=defsT::from_utils::char_size) {
				*out_iter=byte_swap(*str_iter);
			} else {
				*out_iter=*str_iter;
			}
		}
		IO::write(stream, out_buffer, amt*defsT::from_utils::char_size);
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
	@name Helper classes
	@warning All memory stream buffer classes are incapable of automatically growing.
	@{
*/

/**
	Generic memory streambuf.
	@warning Reassigning the buffer will not clear stream state.
*/
template<typename charT, typename traitsT=std::char_traits<charT> >
class basic_memstreambuf : public std::basic_streambuf<charT, traitsT> {
public:
	typedef charT char_type; /**< Character type. */
	typedef traitsT traits_type; /**< Traits type. */
	typedef typename traits_type::pos_type pos_type; /**< Position type. */
	typedef typename traits_type::off_type off_type; /**< Offset type. */

private:
	typedef std::basic_streambuf<charT, traitsT> base_streambuf_type_;

	std::ios_base::openmode m_mode;

	DUCT_DISALLOW_COPY_AND_ASSIGN(basic_memstreambuf);

public:
	/**
		Constructor with input buffer.
		@param buffer Data buffer.
		@param size Size of @a buffer.
		@param mode @c openmode for the streambuf; defaults to and forces @c std::ios_base::in; removes @c std::ios_base::out.
	*/
	basic_memstreambuf(void const* const buffer, std::size_t const size, std::ios_base::openmode const mode=std::ios_base::in)
		: base_streambuf_type_()
		, m_mode((mode&~std::ios_base::out)|std::ios_base::in)
	{
		assign(const_cast<void*>(buffer), size);
	}
	/**
		Constructor with input/output buffer.
		@param buffer Data buffer.
		@param size Size of @a buffer.
		@param mode @c openmode for the streambuf; defaults to and forces @c std::ios_base::out.
	*/
	basic_memstreambuf(void* const buffer, std::size_t const size, std::ios_base::openmode const mode=std::ios_base::out)
		: base_streambuf_type_()
		, m_mode(mode|std::ios_base::out)
	{
		assign(buffer, size);
	}

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
			char_type* cbuf=reinterpret_cast<char_type*>(buffer);
			if (m_mode&std::ios_base::in) { this->setg(cbuf, cbuf, cbuf+size); }
			if (m_mode&std::ios_base::out) { this->setp(cbuf, cbuf+size); }
		}
	}

protected:
	/** @cond INTERNAL */
	virtual pos_type seekoff(off_type soff, std::ios_base::seekdir direction, std::ios_base::openmode mode=std::ios_base::in|std::ios_base::out) {
		pos_type ret_pos=pos_type(off_type(-1));
		bool const do_in=m_mode&std::ios_base::in && mode&std::ios_base::in;
		bool const do_out=m_mode&std::ios_base::out && mode&std::ios_base::out;
		char_type *beg=nullptr, *cur, *end;
		if (do_in) {
			beg=this->eback(); cur=this->gptr(); end=this->egptr();
		} else if (do_out) {
			beg=this->pbase(); cur=this->pptr(); end=this->epptr();
		}
		if (nullptr!=beg) {
			off_type new_off=soff;
			if (std::ios_base::cur==direction) {
				new_off+=cur-beg;
			} else if (std::ios_base::end==direction) {
				new_off+=end-beg;
			}
			if (0<=new_off && (end-beg)>=new_off) {
				if (do_in) { this->setg(beg, beg+new_off, end); }
				if (do_out) { priv_pmove(beg, end, new_off); }
				ret_pos=pos_type(new_off);
			}
		}
		return ret_pos;
	}

	virtual pos_type seekpos(pos_type spos, std::ios_base::openmode mode=std::ios_base::in|std::ios_base::out) {
		pos_type ret_pos=pos_type(off_type(-1));
		bool const do_in=m_mode&std::ios_base::in && mode&std::ios_base::in;
		bool const do_out=m_mode&std::ios_base::out && mode&std::ios_base::out;
		char_type *beg=nullptr, *end;
		if (do_in) {
			beg=this->eback(); end=this->egptr();
		} else if (do_out) {
			beg=this->pbase(); end=this->epptr();
		}
		if (nullptr!=beg) {
			off_type new_off=off_type(spos);
			if (0<=new_off && (end-beg)>=new_off) {
				if (do_in) { this->setg(beg, beg+new_off, end); }
				if (do_out) { priv_pmove(beg, end, new_off); }
				ret_pos=pos_type(new_off);
			}
		}
		return ret_pos;
	}
	/** @endcond */ // INTERNAL

private:
	void priv_pmove(char_type* const beg, char_type* const end, off_type soff) {
		this->setp(beg, end);
		while (std::numeric_limits<int>::max()<soff) {
			this->pbump(std::numeric_limits<int>::max());
			soff-=std::numeric_limits<int>::max();
		}
		this->pbump(soff);
	}
};

/** Narrow memory streambuf. */
typedef basic_memstreambuf<char> mem_streambuf;
/** Wide memory streambuf. */
typedef basic_memstreambuf<wchar_t> wmem_streambuf;

/**
	Input memory stream.
	@sa basic_omemstream, basic_memstream, basic_memstreambuf
*/
template<typename charT, typename traitsT=std::char_traits<charT> >
class basic_imemstream : public std::basic_istream<charT, traitsT> {
public:
	typedef charT char_type; /**< Character type. */
	typedef traitsT traits_type; /**< Traits type. */
	typedef basic_memstreambuf<char_type, traits_type> membuf_type; /**< Memory buffer type. */

private:
	typedef std::basic_istream<charT, traitsT> base_stream_type_;

	membuf_type m_membuf;

	DUCT_DISALLOW_COPY_AND_ASSIGN(basic_imemstream);

public:
	/**
		Constructor with buffer.
		@param buffer Data buffer.
		@param size Size of @c buffer.
		@param mode @c openmode for the stream; defaults to and forces @c std::ios_base::in; removes @c std::ios_base::out.
	*/
	basic_imemstream(void const* const buffer, std::size_t const size, std::ios_base::openmode const mode=std::ios_base::in)
		: base_stream_type_()
		, m_membuf(buffer, size, (mode&~std::ios_base::out)|std::ios_base::in)
	{ this->init(&m_membuf); }

	/**
		Destructor.
	*/
	virtual ~basic_imemstream() {}

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
template<typename charT, typename traitsT=std::char_traits<charT> >
class basic_omemstream : public std::basic_ostream<charT, traitsT> {
public:
	typedef charT char_type; /**< Character type. */
	typedef traitsT traits_type; /**< Traits type. */
	typedef basic_memstreambuf<char_type, traits_type> membuf_type; /**< Memory buffer type. */

private:
	typedef std::basic_ostream<charT, traitsT> base_stream_type_;

	membuf_type m_membuf;

	DUCT_DISALLOW_COPY_AND_ASSIGN(basic_omemstream);

public:
	/**
		Constructor with buffer.
		@param buffer Data buffer.
		@param size Size of @c buffer.
		@param mode @c openmode for the stream; defaults to and forces @c std::ios_base::out; removes @c std::ios_base::in.
	*/
	basic_omemstream(void* const buffer, std::size_t const size, std::ios_base::openmode const mode=std::ios_base::out)
		: base_stream_type_()
		, m_membuf(buffer, size, (mode&~std::ios_base::in)|std::ios_base::out)
	{ this->init(&m_membuf); }

	/**
		Destructor.
	*/
	virtual ~basic_omemstream() {}

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
template<typename charT, typename traitsT=std::char_traits<charT> >
class basic_memstream : public std::basic_iostream<charT, traitsT> {
public:
	typedef charT char_type; /**< Character type. */
	typedef traitsT traits_type; /**< Traits type. */
	typedef basic_memstreambuf<char_type, traits_type> membuf_type; /**< Memory buffer type. */

private:
	typedef std::basic_iostream<charT, traitsT> base_stream_type_;
	
	membuf_type m_membuf;

	DUCT_DISALLOW_COPY_AND_ASSIGN(basic_memstream);

public:
	/**
		Constructor with buffer.
		@param buffer Data buffer.
		@param size Size of @c buffer.
		@param mode @c openmode for the stream; defaults to and forces @c std::ios_base::in and @c std::ios_base::out.
	*/
	basic_memstream(void* const buffer, std::size_t const size, std::ios_base::openmode const mode=std::ios_base::in|std::ios_base::out)
		: base_stream_type_()
		, m_membuf(buffer, size, mode|std::ios_base::in|std::ios_base::out)
	{ this->init(&m_membuf); }

	/**
		Destructor.
	*/
	virtual ~basic_memstream() {}

	/**
		Get streambuf.
		@returns Pointer to the stream's streambuf (never @c nullptr).
	*/
	membuf_type* rdbuf() const { return const_cast<membuf_type*>(&m_membuf); }
};

/** Narrow input memory stream. */
typedef basic_imemstream<char> imemstream;
/** Wide input memory stream. */
typedef basic_imemstream<wchar_t> wimemstream;

/** Narrow output memory stream. */
typedef basic_omemstream<char> omemstream;
/** Wide output memory stream. */
typedef basic_omemstream<wchar_t> womemstream;

/** Narrow input/output memory stream. */
typedef basic_memstream<char> memstream;
/** Wide input/output memory stream. */
typedef basic_memstream<wchar_t> wmemstream;

/**
	Encoding and endian stream context.
	@note Documentation is identical to the static functions. Notice that no methods in StreamContext take an endian parameter nor an EncodingUtils template parameter.
*/
class StreamContext {
private:
	Encoding m_encoding;
	Endian m_endian;

	void operator=(StreamContext const&); // Disallow copy operator

public:
/** @name Constructors */ /// @{
	/**
		Default constructor with UTF-8 encoding and system endian.
	*/
	StreamContext()
		: m_encoding(Encoding::UTF8)
		, m_endian(Endian::SYSTEM)
	{}
	/**
		Constructor with encoding and default system endian.
		@param encoding An encoding.
	*/
	explicit StreamContext(Encoding const encoding)
		: m_encoding(encoding)
		, m_endian(Endian::SYSTEM)
	{}
	/**
		Constructor with endian and default UTF-8 encoding.
		@param endian An endian.
	*/
	explicit StreamContext(Endian const endian)
		: m_encoding(Encoding::UTF8)
		, m_endian(endian)
	{}
	/**
		Constructor with encoding and endian.
		@param encoding An encoding.
		@param endian An endian.
	*/
	StreamContext(Encoding const encoding, Endian const endian)
		: m_encoding(encoding)
		, m_endian(endian)
	{}
	/**
		Copy constructor.
		@param other StreamContext to copy.
	*/
	StreamContext(StreamContext const& other)
		: m_encoding(other.m_encoding)
		, m_endian(other.m_endian)
	{}
/// @}

/** @name Properties */ /// @{
	/**
		Set encoding and endian.
		@param encoding An encoding.
		@param endian An endian.
	*/
	inline void set_properties(Encoding const encoding, Endian const endian) {
		m_encoding=encoding;
		m_endian=endian;
	}
	/**
		Set encoding and endian from another context.
		@param ctx StreamContext to copy.
	*/
	inline void set_properties(StreamContext const& ctx) {
		m_encoding=ctx.m_encoding;
		m_endian=ctx.m_endian;
	}
	/**
		Set encoding.
		@param encoding An encoding.
	*/
	inline void set_encoding(Encoding const encoding) { m_encoding=encoding; }
	/**
		Get encoding.
		@returns The current encoding.
	*/
	inline Encoding get_encoding() const { return m_encoding; }
	/**
		Set endian.
		@param endian An endian.
	*/
	inline void set_endian(Endian const endian) { m_endian=endian; }
	/**
		Get endian.
		@returns The current endian.
	*/
	inline Endian get_endian() const { return m_endian; }
/// @}

/** @name Raw data */ /// @{
	/** See @c duct::IO::read_arithmetic(). */
	template<typename T>
	inline void read_arithmetic(std::istream& stream, T& value) const {
		::duct::IO::read_arithmetic<T>(stream, value, m_endian);
	}
	/** See @c duct::IO::read_arithmetic(). */
	template<typename T>
	inline T read_arithmetic(std::istream& stream) const {
		return ::duct::IO::read_arithmetic<T>(stream, m_endian);
	}
	/** See @c duct::IO::read_arithmetic_array(). */
	template<typename T>
	inline void read_arithmetic_array(std::istream& stream, T* dest, std::size_t const count) const {
		::duct::IO::read_arithmetic_array<T>(stream, dest, count, m_endian);
	}

	/** See @c duct::IO::write_arithmetic(). */
	template<typename T>
	inline void write_arithmetic(std::ostream& stream, T const value) const {
		::duct::IO::write_arithmetic<T>(stream, value, m_endian);
	}
	/** See @c duct::IO::write_arithmetic_array(). */
	template<typename T>
	inline void write_arithmetic_array(std::ostream& stream, T const* src, std::size_t const count) const {
		::duct::IO::write_arithmetic_array<T>(stream, src, count, m_endian);
	}
/// @}

/** @name Unicode */ /// @{
	/** See @c duct::IO::read_char(). */
	char32 read_char(std::istream& stream, char32 const replacement=CHAR_SENTINEL) const {
		switch (m_encoding) {
		case Encoding::UTF8:	return (::duct::IO::read_char<UTF8Utils>(stream, replacement, m_endian));
		case Encoding::UTF16:	return (::duct::IO::read_char<UTF16Utils>(stream, replacement, m_endian));
		case Encoding::UTF32:	return (::duct::IO::read_char<UTF32Utils>(stream, replacement, m_endian));
		default: DUCT_DEBUG_ASSERT(false, "Somehow the context has an invalid encoding; shame on you!"); return replacement;
	}}
	/** See @c duct::IO::write_char(). */
	std::size_t write_char(std::ostream& stream, char32 const cp, unsigned int const num=1, char32 const replacement=CHAR_NULL) const {
		switch (m_encoding) {
		case Encoding::UTF8:	return (::duct::IO::write_char<UTF8Utils>(stream, cp, num, replacement, m_endian));
		case Encoding::UTF16:	return (::duct::IO::write_char<UTF16Utils>(stream, cp, num, replacement, m_endian));
		case Encoding::UTF32:	return (::duct::IO::write_char<UTF32Utils>(stream, cp, num, replacement, m_endian));
		default: DUCT_DEBUG_ASSERT(false, "Somehow the context has an invalid encoding; shame on you!"); return replacement;
	}}

	/** See @c duct::IO::read_string(). */
	template<class stringT>
	void read_string(std::istream& stream, stringT& value, std::size_t size, char32 const replacement=CHAR_NULL) const {
		switch (m_encoding) {
		case Encoding::UTF8:	(::duct::IO::read_string<UTF8Utils>(stream, value, size, replacement, m_endian)); return;
		case Encoding::UTF16:	(::duct::IO::read_string<UTF16Utils>(stream, value, size, replacement, m_endian)); return;
		case Encoding::UTF32:	(::duct::IO::read_string<UTF32Utils>(stream, value, size, replacement, m_endian)); return;
		default: DUCT_DEBUG_ASSERT(false, "Somehow the context has an invalid encoding; shame on you!");
	}}
	/** See @c duct::IO::write_string(). */
	template<class stringT>
	std::size_t write_string(std::ostream& stream, stringT const& value, char32 const replacement=CHAR_NULL) const {
		switch (m_encoding) {
		case Encoding::UTF8:	return (::duct::IO::write_string<UTF8Utils>(stream, value, replacement, m_endian));
		case Encoding::UTF16:	return (::duct::IO::write_string<UTF16Utils>(stream, value, replacement, m_endian));
		case Encoding::UTF32:	return (::duct::IO::write_string<UTF32Utils>(stream, value, replacement, m_endian));
		default: DUCT_DEBUG_ASSERT(false, "Somehow the context has an invalid encoding; shame on you!"); return 0;
	}}
/// @}
};

/** @} */ // end of name-group Helper classes
/** @} */ // end of doc-group io

} // namespace IO
} // namespace duct

#endif // DUCT_IO_HPP_
