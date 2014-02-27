/**
@file CharacterRange.hpp
@brief CharacterRange.

@author Timothy Howard
@copyright 2010-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_CHARACTERRANGE_HPP_
#define DUCT_CHARACTERRANGE_HPP_

#include "./config.hpp"
#include "./char.hpp"
#include "./string.hpp"
#include "./detail/string_traits.hpp"

namespace duct {

// Forward declarations
class CharacterRange;

/**
	@addtogroup text
	@{
*/
/**
	@name Comparison helpers
	@{
*/

/**
	A range of characters.

	@note The range is inclusive and can thus map the entire UTF-32
	space.
*/
class CharacterRange final {
private:
	char32 m_first;
	char32 m_last;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Construct null.

		@note Equivalent to @c CharacterRange('\0').
	*/
	CharacterRange() noexcept
		: m_first{'\0'}
		, m_last{'\0'}
	{}

	/**
		Construct with single code point.

		@note Equivalent to @c CharacterRange(cp, 0).

		@param cp Code point.
	*/
	explicit
	CharacterRange(
		char32 const cp
	) noexcept
		: m_first{cp}
		, m_last{cp}
	{}

	/**
		Construct with range.

		@note A @a length of 0 will still match @a first.

		@param first First code point of the range.
		@param length Length of the range.
	*/
	CharacterRange(
		char32 const first,
		unsigned const length
	) noexcept
		: m_first{first}
		, m_last{first + length}
	{}

	/** Copy constructor. */
	CharacterRange(CharacterRange const&) = default;
	/** Move constructor. */
	CharacterRange(CharacterRange&&) = default;
	/** Destructor. */
	~CharacterRange() = default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator. */
	CharacterRange& operator=(CharacterRange const&) = default;
	/** Move assignment operator. */
	CharacterRange& operator=(CharacterRange&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Set first code point.

		@param first New first code point.
	*/
	void
	set_first(
		char32 const first
	) noexcept {
		m_first = first;
	}

	/**
		Get first code point.

		@returns The first code point in range.
	*/
	char32
	first() const noexcept {
		return m_first;
	}

	/**
		Set last code point.

		@param last New last code point.
	*/
	void
	set_last(
		char32 const last
	) noexcept {
		m_last = last;
	}

	/**
		Get last code point.

		@returns The last code point in range.
	*/
	char32
	last() const noexcept {
		return m_last;
	}
/// @}

/** @name Comparison */ /// @{
	/**
		Check if the range contains a code point.

		@returns
		- @c true if @a cp is contained, or
		- @c false if it was not.
		@param cp Code point to test.
	*/
	bool
	contains(
		char32 const cp
	) const noexcept {
		return cp >= m_first && cp <= m_last;
	}

	/**
		Compare against another range.

		@returns
		- @c -1 if @a other is greater than @c this,
		- @c 1 if @c this is greater than @a other, or
		- @c 0 if @c this and @a other are equivalent.
		@param other Range to compare against.
	*/
	signed
	compare(
		CharacterRange const& other
	) const noexcept {
		signed const sd = m_last - m_first;
		signed const od = other.m_last - other.m_first;
		if (sd < od) {
			return -1;
		} else if (sd > od) {
			return 1;
		}
		if (m_first < other.m_first) {
			return -1;
		} else if (m_first > other.m_first) {
			return 1;
		}
		return 0;
	}

	/**
		Check if a range intersects with this one.

		@returns
		- @c true if the ranges intersect, or
		- @c false if they do not.
		@param other Range to compare against.
	*/
	bool
	intersects(
		CharacterRange const& other
	) const noexcept {
		if (0 == compare(other)) {
			return true;
		}
		if (m_last == (other.m_first - 1)) {
			return true;
		} else if ((m_first - 1) == other.m_last) {
			return true;
		}
		return !(m_first > other.m_last || m_last < other.m_first);
	}

	/**
		Find the first matching code point in a sequence.

		@returns
		- Iterator of the first matching code point in the sequence;
		  or
		- @a end if the sequence had no matching code points.
		@tparam StringU Encoding utilities.
		@tparam InputIt Input iterator type; inferred from @a pos.
		@param pos Start of sequence. Behavior is undefined if this
		does not point to the lead unit for a code unit sequence.
		@param end End of sequence.
	*/
	template<
		class StringU,
		typename InputIt
	>
	InputIt
	sequence_find(
		InputIt pos,
		InputIt const end
	) const noexcept {
		char32 cp;
		InputIt next;
		for (; end != pos; pos = next) {
			next = StringU::decode(pos, end, cp, CHAR_SENTINEL);
			if (next == pos) { // Incomplete sequence
				return end;
			} else if (CHAR_SENTINEL != cp && contains(cp)) {
				return pos;
			}
		}
		return end;
	}
	/**
		Find the first matching code point in a string.

		@returns
		- Iterator of the first matching code point in @a str, or
		- @c str.cend() if the sequence had no matching code points.
		@tparam StringT String type; inferred from @a str.
		@param str String to search.
		@param pos Start iterator. Behavior is undefined if this does
		not point to the lead unit for a code unit sequence.
	*/
	template<
		class StringT,
		class StringU = typename detail::string_traits<StringT>::encoding_utils,
		class StringI = typename StringT::const_iterator
	>
	StringI
	find(
		StringT const& str,
		StringI pos
	) const noexcept {
		return sequence_find<StringU>(pos, str.cend());
	}

	/**
		Check if all code points in a sequence match.

		@returns
		- @c true if all code points in the sequence match, or
		- @c false if one does not.
		@tparam StringU Encoding utilities.
		@tparam InputIt Input iterator type; inferred from @a pos.
		@param pos Start of sequence. Behavior is undefined if this
		does not point to the lead unit for a code unit sequence.
		@param end End of sequence.
	*/
	template<
		class StringU,
		typename InputIt
	>
	bool
	sequence_matches(
		InputIt pos,
		InputIt const end
	) const noexcept {
		char32 cp;
		InputIt next;
		for (; end != pos; pos = next) {
			next = StringU::decode(pos, end, cp, CHAR_SENTINEL);
			// Incomplete sequence || bad sequence || not a match
			if (next == pos || CHAR_SENTINEL == cp || !contains(cp)) {
				return false;
			}
		}
		return true;
	}

	/**
		Check if all code points in a string match.

		@returns
		- @c true if all code points in the sequence match; or
		- @c false if one does not.
		@tparam StringT String type; inferred from @a str.
		@param str String to search.
		@param pos Start iterator. Behavior is undefined if @a pos
		does not point to the lead unit for a code unit sequence.
	*/
	template<
		class StringT,
		class StringU = typename detail::string_traits<StringT>::encoding_utils,
		class StringI = typename StringT::const_iterator
	>
	bool matches(
		StringT const& str,
		StringI pos
	) const noexcept {
		return sequence_matches<StringU>(pos, str.cend());
	}
/// @}
};

/** @} */ // end of name-group Comparison helpers
/** @} */ // end of doc-group text

} // namespace duct

#endif // DUCT_CHARACTERRANGE_HPP_
