/**
@file CharacterSet.hpp
@brief CharacterSet.

@author Tim Howard
@copyright 2010-2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_CHARACTERSET_HPP_
#define DUCT_CHARACTERSET_HPP_

#include "./config.hpp"
#include "./debug.hpp"
#include "./aux.hpp"
#include "./char.hpp"
#include "./string.hpp"
#include "./detail/string_traits.hpp"
#include "./CharacterRange.hpp"

namespace duct {

// Forward declarations
class CharacterSet;

/**
	@addtogroup text
	@{
*/
/*
	@name Comparison helpers
	@{
*/

/**
	A set of CharacterRanges.
*/
class CharacterSet /*final*/ {
public:
/** @name Types */ /// @{
	/** CharacterRange vector. */
	typedef duct::aux::vector<CharacterRange> vector_type;
	/** CharacterRange iterator. */
	typedef vector_type::iterator iterator;
	/** @copydoc iterator */
	typedef vector_type::const_iterator const_iterator;
/// @}

private:
	vector_type m_ranges{};

public:
/** @name Constructors */ /// @{
	/** Construct empty set. */
	CharacterSet()=default;
	/**
		Construct with string ranges.
		@param str String of ranges.
	*/
	explicit CharacterSet(u8string const& str)
	{
		add_from_string(str);
	}
	/** @copydoc CharacterSet(u8string const&) */
	explicit CharacterSet(char const* str)
	{
		add_from_string(u8string{str});
	}
	/**
		Construct with single range.
		@param start First code point of range.
		@param length Number of code points in the range.
	*/
	CharacterSet(char32 const start, unsigned const length)
	{
		add_range(start, length);
	}
	/**
		Construct with single code point.
		@param cp Code point.
	*/
	explicit CharacterSet(char32 const cp)
	{
		add_range(cp, 0);
	}
	/** Copy constructor. */
	CharacterSet(CharacterSet const& other)=default;
	/** Move constructor. */
	CharacterSet(CharacterSet&& other)=default;
	/** Destructor. */
	~CharacterSet()=default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator. */
	CharacterSet& operator=(CharacterSet const&)=default;
	/** Move assignment operator. */
	CharacterSet& operator=(CharacterSet&&)=default;
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
		@returns
		- @c true if @a cp is contained, or
		- @c false if it was not.
		@param cp Code point to test.
	*/
	bool contains(char32 const cp) const {
		for (auto const& r : m_ranges) {
			if (r.contains(cp)) {
				return true;
			}
		}
		return false;
	}
	/**
		Check if the set contains a range.
		@returns
		- @c true if the range was found, or
		- @c false if it was not.
		@param range Range to test.
	*/
	bool contains(CharacterRange const& range) const {
		for (auto const& r : m_ranges) {
			if (0==range.compare(r)) {
				return true;
			}
		}
		return false;
	}

	/** @copydoc CharacterRange::sequence_find(InputIt,InputIt const) const */
	template<
		class StringU,
		typename InputIt
	>
	InputIt sequence_find(InputIt pos, InputIt const end) const {
		for (auto const& r : m_ranges) {
			InputIt sit=r.sequence_find<StringU>(pos, end);
			if (end!=sit) {
				return sit;
			}
		}
		return end;
	}
	/** @copydoc CharacterRange::find(StringT const&,StringI) const */
	template<
		class StringT,
		class StringU=typename detail::string_traits<StringT>::encoding_utils,
		class StringI=typename StringT::const_iterator
	>
	StringI find(StringT const& str, StringI pos) const {
		return sequence_find<StringU>(pos, str.cend());
	}

	/** @copydoc CharacterRange::sequence_matches(InputIt,InputIt const) const */
	template<
		class StringU,
		typename InputIt
	>
	bool sequence_matches(InputIt pos, InputIt const end) const {
		char32 cp;
		InputIt next;
		for (; end!=pos; pos=next) {
			next=StringU::decode(pos, end, cp, CHAR_SENTINEL);
			if (next==pos || CHAR_SENTINEL==cp) { // Incomplete or bad sequence
				return false;
			} else {
				for (auto const& r : m_ranges) {
					if (r.contains(cp)) {
						continue;
					}
				}
				// No ranges matched the code point
				return false;
			}
		}
		// Every code point in the sequence matched
		return true;
	}
	/** @copydoc CharacterRange::matches(StringT const&,StringI) const */
	template<
		class StringT,
		class StringU=typename detail::string_traits<StringT>::encoding_utils,
		class StringI=typename StringT::const_iterator
	>
	bool matches(StringT const& str, StringI pos) const {
		return sequence_matches<StringU>(pos, str.cend());
	}
/// @}

/** @name Modification */ /// @{
	/**
		Remove all ranges.
	*/
	void clear() { m_ranges.clear(); }

