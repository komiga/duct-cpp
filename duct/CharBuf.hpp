/**
@file CharBuf.hpp
@brief Character buffer.

@author Timothy Howard
@copyright 2010-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include "./config.hpp"
#include "./aux.hpp"
#include "./char.hpp"
#include "./CharacterSet.hpp"
#include "./EncodingUtils.hpp"
#include "./StringUtils.hpp"

#include <cstdlib>
#include <type_traits>

namespace duct {

// Forward declarations
class CharBuf;

/**
	@addtogroup text
	@{
*/

/**
	Character buffer.
*/
class CharBuf final {
public:
/** @name Types */ /// @{
	/** Internal character type. */
	using char_type = char32;
/// @}

private:
	duct::aux::vector<char_type> m_buffer{};
	bool m_cached{false};
	u8string m_cache_string{};

public:
/** @name Constructors and destructor */ /// @{
	/** Construct empty. */
	CharBuf() = default;
	/**
		Construct with capacity.

		@param capacity Capacity of buffer.
	*/
	explicit
	CharBuf(
		std::size_t const capacity
	) {
		m_buffer.reserve(capacity);
	}

	/** Copy constructor. */
	CharBuf(CharBuf const&) = default;
	/** Move constructor. */
	CharBuf(CharBuf&&) = default;
	/** Destructor. */
	~CharBuf() = default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator. */
	CharBuf& operator=(CharBuf const&) = default;
	/** Move assignment operator. */
	CharBuf& operator=(CharBuf&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Get size.

		@returns The number of characters in the buffer.
		@sa capacity()
	*/
	std::size_t
	size() const noexcept {
		return m_buffer.size();
	}

	/**
		Get capacity.

		@returns The reserved size of the buffer.
		@sa size()
	*/
	std::size_t
	capacity() const noexcept {
		return m_buffer.capacity();
	}

	/**
		Check if the buffer is empty.
	*/
	bool
	empty() const noexcept {
		return m_buffer.empty();
	}
/// @}

/** @name Operations and comparison */ /// @{
	/**
		Reset.
	*/
	void
	reset() noexcept {
		m_cached = false;
		m_buffer.clear();
	}

	/**
		Cache buffer as string.

		@returns Reference to cached string.
	*/
	u8string const&
	cache() {
		if (!m_cached) {
			StringUtils::convert<UTF32Utils>(
				m_cache_string, m_buffer.cbegin(), m_buffer.cend()
			);
			m_cached = true;
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
	void
	push_back(
		char_type const cp
	) {
		if (DUCT_UNI_IS_CP_VALID(cp)) {
			grow();
			m_buffer.push_back(cp);
			m_cached = false;
		}
	}

	/**
		Compare all characters in the buffer to a character.

		@returns
		- @c true if all characters match the character;
		- @c false otherwise.
		@tparam CharT Character type; inferred from @a c.
		@param c Character to compare against.
	*/
	template<
		typename CharT
	>
	bool
	compare(
		CharT const c
	) const noexcept {
		for (char_type const x : m_buffer) {
			if (x != c) {
				return false;
			}
		}
		return true;
	}

	/**
		Compare buffer to a character set.

		@returns
		- @c true if all characters match a character
		  from @a char_set;
		- @c false otherwise.
		@param char_set Character set to compare against.
	*/
	bool
	compare(
		CharacterSet const& char_set
	) const noexcept {
		for (char_type const x : m_buffer) {
			if (!char_set.contains(x)) {
				return false;
			}
		}
		return true;
	}
/// @}

/** @name Extraction */ /// @{
	/**
		Convert buffer to a string.

		@note The explicit u8string-returning to_string() is more
		efficient if the buffer is often needed as a string.

		@returns Buffer converted to @a StringT.
		@tparam StringT String type to convert to. Encoding is
		inferred from the type's character size.
	*/
	template<
		class StringT
	>
	StringT
	to_string() const {
		StringT str;
		StringUtils::convert<UTF32Utils>(
			str, m_buffer.cbegin(), m_buffer.cend(), true
		);
		return str;
	}

	/**
		Convert buffer to a @c u8string.

		@note This will cache the current buffer as a u8string if
		needed. It is more efficient to use this than it is to
		directly convert to other string types.

		@returns Cache string.
	*/
	u8string
	to_string() {
		return cache();
	}

	/**
		Convert buffer to a string (by-ref).

		@note The explicit u8string-returning to_string() is more
		efficient if the buffer is often needed as a string.

		@param[out] str Output string.
		@param append Whether to append to @a str; defaults
		to @c false (@a str is cleared on entry).
	*/
	template<
		class StringT
	>
	void
	to_string(
		StringT& str,
		bool const append = false
	) const {
		StringUtils::convert<UTF32Utils>(
			str, m_buffer.cbegin(), m_buffer.cend(), append
		);
	}

	/**
		Convert the buffer to an arithmetic type with error return.

		@note @a value is guaranteed to be equal to @c T() if
		extraction failed.

		@returns
		- @c true if the buffer was convertible to @a T (@a value
		  is set); or
		- @c false otherwise (@a value equals @c T()).
		@tparam T An arithmetic type; inferred from @a value.
		@param[out] value Output value.
	*/
	template<
		typename T
	>
	bool
	to_arithmetic(
		T& value
	) {
		static_assert(
			std::is_arithmetic<T>::value,
			"T must be arithmetic"
		);
		cache();
		// FIXME: Clang 3.2 has a defect (with libstdc++ 4.6.3?) in
		// that it believes the istringstream copy ctor is being used
		// if aggregate initialization is used here.
		//
		// It oddly only occurs when inside of a function template.
		// GCC 4.6.3 doesn't even care.
		aux::istringstream stream(m_cache_string);
		stream >> value;
		if (stream.fail()) {
			value = T();
			return false;
		} else {
			return true;
		}
	}

	/**
		Convert the buffer to an arithmetic type.

		@returns The buffer converted to arithmetic type @a T.
		@tparam T An arithmetic type.
		@sa to_arithmetic(T&)
	*/
	template<
		typename T
	>
	T
	to_arithmetic() {
		static_assert(
			std::is_arithmetic<T>::value,
			"T must be arithmetic"
		);
		T val;
		to_arithmetic(val);
		return val;
	}
/// @}

private:
	void grow() {
		if (0 == capacity()) {
			m_buffer.reserve(64u);
		} else if (size() + 1 >= capacity()) {
			m_buffer.reserve(
				(16 >= capacity()) ? 64u : 2u * capacity()
			);
		}
	}
};

/** @} */ // end of doc-group text

} // namespace duct

