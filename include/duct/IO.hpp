/**
@file IO.hpp
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

I/O utilities and Standard Library iostream extensions.

@defgroup io I/O utilities and Standard Library iostream extensions
*/

// TODO: Unicode read/write variants for generic iterators (to supply support for narrow and wide string literals)

#ifndef DUCT_IO_HPP_
#define DUCT_IO_HPP_

#include <duct/config.hpp>
#include <duct/EndianUtils.hpp>
#include <duct/string.hpp>
#include <duct/detail/string_traits.hpp>
#include <duct/debug.hpp>

#include <iostream>
#include <type_traits>
#include <strings.h>

namespace duct {
namespace IO {

// Forward declarations
template<typename charT> class basic_mem_istreambuf;
template<typename charT> class basic_mem_ostreambuf;
template<typename charT> class basic_mem_iostreambuf;
class StreamContext;

/**
	@addtogroup io
	@warning
	All I/O functions defined here are "unsafe" in that they do not check stream state when executing operations; they assume a @c stream.good()==true state on entry and cannot guarantee it on return.
	@c ios::badbit and/or @c ios::failbit may be set on the stream after the operation has executed; see std::basic_istream::read() and std::basic_ostream::write().
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
	if (0>count) {
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
	static constexpr unsigned int BUFFER_SIZE=32;
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
	static constexpr unsigned int char_size=from_utils::char_size;
	static constexpr unsigned int BUFFER_SIZE=6u; // max UTF-8 (including invalid planes)
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
			DUCT_DEBUG("rchar_impl<(defaults)>::read: !stream.good()");
			return replacement;
		}
		amt=defsT::from_utils::required_first(buffer[0]);
		if (amt) {
			IO::read_arithmetic_array(stream, buffer+1u, amt, endian);
			if (!stream.good()) {
				DUCT_DEBUG("rchar_impl<(defaults)>::read: 2 !stream.good()");
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
			DUCT_DEBUG("rchar_impl<(default),4>::read: !stream.good()");
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
	static constexpr unsigned int char_size=to_utils::char_size;
	static constexpr unsigned int BUFFER_SIZE=4u; // max valid UTF-8
};

template<class defsT, std::size_t _size=defsT::char_size>
struct wchar_impl {
	static std::size_t write_char(std::ostream& stream, char32 cp, char32 const replacement, Endian const endian) {
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
		defsT::to_utils::encode(cp, out_iter, CHAR_NULL);
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
			IO::write(stream, out_buffer, amt*defsT::char_size); // Using raw write instead of write_arithmetic_array() because it would use an unnecessary touch of the stack
			return amt;
		}
	}
};

// Specialize for UTF-32
template<class defsT>
struct wchar_impl<defsT, 4> {
	static std::size_t write_char(std::ostream& stream, char32 cp, char32 const replacement, Endian const endian) {
		if (!DUCT_UNI_IS_CP_VALID(cp)) {
			if (CHAR_NULL==replacement || !DUCT_UNI_IS_CP_VALID(replacement)) {
				return 0;
			} else {
				cp=replacement;
			}
		}
		IO::write_arithmetic(stream, cp, endian);
		return sizeof(char32);
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
	@param replacement Replacement code point. If invalid or equal to @c CHAR_NULL (default) when @a cp is invalid, <strong>nothing will be written</strong> (returns @c 0).
	@param endian Endian to use when writing; defaults to @c Endian::SYSTEM (no swapping).
*/
template<class toU, class defsT=wchar_defs<toU> >
std::size_t write_char(std::ostream& stream, char32 const cp, char32 const replacement=CHAR_NULL, Endian const endian=Endian::SYSTEM) {
	return wchar_impl<defsT>::write_char(stream, cp, replacement, endian);
}

/** @cond INTERNAL */
namespace {
template<class fromU, class stringT>
struct rstr_defs {
	typedef stringT string_type;
	typedef detail::string_traits<string_type> string_traits;
	typedef fromU from_utils;
	typedef typename string_traits::encoding_utils to_utils;
	static constexpr unsigned int BUFFER_SIZE=512u;
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
			DUCT_DEBUGF("read_string: ics - pos: %lu offset: %u iter: 0x%X left: %ld", next-buffer, offset, *next, size);
			if (0>=size) { // No sense pushing back if there's no more data to read
				break;
			} else {
				memcpy(buffer, next, offset); // Push the incomplete sequence to the beginning
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
		DUCT_DEBUGF("read_string: eos; offset: %u size: %ld replacement: 0x%X", offset, size, replacement);
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
	static constexpr unsigned int BUFFER_SIZE=512u;
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
		//DUCT_DEBUGF("write_string: in: %lu  out: %lu %p cp: %u 0x%X", in_next-value.cbegin(), out_iter-out_buffer, out_iter, cp, cp);
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
	@{
*/

/**
	Generic read-only memory streambuf.
	@warning Re-assigning the buffer will not clear stream (good/bad) state.
*/
template<typename charT>
class basic_mem_istreambuf : public std::basic_streambuf<charT, std::char_traits<charT> > {
private:
	DUCT_DISALLOW_COPY_AND_ASSIGN(basic_mem_istreambuf<charT>);

public:
	/**
		Constructor.
		@param buffer Data buffer.
		@param size Size of @a buffer.
	*/
	basic_mem_istreambuf(void const* buffer, std::size_t const size) {
		assign(buffer, size);
	}

	/**
		Assign the streambuf's get area.
		@param buffer New data buffer.
		@param size Size of @a buffer.
	*/
	void assign(void const* buffer, std::size_t const size) {
		setg(
			const_cast<charT*>(reinterpret_cast<charT const*>(buffer)),
			const_cast<charT*>(reinterpret_cast<charT const*>(buffer)),
			const_cast<charT*>(reinterpret_cast<charT const*>(buffer))+size
		);
	}
};

/**
	Generic write-only memory streambuf.
	@warning Re-assigning the buffer will not clear stream (good/bad) state.
*/
template<typename charT>
class basic_mem_ostreambuf : public std::basic_streambuf<charT, std::char_traits<charT> > {
private:
	DUCT_DISALLOW_COPY_AND_ASSIGN(basic_mem_ostreambuf<charT>);

public:
	/**
		Constructor.
		@param buffer Data buffer.
		@param size Size of @a buffer.
	*/
	basic_mem_ostreambuf(void* buffer, std::size_t const size) {
		assign(buffer, size);
	}

	/**
		Assign the streambuf's put area.
		@param buffer New data buffer.
		@param size Size of @a buffer.
	*/
	void assign(void* buffer, std::size_t const size) {
		setp(reinterpret_cast<charT*>(buffer), reinterpret_cast<charT*>(buffer)+size);
	}
};

/**
	Generic memory streambuf.
	@warning Re-assigning the buffer will not clear stream (good/bad) state.
*/
template<typename charT>
class basic_mem_iostreambuf : public std::basic_streambuf<charT, std::char_traits<charT> > {
private:
	DUCT_DISALLOW_COPY_AND_ASSIGN(basic_mem_iostreambuf<charT>);

public:
	/**
		Constructor.
		@param buffer Data buffer.
		@param size Size of @a buffer.
	*/
	basic_mem_iostreambuf(void* buffer, std::size_t const size) {
		assign(buffer, size);
	}

	/**
		Assign the streambuf's put and get areas.
		@param buffer New data buffer.
		@param size Size of @a buffer.
	*/
	void assign(void* buffer, std::size_t const size) {
		setg(reinterpret_cast<charT*>(buffer), reinterpret_cast<charT*>(buffer), reinterpret_cast<charT*>(buffer)+size);
		setp(reinterpret_cast<charT*>(buffer), reinterpret_cast<charT*>(buffer)+size);
	}
};

/** Narrow read-only memory streambuf for @c istream. */
typedef basic_mem_istreambuf<char> mem_istreambuf;
/** Wide read-only memory streambuf for @c wistream. */
typedef basic_mem_istreambuf<wchar_t> wmem_istreambuf;

/** Narrow write-only memory streambuf for @c ostream. */
typedef basic_mem_ostreambuf<char> mem_ostreambuf;
/** Wide write-only memory streambuf for @c wostream. */
typedef basic_mem_ostreambuf<wchar_t> wmem_ostreambuf;

/** Narrow memory streambuf for @c iostream. */
typedef basic_mem_iostreambuf<char> mem_iostreambuf;
/** Wide memory streambuf for @c wiostream. */
typedef basic_mem_iostreambuf<wchar_t> wmem_iostreambuf;

/**
	Encoding and endian stream context.
	@note Documentation is identical to the static functions. Notice that no methods in StreamContext take an endian parameter nor an EncodingUtils template parameter.
*/
class StreamContext {
private:
	void operator=(StreamContext const&); // Disallow copy operator

public:
// ctor/dtor
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

// properties
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

// Raw data
	/** See @c duct::IO::read_arithmetic(). */
	template<typename T>
	inline void read_arithmetic(std::istream& stream, T& value) {
		::duct::IO::read_arithmetic(stream, value, m_endian);
	}
	/** See @c duct::IO::read_arithmetic(). */
	template<typename T>
	inline T read_arithmetic(std::istream& stream) {
		return ::duct::IO::read_arithmetic(stream, m_endian);
	}
	/** See @c duct::IO::read_arithmetic_array(). */
	template<typename T>
	inline void read_arithmetic_array(std::istream& stream, T* dest, std::size_t const count) {
		::duct::IO::read_arithmetic_array(stream, dest, count, m_endian);
	}

	/** See @c duct::IO::write_arithmetic(). */
	template<typename T>
	inline void write_arithmetic(std::ostream& stream, T const value) {
		::duct::IO::write_arithmetic(stream, value, m_endian);
	}
	/** See @c duct::IO::write_arithmetic_array(). */
	template<typename T>
	inline void write_arithmetic_array(std::ostream& stream, T const* src, std::size_t const count) {
		::duct::IO::write_arithmetic_array(stream, src, count, m_endian);
	}

// Unicode
	/** See @c duct::IO::read_char(). */
	char32 read_char(std::istream& stream, char32 const replacement=CHAR_SENTINEL) {
		switch (m_encoding) {
		case Encoding::UTF8:	return (::duct::IO::read_char<UTF8Utils>(stream, replacement, m_endian));
		case Encoding::UTF16:	return (::duct::IO::read_char<UTF16Utils>(stream, replacement, m_endian));
		case Encoding::UTF32:	return (::duct::IO::read_char<UTF32Utils>(stream, replacement, m_endian));
		default: DUCT_ASSERT(false, "Somehow the context has an invalid encoding; shame on you!"); return replacement;
	}}
	/** See @c duct::IO::write_char(). */
	std::size_t write_char(std::ostream& stream, char32 const cp, char32 const replacement=CHAR_NULL) {
		switch (m_encoding) {
		case Encoding::UTF8:	return (::duct::IO::write_char<UTF8Utils>(stream, cp, replacement, m_endian));
		case Encoding::UTF16:	return (::duct::IO::write_char<UTF16Utils>(stream, cp, replacement, m_endian));
		case Encoding::UTF32:	return (::duct::IO::write_char<UTF32Utils>(stream, cp, replacement, m_endian));
		default: DUCT_ASSERT(false, "Somehow the context has an invalid encoding; shame on you!"); return replacement;
	}}

	/** See @c duct::IO::read_string(). */
	template<class stringT>
	void read_string(std::istream& stream, stringT& value, std::size_t size, char32 const replacement=CHAR_NULL) {
		switch (m_encoding) {
		case Encoding::UTF8:	(::duct::IO::read_string<UTF8Utils>(stream, value, size, replacement, m_endian)); return;
		case Encoding::UTF16:	(::duct::IO::read_string<UTF16Utils>(stream, value, size, replacement, m_endian)); return;
		case Encoding::UTF32:	(::duct::IO::read_string<UTF32Utils>(stream, value, size, replacement, m_endian)); return;
		default: DUCT_ASSERT(false, "Somehow the context has an invalid encoding; shame on you!");
	}}
	/** See @c duct::IO::write_string(). */
	template<class stringT>
	std::size_t write_string(std::ostream& stream, stringT const& value, char32 const replacement=CHAR_NULL) {
		switch (m_encoding) {
		case Encoding::UTF8:	return (::duct::IO::write_string<UTF8Utils>(stream, value, replacement, m_endian));
		case Encoding::UTF16:	return (::duct::IO::write_string<UTF16Utils>(stream, value, replacement, m_endian));
		case Encoding::UTF32:	return (::duct::IO::write_string<UTF32Utils>(stream, value, replacement, m_endian));
		default: DUCT_ASSERT(false, "Somehow the context has an invalid encoding; shame on you!"); return 0;
	}}

private:
	Encoding m_encoding;
	Endian m_endian;
};

/** @} */ // end of name-group Helper classes
/** @} */ // end of doc-group io

} // namespace IO
} // namespace duct

#endif // DUCT_IO_HPP_
