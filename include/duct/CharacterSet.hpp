/**
@file CharacterSet.hpp
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

CharacterSet and CharacterRange classes.
*/

#ifndef DUCT_CHARACTERSET_HPP_
#define DUCT_CHARACTERSET_HPP_

#include <duct/config.hpp>
#include <duct/string.hpp>
#include <duct/char.hpp>
#include <duct/detail/string_traits.hpp>
#include <duct/debug.hpp>

#include <vector>
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
public:
// ctor/dtor
	/**
		Constructor with single code point.
		@note Equivalent to @c CharacterRange(cp, 0).
		@param cp Code point.
	*/
	explicit CharacterRange(char32 const cp)
		: m_start(cp)
		, m_end(cp)
	{}
	/**
		Constructor with range.
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
		@param other Range to copy from.
	*/
	CharacterRange(CharacterRange const& other)
		: m_start(other.m_start)
		, m_end(other.m_end)
	{}

// properties
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

// comparison
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
	
private:
	char32 m_start;
	char32 m_end;
};

/**
	A set of CharacterRanges.
*/
class CharacterSet {
public:
	/**
		CharacterRange @c std::vector.
	*/
	typedef std::vector<CharacterRange> vector_type;

public:
// ctor/dtor
	/**
		Constructor.
	*/
	CharacterSet()
		: m_ranges()
	{}
	/** @{ */
	/**
		Constructor with string ranges.
		@param str String of ranges.
	*/
	explicit CharacterSet(u8string const& str)
		: m_ranges()
	{
		add_from_string(str);
	}
	explicit CharacterSet(char const* str)
		: m_ranges()
	{
		add_from_string(u8string(str));
	}
	/** @} */
	/**
		Constructor with single range.
		@param start First code point of range.
		@param length Number of code points in the range.
	*/
	CharacterSet(char32 const start, unsigned int const length)
		: m_ranges()
	{
		add_range(start, length);
	}
	/**
		Constructor with single code point.
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

// properties
	/** @{ */
	/**
		Begin range iterator.
		@returns Beginning iterator for the set's ranges.
	*/
	vector_type::iterator begin() { return m_ranges.begin(); }
	vector_type::const_iterator cbegin() const { return m_ranges.cbegin(); }
	/** @} */

	/** @{ */
	/**
		End range iterator.
		@returns Ending iterator for the set's ranges.
	*/
	vector_type::iterator end() { return m_ranges.end(); }
	vector_type::const_iterator cend() const { return m_ranges.cend(); }
	/** @} */

// comparison
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
		for (vector_type::const_iterator rit=cbegin(); cend()!=rit; ++rit) {
			sit=(*rit).find(str, from);
			if (str.cend()!=sit) {
				return sit;
			}
		}
		return str.cend();
	}

// addition
	/**
		Remove all ranges from the set.
	*/
	void clear() {
		m_ranges.clear();
	}

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
		@see CharacterRange::CharacterRange(char32 const, unsigned int const)
	*/
	CharacterSet& add_range(char32 const start, unsigned int const length=0) {
		bool empty=m_ranges.empty();
		CharacterRange const new_range(start, length);
		if (empty || !contains(new_range)) { // Try to avoid adding the same new_range twice
			if (!empty) {
				for (vector_type::iterator it=begin(); end()!=it; ++it) {
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
	
private:
	vector_type m_ranges;
};

/** @} */ // end of name-group Comparison helpers
/** @} */ // end of doc-group string

} // namespace duct

#endif // DUCT_CHARACTERSET_HPP_
