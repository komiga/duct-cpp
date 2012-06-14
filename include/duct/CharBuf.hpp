/**
@file CharBuf.hpp
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

duct++ CharBuf class.
*/

#ifndef DUCT_CHARBUF_HPP_
#define DUCT_CHARBUF_HPP_

#include <duct/config.hpp>
#include <duct/char.hpp>
#include <duct/EncodingUtils.hpp>
#include <duct/StringUtils.hpp>
#include <duct/CharacterSet.hpp>

#include <type_traits>
#include <sstream>
#include <cstdlib>

namespace duct {

/**
	@addtogroup string
	@{
*/

// Forward declarations
class CharBuf;

/**
	Character buffer.
*/
class CharBuf /*final*/ {
public:
	/** Internal character type. */
	typedef char32 char_type;

public:
/** @name Constructors */ /// @{
	/**
		Construct empty.
	*/
	CharBuf()
		: m_buffer()
		, m_cached(false)
		, m_cache_string()
	{}
	/**
		Construct with capacity.
		@param capacity Capacity of buffer.
	*/
	CharBuf(std::size_t const capacity)
		: m_buffer()
		, m_cached(false)
		, m_cache_string()
	{
		m_buffer.reserve(capacity);
	}
/// @}

/** @name Properties */ /// @{
	/**
		Get size.
		@returns The number of characters in the buffer.
		@sa get_capacity().
	*/
	inline std::size_t get_size() const { return m_buffer.size(); }
	/**
		Get capacity.
		@returns The reserved size of the buffer.
		@sa get_size().
	*/
	inline std::size_t get_capacity() const { return m_buffer.capacity(); }
/// @}

/** @name Operations and comparison */ /// @{
	/**
		Reset.
	*/
	void reset() {
		m_cached=false;
		m_buffer.clear();
	}

	/**
		Cache buffer as string.
		@returns Reference to cached string.
	*/
	u8string const& cache() {
		if (!m_cached) {
			StringUtils::convert<UTF32Utils>(m_cache_string, m_buffer.cbegin(), m_buffer.cend());
			m_cached=true;
		}
		return m_cache_string;
	}

	/**
		Append code point to the end of the buffer.
		@warning Invalid code points are ignored.
		@warning An exception may be thrown by internal resizing.
		@note The cached string will be invalidated by this operation.
		@param cp Code point to insert.
	*/
	void push_back(char_type const cp) {
		if (DUCT_UNI_IS_CP_VALID(cp)) {
			grow();
			m_buffer.push_back(cp);
			m_cached=false;
		}
	}

	/**
		Compare all characters in the buffer to a character.
		@returns @c true if all characters match the character; @c false otherwise.
		@tparam charT Character type; inferred from @a c.
		@param c Character to compare against.
	*/
	template<typename charT>
	bool compare(charT const c) const {
		for (unsigned int i=0; get_size()>i; ++i) {
			if (m_buffer[i]!=c) {
				return false;
			}
		}
		return true;
	}
	/**
		Compare buffer to a character set.
		@returns @c true if all characters match a character from @a char_set; @c false otherwise.
		@param char_set Ccharacter set to compare against.
	*/
	bool compare(CharacterSet const& char_set) const {
		for (unsigned int i=0; get_size()>i; ++i) {
			if (!char_set.contains(m_buffer[i])) {
				return false;
			}
		}
		return true;
	}
/// @}

/** @name Output */ /// @{
	/**
		Convert buffer to a string.
		@returns Cache string converted to @a stringT.
		@tparam stringT String type to convert to. Encoding is inferred from the type's character size.
	*/
	template<class stringT>
	stringT to_string() {
		stringT str;
		StringUtils::convert<UTF32Utils>(str, m_buffer.cbegin(), m_buffer.cend(), true);
		return str;
	}
	/**
		Convert buffer to a string (by-ref).
		@param[out] str Output string.
		@param append Whether to append to @a str; defaults to @c false (@a str is cleared on entry).
	*/
	template<class stringT>
	void to_string(stringT& str, bool append=false) {
		StringUtils::convert<UTF32Utils>(str, m_buffer.cbegin(), m_buffer.cend(), append);
	}

	/**
		Convert the buffer to an arithmetic type.
		@returns The buffer converted to arithmetic type @a T.
		@tparam T An arithmetic type.
		@sa to_arithmetic(T&)
	*/
	template<typename T>
	T to_arithmetic() {
		static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
		T val;
		to_arithmetic(val);
		return val;
	}
	/**
		Convert the buffer to a 32-bit integer with error return.
		@note @a value is guaranteed to be equal to @c T() if extraction failed.
		@returns @c true if the buffer was convertible to @a T (@a value is set); @c false otherwise (@a value equals @c T(0)).
		@tparam T An arithmetic type; inferred from @a value.
		@param[out] value Output value.
	*/
	template<typename T>
	bool to_arithmetic(T& value) {
		static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
		cache();
		std::istringstream stream(m_cache_string);
		stream>>value;
		if (stream.good()) {
			return true;
		} else {
			value=T();
			return false;
		}
	}
/// @}

private:
	DUCT_DISALLOW_COPY_AND_ASSIGN(CharBuf);

	void grow() {
		if (0==get_capacity()) {
			m_buffer.reserve(64u);
		} else if (get_size()+1>=get_capacity()) {
			m_buffer.reserve(2*get_capacity());
		}
	}

private:
	std::vector<char_type> m_buffer;
	bool m_cached;
	u8string m_cache_string;
};

/** @} */ // end of doc-group string

} // namespace duct

#endif // DUCT_CHARBUF_HPP_
