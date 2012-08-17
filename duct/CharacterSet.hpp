/**
@file CharacterSet.hpp
@brief CharacterSet and CharacterRange.

@author Tim Howard
@copyright 2010-2012 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for license text.
*/

#ifndef DUCT_CHARACTERSET_HPP_
#define DUCT_CHARACTERSET_HPP_

#include "./config.hpp"
#include "./debug.hpp"
#include "./stl.hpp"
#include "./char.hpp"
#include "./string.hpp"
#include "./detail/string_traits.hpp"

#include <algorithm>

namespace duct {

// Forward declarations
class CharacterRange;
class CharacterSet;

/**
	@addtogroup string
	@{
*/
/**
	@name Comparison helpers
	@{
*/

/**
	A range of characters.
	@note The range defined is end-inclusive; for a code point @em cp and CharacterRange @em range, will match: @code cp>=range.start() && cp<=range.end() @endcode
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
	void set_start(char32 const start) { m_start=start; }
	/**
		Get the start of the range.
		@returns First code point in range.
	*/
	char32 start() const { return m_start; }
	/**
		Set the end of the range.
		@param end New ending code point.
	*/
	void set_end(char32 const end) { m_end=end; }
	/**
		Get the end of the range.
		@returns Last code point in range.
	*/
	char32 end() const { return m_end; }
/// @}

/** @name Comparison */ /// @{
	/**
		Check if the range contains a code point.
		@returns @c true if @a cp was in the range, @c false if it was not.
		@param cp Code point to test.
	*/
	bool contains(char32 const cp) const {
		return cp>=m_start && cp<=m_end;
	}
	/**
		Compare against another CharacterRange.
		@returns @c -1 if the @a other is greater than this, @c 1 if @c this is greater than @a other, or 0 if @c this and @a other are equivalent.
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
		Find the first matching code point in a string.
		@returns Iterator of the first matching code point in @a str, or the end iterator if the @a from to @c str.cend() range had no matching code points.
		@tparam stringT String type; inferred from @a str.
		@param str String to search.
		@param from Start iterator. Behavior is undefined if @a from is not pointing to the lead unit for a sequence.
	*/
	template<class stringT, class stringU=typename detail::string_traits<stringT>::encoding_utils, class string_iterator=typename stringT::const_iterator>
	string_iterator find(stringT const& str, string_iterator from) const {
		char32 cp;
		string_iterator it, next;
		for (it=from; str.cend()!=it; it=next) {
			next=stringU::decode(it, str.cend(), cp, CHAR_NULL);
			if (next==it) { // Incomplete sequence
				return str.cend();
			} else if (CHAR_NULL!=cp && contains(cp)) {
				return it;
			}
		}
		return str.cend();
	}
/// @}
};

/**
	A set of CharacterRanges.
*/
class CharacterSet {
public:
	/** CharacterRange vector. */
	typedef duct::stl::vector<CharacterRange>::type vector_type;
	/** CharacterRange iterator. */
	typedef vector_type::iterator iterator;
	/** @copydoc iterator */
	typedef vector_type::const_iterator const_iterator;

private:
	vector_type m_ranges;

public:
/** @name Constructors */ /// @{
	/**
		Construct empty set.
	*/
	CharacterSet()
		: m_ranges()
	{}
	/**
		Construct with string ranges.
		@param str String of ranges.
	*/
	explicit CharacterSet(u8string const& str)
		: m_ranges()
	{
		add_from_string(str);
	}
	/** @copydoc CharacterSet(u8string const&) */
	explicit CharacterSet(char const* str)
		: m_ranges()
	{
		add_from_string(u8string(str));
	}
	/**
		Construct with single range.
		@param start First code point of range.
		@param length Number of code points in the range.
	*/
	CharacterSet(char32 const start, unsigned int const length)
		: m_ranges()
	{
		add_range(start, length);
	}
	/**
		Construct with single code point.
		@param cp Code point.
	*/
	explicit CharacterSet(char32 const cp)
		: m_ranges()
	{
		add_range(cp, 0);
	}
	/**
		Copy constructor.
		@param other CharacterSet to copy.
	*/
	CharacterSet(CharacterSet const& other)
		: m_ranges(other.m_ranges)
	{}
/// @}

/** @name Properties */ /// @{
	/**
		Get number of ranges.
		@returns The current number of ranges.
	*/
	vector_type::size_type size() const { return m_ranges.size(); }

	/**
		Get beginning range iterator.
		@returns The beginning range iterator.
	*/
	iterator begin() { return m_ranges.begin(); }
	/** @copydoc begin() */
	const_iterator cbegin() const { return m_ranges.cbegin(); }

