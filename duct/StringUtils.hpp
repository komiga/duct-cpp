/**
@file StringUtils.hpp
@brief String utilities.

@author Tim Howard
@copyright 2010-2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_STRINGUTILS_HPP_
#define DUCT_STRINGUTILS_HPP_

#include "./config.hpp"
#include "./char.hpp"
#include "./string.hpp"
#include "./detail/string_traits.hpp"
#include "./EncodingUtils.hpp"

#include <cstring>
#include <type_traits>
#include <utility>

namespace duct {
namespace StringUtils {

/**
	@addtogroup text
	@{
*/
/**
	@name General utilities
	@{
*/

/** @cond INTERNAL */
namespace {
template<
	class StringD,
	class FromU
>
struct cvt_defs {
	typedef StringD string_type;
	typedef typename detail::string_traits<string_type>::encoding_utils to_utils;
	typedef FromU from_utils;
	typedef typename to_utils::char_type to_char;
	static constexpr bool equivalent=std::is_same<typename from_utils::char_type, to_char>::value;
	enum {BUFFER_SIZE=256u};
};

template<
	class DefsT,
	typename RandomAccessIt
>
bool do_cvt(typename DefsT::string_type& dest, RandomAccessIt pos, RandomAccessIt const end) {
	typename DefsT::to_char
		out_buffer[DefsT::BUFFER_SIZE],
		*out_iter=out_buffer
	;
	RandomAccessIt next;
	char32 cp;
	for (; end!=pos; pos=next) {
		next=DefsT::from_utils::decode(pos, end, cp, CHAR_NULL);
		// Incomplete sequence
		if (next==pos) {
			// Flush if any data is left in the buffer
			if (out_buffer!=out_iter) {
				dest.append(out_buffer, out_iter);
			}
			return false;
		}
		out_iter=DefsT::to_utils::encode(cp, out_iter, CHAR_NULL);
		// Prevent output overrun
		if (DefsT::BUFFER_SIZE<=6+(out_iter-out_buffer)) {
			dest.append(out_buffer, out_iter);
			out_iter=out_buffer;
		}
	}
	// Flush if any data is left in the buffer
	if (out_buffer!=out_iter) {
		dest.append(out_buffer, out_iter);
	}
	return true;
}

template<class DefsT, bool eq_=DefsT::equivalent>
struct cvt_impl;

template<class DefsT>
struct cvt_impl<DefsT, false> {
	template<typename RandomAccessIt>
	static bool do_sequence(typename DefsT::string_type& dest, RandomAccessIt pos, RandomAccessIt const end) {
		return do_cvt<DefsT>(dest, pos, end);
	}
	template<class StringS>
	static bool do_string(typename DefsT::string_type& dest, StringS const& src) {
		return do_cvt<DefsT>(dest, src.cbegin(), src.cend());
	}
};

template<class DefsT>
struct cvt_impl<DefsT, true> {
	template<typename InputIt>
	static bool do_sequence(typename DefsT::string_type& dest, InputIt pos, InputIt const end) {
		dest.append(pos, end);
		return true;
	}
	template<class StringS>
	static bool do_string(typename DefsT::string_type& dest, StringS const& src) {
		dest.append(src);
		return true;
	}
};
} // anonymous namespace
/** @endcond */ // INTERNAL

/**
	Convert a string from one type (and encoding) to another.
	@note If @a StringD and @a StringS have equivalent character sizes, @a src is
	directly copied to @a dest (no re-encoding is performed).
	@note If an incomplete sequence was encountered in @a src (@c false is
	returned), @a dest is guaranteed to contain all valid code points up to the
	incomplete sequence.
	@returns
	- @c true on success; or
	- @c false if an incomplete sequence was encountered.
	@tparam StringD Destination string type; inferred from @a dest.
	@tparam StringS Source string type; inferred from @a src.
	@param[out] dest Destination string.
	@param src Source string.
	@param append Whether to append to @a dest; defaults to @c false (@a dest is cleared
	on entry).
*/
template<
	class StringD,
	class StringS
>
inline bool convert(StringD& dest, StringS const& src, bool const append=false) {
	if (!append) {
		dest.clear();
	}
	using FromU=typename detail::string_traits<StringS>::encoding_utils;
	return cvt_impl<cvt_defs<StringD, FromU> >::do_string(dest, src);
}
/**
	Convert a sequence from one encoding to another.
	@note If @a StringD's encoding is equivalent to @a FromU, @c [pos..end] is directly
	copied to @a dest (no re-encoding is performed).
	@note If an incomplete sequence was encountered (@c false is returned), @a dest is
	guaranteed to contain all valid code points up to the incomplete sequence.
	@returns
	- @c true on success; or
	- @c false if an incomplete sequence was encountered.
	@tparam FromU @c EncodingUtils specialization for decoding the sequence.
	@tparam StringD Destination string type; inferred from @a dest.
	@tparam InputIt Type which satisfies @c InputIt requirements; inferred from @a pos.
	@param[out] dest Destination string.
	@param pos Start of sequence.
	@param end End of sequence.
	@param append Whether to append to @a dest; defaults to @c false (@a dest is cleared
	on entry).
*/
template<
	class FromU,
	class StringD,
	typename InputIt
>
inline bool convert(StringD& dest, InputIt pos, InputIt const end, bool const append=false) {
	if (!append) {
		dest.clear();
	}
	return cvt_impl<cvt_defs<StringD, FromU> >::do_sequence(dest, pos, end);
}

