/**
@file CharacterRange.hpp
@brief CharacterRange.

@author Tim Howard
@copyright 2010-2012 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for license text.
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
	@note The range defined is end-inclusive; for a code point @em cp and CharacterRange @em range, will match:
	@code cp>=range.start() && cp<=range.end() @endcode
*/
class CharacterRange {
private:
	char32 m_start;
	char32 m_end;

public:
/** @name Constructors */ /// @{
	/**
		Construct with single code point.
		@note Equivalent to @c CharacterRange(cp, 0).
		@param cp Code point.
	*/
	explicit CharacterRange(char32 const cp)
		: m_start(cp)
		, m_end(cp)
	{}
	/**
		Construct with range.
		@note A @a length of 0 will still match @a start.
		@param start First code point of the range.
		@param length Length of the range.
	*/
	CharacterRange(char32 const start, unsigned int const length)
		: m_start(start)
		, m_end(start+length)
	{}
	/**
		Copy constructor.
		@param other Range to copy.
	*/
	CharacterRange(CharacterRange const& other)
		: m_start(other.m_start)
		, m_end(other.m_end)
	{}
/// @}

/** @name Properties */ /// @{
	/**
		Set the start of the range.
		@param start New starting code point.
	*/
	inline void set_start(char32 const start) { m_start=start; }
	/**
		Get the start of the range.
		@returns First code point in range.
	*/
	inline char32 start() const { return m_start; }
	/**
		Set the end of the range.
		@param end New ending code point.
	*/
	inline void set_end(char32 const end) { m_end=end; }
	/**
		Get the end of the range.
		@returns Last code point in range.
	*/
	inline char32 end() const { return m_end; }
/// @}

/** @name Comparison */ /// @{
	/**
		Check if the range contains a code point.
		@returns @c true if @a cp was in the range, @c false if it was not.
		@param cp Code point to test.
	*/
	inline bool contains(char32 const cp) const {
		return cp>=m_start && cp<=m_end;
	}
	/**
		Compare against another range.
		@returns @c -1 if the @a other is greater than this, @c 1 if @c this is greater than @a other, or @c 0 if @c this and @a other are equivalent.
		@param other Range to compare against.
	*/
	int compare(CharacterRange const& other) const {
		int const sd=m_end-m_start;
		int const od=other.m_end-other.m_start;
		if (sd<od) {
			return -1;
		} else if (sd>od) {
			return 1;
		}
		if (m_start<other.m_start) {
			return -1;
		} else if (m_start>other.m_start) {
			return 1;
		}
		return 0;
	}
	/**
		Check if a range intersects with this one.
		@returns @c true if the ranges intersect, @c false if they do not.
		@param other Range to compare against.
	*/
	bool intersects(CharacterRange const& other) const {
		if (0==compare(other)) {
			return true;
		}
		if (m_end==(other.m_start-1)) {
			return true;
		} else if ((m_start-1)==other.m_end) {
			return true;
		}
		return !(m_start>other.m_end || m_end<other.m_start);
	}

	/**
		Find the first matching code point in a sequence.
		@returns Iterator of the first matching code point in the sequence, or @a end if the sequence had no matching code points.
		@tparam stringU Encoding utilities.
		@tparam InputIterator Input iterator type; inferred from @a pos.
		@param pos Start of sequence. Behavior is undefined if this does not point to the lead unit for a code unit sequence.
		@param end End of sequence.
	*/
	template<class stringU, typename InputIterator>
	InputIterator sequence_find(InputIterator pos, InputIterator const end) const {
		char32 cp;
		InputIterator next;
		for (; end!=pos; pos=next) {
			next=stringU::decode(pos, end, cp, CHAR_SENTINEL);
			if (next==pos) { // Incomplete sequence
				return end;
			} else if (CHAR_SENTINEL!=cp && contains(cp)) {
				return pos;
			}
		}
		return end;
	}
	/**
		Find the first matching code point in a string.
		@returns Iterator of the first matching code point in @a str, or @c str.cend() if the @a pos to @c str.cend() sequence had no matching code points.
		@tparam stringT String type; inferred from @a str.
		@param str String to search.
		@param pos Start iterator. Behavior is undefined if this does not point to the lead unit for a code unit sequence.
	*/
	template<class stringT, class stringU=typename detail::string_traits<stringT>::encoding_utils, class string_iterator=typename stringT::const_iterator>
	inline string_iterator find(stringT const& str, string_iterator pos) const {
		return sequence_find(pos, str.cend());
	}

	/**
		Check if all code points in a sequence match the range.
		@returns @c true if all code points in the sequence match at least one code point from the range.
		@tparam stringU Encoding utilities.
		@tparam InputIterator Input iterator type; inferred from @a pos.
		@param pos Start of sequence. Behavior is undefined if this does not point to the lead unit for a code unit sequence.
		@param end End of sequence.
	*/
	template<class stringU, typename InputIterator>
	bool sequence_matches(InputIterator pos, InputIterator const end) const {
		char32 cp;
		InputIterator next;
		for (; end!=pos; pos=next) {
			next=stringU::decode(pos, end, cp, CHAR_SENTINEL);
			if (next==pos || CHAR_SENTINEL==cp || !contains(cp)) { // Incomplete sequence || bad sequence || not a match
				return false;
			}
		}
		return true;
	}
	/**
		Check if all code points in a string match the range.
		@returns @c true if all code points in the @a pos to @c str.cend() sequence match at least one code point from the range.
		@tparam stringT String type; inferred from @a str.
		@param str String to search.
		@param pos Start iterator. Behavior is undefined if this does not point to the lead unit for a code unit sequence.
	*/
	template<class stringT, class stringU=typename detail::string_traits<stringT>::encoding_utils, class string_iterator=typename stringT::const_iterator>
	inline bool matches(stringT const& str, string_iterator pos) const {
		return sequence_matches<stringU>(pos, str.cend());
	}
/// @}
};

/** @} */ // end of name-group Comparison helpers
/** @} */ // end of doc-group text

} // namespace duct

#endif // DUCT_CHARACTERRANGE_HPP_