	/**
		Get ending range iterator.
		@returns The ending range iterator.
	*/
	iterator end() { return m_ranges.end(); }
	/** @copydoc end() */
	const_iterator cend() const { return m_ranges.cend(); }
/// @}

/** @name Comparison */ /// @{
	/**
		Check if the set contains a code point.
		@returns @c true if @a cp was in the set's ranges, or @c false if it was not.
		@param cp Code point to test.
	*/
	bool contains(char32 const cp) const {
		for (auto it=cbegin(); cend()!=it; ++it) {
			if ((*it).contains(cp)) {
				return true;
			}
		}
		return false;
	}
	/**
		Check if the set contains the given range.
		@returns @c true if the range was found, or @c false if it was not.
		@param range Range to test.
	*/
	bool contains(CharacterRange const& range) const {
		for (auto it=cbegin(); cend()!=it; ++it) {
			if (0==range.compare(*it)) {
				return true;
			}
		}
		return false;
	}
	/**
		Find the first matching code point in a string.
		@returns Iterator of the first matching code point in @a str, or the end iterator if the @a from to @c str.cend() range had no matching code points.
		@tparam stringT String type; inferred from @a str.
		@param str String to search.
		@param from Start iterator. Behavior is undefined if @a from is not pointing to the lead unit for a sequence.
	*/
	template<class stringT, class stringU=typename detail::string_traits<stringT>::encoding_utils, class string_iterator=typename stringT::const_iterator>
	string_iterator find(stringT const& str, string_iterator from) const {
		string_iterator sit;
		for (const_iterator rit=cbegin(); cend()!=rit; ++rit) {
			sit=(*rit).find(str, from);
			if (str.cend()!=sit) {
				return sit;
			}
		}
		return str.cend();
	}
/// @}

/** @name Modification */ /// @{
	/**
		Remove all ranges from the set.
	*/
	void clear() { m_ranges.clear(); }

	/**
		Add the given string ranges to the set.
		@returns @c *this.
		@tparam stringT String type; inferred from @a str.
		@param str String ranges to add.
	*/
	template<class stringT, class stringU=typename detail::string_traits<stringT>::encoding_utils, class string_iterator=typename stringT::const_iterator>
	CharacterSet& add_from_string(stringT const& str) {
		char32 lastcp=CHAR_NULL;
		char32 cp;
		bool isrange=false;
		bool escape=false;
		string_iterator it, next;
		for (it=str.cbegin(); str.cend()!=it; it=next) {
			next=stringU::decode(it, str.cend(), cp, CHAR_NULL);
			if (next==it) { // Incomplete sequence
				DUCT_DEBUG("CharacterSet::add_from_string: ics");
				break;
			} else if (CHAR_NULL==cp) { // Invalid code point
				DUCT_DEBUGF("CharacterSet::add_from_string: Invalid code point in string at %lu", (unsigned long)(str.cend()-it));
				escape=isrange=false;
				lastcp=CHAR_NULL;
				continue;
			}
			if (escape) {
				escape=false;
			} else if (CHAR_BACKSLASH==cp) {
				escape=true;
				continue;
			} else if (CHAR_NULL!=lastcp && CHAR_DASH==cp && !isrange) {
				isrange=true;
				continue;
			}
			if (CHAR_NULL!=lastcp) {
				if (isrange) {
					if (cp==lastcp) {
						add_range(cp, 0);
					} else if (cp<lastcp) {
						add_range(cp, lastcp-cp);
					} else {
						add_range(lastcp, cp-lastcp);
					}
					lastcp=CHAR_NULL;
					isrange=false;
				} else {
					add_range(lastcp, 0);
					lastcp=cp;
				}
			} else {
				lastcp=cp;
			}
		}
		if (CHAR_NULL!=lastcp) {
			if (isrange) {
				DUCT_DEBUG("CharacterSet::add_from_string: Invalid range in string");
			}
			add_range(lastcp, 0);
		}
		return *this;
	}

	/**
		Add a length range.
		@returns @c *this.
		@param start Start of the range.
		@param length Length of the range.
		@sa CharacterRange::CharacterRange(char32 const, unsigned int const)
	*/
	CharacterSet& add_range(char32 const start, unsigned int const length=0) {
		bool empty=m_ranges.empty();
		CharacterRange const new_range(start, length);
		if (empty || !contains(new_range)) { // Try to avoid adding the same new_range twice
			if (!empty) {
				for (iterator it=begin(); end()!=it; ++it) {
					CharacterRange& cr=(*it);
					if (new_range.intersects(cr)) {
						if (new_range.start()<cr.start()) {
							cr.set_start(new_range.start());
						}
						if (new_range.end()>cr.end()) {
							cr.set_end(new_range.end());
						}
						return *this;
					}
				}
			}
			m_ranges.push_back(new_range);
		}
		return *this;
	}

	/**
		Add all whitespace characters to the set: tab, linefeed, carriage return, and space.
		@returns @c *this.
	*/
	CharacterSet& add_whitespace() {
		add_range('\t', 1); // \t and \n
		add_range('\r', 0);
		add_range(' ', 0);
		return *this;
	}
	/**
		Add all alphanumberic characters to the set: @c A-Z, @c a-z, and @c 0-9.
		@returns @c *this.
	*/
	CharacterSet& add_alphanumeric() {
		add_range('A', 25);
		add_range('a', 25);
		add_range('0', 9);
		return *this;
	}
	/**
		Add @c A-Z and @c a-z to the set.
		@returns @c *this.
	*/
	CharacterSet& add_letters() {
		add_range('A', 25);
		add_range('a', 25);
		return *this;
	}
	/**
		Add @c A-Z to the set.
		@returns @c *this.
	*/
	CharacterSet& add_uppercase_letters() {
		add_range('A', 25);
		return *this;
	}
	/**
		Add @c a-z to the set.
		@returns @c *this.
	*/
	CharacterSet& add_lowercase_letters() {
		add_range('a', 25);
		return *this;
	}
	/**
		Add @c 0-9 to the set.
		@returns @c *this.
	*/
	CharacterSet& add_numbers() {
		add_range('0', 9);
		return *this;
	}
/// @}
};

/** @} */ // end of name-group Comparison helpers
/** @} */ // end of doc-group string

} // namespace duct

#endif // DUCT_CHARACTERSET_HPP_