/**
	Count the number of times a code unit occurs in a sequence.
	@note This function does not decode the string into code points; it operates with
	<strong>code units</strong>.
	@returns The number of times @a cu occurs in the sequence.
	@tparam CharT Character type; inferred from @a cu.
	@tparam InputIt Input iterator type; inferred from @a pos.
	@param cu Code unit to count.
	@param pos Start of sequence.
	@param end End of sequence.
*/
template<
	typename CharT,
	typename InputIt
>
inline unsigned unit_occurrences(CharT const cu, InputIt pos, InputIt const end) {
	unsigned count=0;
	for (; end!=pos; ++pos) {
		if (cu==*pos) {
			++count;
		}
	}
	return count;
}
/**
	Count the number of times a code unit occurs in a string.
	@note This function does not decode the string into code points; it operates with
	<strong>code units</strong>.
	@returns The number of times @a cu occurs in @a str.
	@tparam StringT String type; inferred from @a str.
	@param cu Code unit to count.
	@param str String to test.
*/
template<class StringT>
inline unsigned unit_occurrences(typename StringT::value_type const cu, StringT const& str) {
	return unit_occurrences(cu, str.cbegin(), str.cend());
}

/** @} */ // end of name-group General utilities

/**
	@name Escape utilities
	@note Observe that all escape utilities operate only with ASCII and the
	backslash.
	@{
*/

/**
	Pair of escapable chars (member @c first) and their replacements
	(member @c second).
	@warning Both strings must be the same size; behavior is undefined otherwise.
*/
typedef std::pair<char const*, char const*> EscapeablePair;

/**
	Get the escapeable character for its replacement (e.g., 't' ->
	literal tabulation).
	@returns
	- The escapeable character for @a cu; or
	- @c CHAR_NULL if @a cu was non-matching.
	@tparam CharT Character type; inferred from @a cu.
	@param cu Code unit to test.
	@param esc_pair Escapeables and replacements.
*/
template<typename CharT>
inline CharT get_escape_char(CharT const cu, EscapeablePair const& esc_pair) {
	auto const pos=std::strchr(esc_pair.second, static_cast<signed>(cu));
	if (nullptr!=pos) {
		return static_cast<CharT>(esc_pair.first[pos-esc_pair.second]);
	} else {
		return static_cast<CharT>(CHAR_NULL);
	}
}

/**
	Escape code units in a string.
	@returns The number of units escaped.
	@tparam StringT Input and result string type; inferred from @a result.
	@param[out] result Result string.
	@param str String to escape.
	@param esc_pair Escapeables and replacements.
	@param ignore_invalids Whether to ignore existing non-matching 
	escape sequences. If @c false, will escape the backslash for
	non-matching sequences.
	@param clear Whether to clear @a result before escaping; @c false by default.
*/
template<
	class StringT,
	class StringU=typename detail::string_traits<StringT>::encoding_utils,
	typename CharT=typename detail::string_traits<StringT>::char_type
>
unsigned escape_string(StringT& result, StringT const& str, EscapeablePair const& esc_pair, bool const ignore_invalids=false, bool const clear=false) {
	unsigned escaped_count=0;
	typename StringT::const_iterator next, last=str.cbegin();
	CharT cu;
	char const* es_pos;
	if (clear) {
		result.clear();
	}
	result.reserve(str.size());
	for (auto it=str.cbegin(); str.cend()!=it; it=next) {
		cu=(*it);
		next=it+1;
		if (CHAR_BACKSLASH==cu) {
			// Escape if backslash is trailing or if escaped unit is non-matching
			// (only when ignoring invalids)
			if (str.cend()==next
				|| (!ignore_invalids
					&& nullptr==std::strchr(esc_pair.second, static_cast<signed>(*next)))
			) {
				// Append section from last position to backslash (inclusive)
				result.append(last, next);
				// Prior backslash will form an escaped backslash
				result.append(1, CHAR_BACKSLASH);
				last=next;
				++escaped_count;
			}
			// Can skip entire unit sequence
			next=StringU::next(next, str.cend());
			if ((it+1)==next) {
				// Invalid or incomplete sequence
				break;
			}
		} else if ((es_pos=std::strchr(esc_pair.first, static_cast<signed>(cu)), nullptr!=es_pos)) {
			// Append last position to escapable unit (exclusive)
			result.append(last, it);
			result.append(1, CHAR_BACKSLASH);
			// Append escaped form of unit
			result.append(1, static_cast<CharT>(esc_pair.second[es_pos-esc_pair.first]));
			// Skip over the unit replaced
			last=next;
			++escaped_count;
		} else { // Non-escapeable
			// Can skip entire unit sequence
			next=StringU::next(it, str.cend());
			if (next==it) {
				// Invalid or incomplete sequence
				break;
			}
		}
	}
	result.append(last, str.cend());
	return escaped_count;
}
/**
	Escape code units in a string.
	@returns The escaped string.
	@tparam StringT Input and result string type; inferred from @a str.
	@param str String to escape.
	@param esc_pair Escapeables and replacements.
	@param ignore_invalids Whether to ignore existing non-matching
	escape sequences. If @c false, will escape the backslash for
	non-matching sequences.
*/
template<class StringT>
inline StringT escape_string(StringT const& str, EscapeablePair const& esc_pair, bool const ignore_invalids=false) {
	StringT escaped;
	escape_string(escaped, str, esc_pair, ignore_invalids, false);
	return escaped;
}

/** @} */ // end of name-group Escape utilities
/** @} */ // end of doc-group text

} // namespace StringUtils
} // namespace duct

#endif // DUCT_STRINGUTILS_HPP_