	/**
		Add string ranges.
		@returns @c *this.
		@tparam StringT String type; inferred from @a str.
		@param str String ranges to add.
	*/
	template<
		class StringT,
		class StringU=typename detail::string_traits<StringT>::encoding_utils,
		class StringI=typename StringT::const_iterator
	>
	CharacterSet& add_from_string(StringT const& str) {
		char32 lastcp=CHAR_SENTINEL;
		char32 cp=CHAR_SENTINEL;
		bool isrange=false;
		bool escape=false;
		StringI it, next;
		for (it=str.cbegin(); str.cend()!=it; it=next) {
			next=StringU::decode(it, str.cend(), cp, CHAR_SENTINEL);
			if (next==it) { // Incomplete sequence
				DUCT_DEBUG("CharacterSet::add_from_string: ics");
				break;
			} else if (CHAR_SENTINEL==cp) { // Invalid code point
				DUCT_DEBUGF(
					"CharacterSet::add_from_string"
					": Invalid code point in string at %lu",
					(unsigned long)(str.cend()-it)
				);
				escape=isrange=false;
				lastcp=CHAR_SENTINEL;
				continue;
			}
			if (escape) {
				escape=false;
			} else if (CHAR_BACKSLASH==cp) {
				escape=true;
				continue;
			} else if (CHAR_SENTINEL!=lastcp && CHAR_DASH==cp && !isrange) {
				isrange=true;
				continue;
			}
			if (CHAR_SENTINEL!=lastcp) {
				if (isrange) {
					if (cp==lastcp) {
						add_range(cp, 0);
					} else if (cp<lastcp) {
						add_range(cp, lastcp-cp);
					} else {
						add_range(lastcp, cp-lastcp);
					}
					lastcp=CHAR_SENTINEL;
					isrange=false;
				} else {
					add_range(lastcp, 0);
					lastcp=cp;
				}
			} else {
				lastcp=cp;
			}
		}
		if (CHAR_SENTINEL!=lastcp) {
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
		@sa CharacterRange::CharacterRange(char32 const, unsigned const)
	*/
	CharacterSet& add_range(char32 const start, unsigned const length=0) {
		bool empty=m_ranges.empty();
		CharacterRange const new_range{start, length};
		// Avoid adding duplicate ranges
		if (empty || !contains(new_range)) {
			if (!empty) {
				for (auto& r : m_ranges) {
					if (new_range.intersects(r)) {
						if (new_range.first()<r.first()) {
							r.set_first(new_range.first());
						}
						if (new_range.last()>r.last()) {
							r.set_last(new_range.last());
						}
						return *this;
					}
				}
			}
			m_ranges.emplace_back(new_range);
		}
		return *this;
	}

	/**
		Add all whitespace characters: tab, linefeed, carriage
		return, and space.
		@returns @c *this.
	*/
	CharacterSet& add_whitespace() {
		add_range('\t', 1); // \t and \n
		add_range('\r', 0);
		add_range(' ', 0);
		return *this;
	}
	/**
		Add all alphanumberic characters: @c A-Z, @c a-z, and @c 0-9.
		@returns @c *this.
	*/
	CharacterSet& add_alphanumeric() {
		add_range('A', 25);
		add_range('a', 25);
		add_range('0', 9);
		return *this;
	}
	/**
		Add @c A-Z and @c a-z.
		@returns @c *this.
	*/
	CharacterSet& add_letters() {
		add_range('A', 25);
		add_range('a', 25);
		return *this;
	}
	/**
		Add @c A-Z.
		@returns @c *this.
	*/
	CharacterSet& add_uppercase_letters() {
		add_range('A', 25);
		return *this;
	}
	/**
		Add @c a-z.
		@returns @c *this.
	*/
	CharacterSet& add_lowercase_letters() {
		add_range('a', 25);
		return *this;
	}
	/**
		Add @c 0-9.
		@returns @c *this.
	*/
	CharacterSet& add_numbers() {
		add_range('0', 9);
		return *this;
	}
/// @}
};

/* @} */ // end of name-group Comparison helpers
/** @} */ // end of doc-group text

} // namespace duct

#endif // DUCT_CHARACTERSET_HPP_
