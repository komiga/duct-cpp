/**
@file StringUtils.hpp
@brief String utilities.

@author Tim Howard
@copyright 2010-2012 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
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
	@addtogroup string
	@{
*/
/**
	@name General utilities
	@{
*/

/** @cond INTERNAL */
namespace {
template<class stringD, class fromU>
struct cvt_defs {
	typedef stringD string_type;
	typedef typename detail::string_traits<string_type>::encoding_utils to_utils;
	typedef fromU from_utils;
	typedef typename to_utils::char_type to_char;
	static constexpr bool equivalent=std::is_same<typename from_utils::char_type, to_char>::value;
	enum {BUFFER_SIZE=256u};
};

template<class defsT, typename InputIterator>
bool do_cvt(typename defsT::string_type& dest, InputIterator pos, InputIterator const end) {
	typename defsT::to_char out_buffer[defsT::BUFFER_SIZE], *out_iter=out_buffer;
	InputIterator next;
	char32 cp;
	for (; end!=pos; pos=next) {
		next=defsT::from_utils::decode(pos, end, cp, CHAR_NULL);
		if (next==pos) { // Incomplete sequence
			if (out_buffer!=out_iter) { // Flush if there's any data left in the buffer
				dest.append(out_buffer, out_iter);
			}
			return false;
		}
		out_iter=defsT::to_utils::encode(cp, out_iter, CHAR_NULL);
		if (defsT::BUFFER_SIZE<=6+(out_iter-out_buffer)) { // Prevent output overrun
			dest.append(out_buffer, out_iter);
			out_iter=out_buffer;
		}
	}
	if (out_buffer!=out_iter) { // Flush if there's any data left in the buffer
		dest.append(out_buffer, out_iter);
	}
	return true;
}

template<class defsT, bool eq_=defsT::equivalent>
struct cvt_impl;

template<class defsT>
struct cvt_impl<defsT, false> {
	template<typename InputIterator>
	static inline bool do_sequence(typename defsT::string_type& dest, InputIterator pos, InputIterator const end) {
		return do_cvt<defsT>(dest, pos, end);
	}
	template<class stringS>
	static inline bool do_string(typename defsT::string_type& dest, stringS const& src) {
		return do_cvt<defsT>(dest, src.cbegin(), src.cend());
	}
};

template<class defsT>
struct cvt_impl<defsT, true> {
	template<typename InputIterator>
	static inline bool do_sequence(typename defsT::string_type& dest, InputIterator pos, InputIterator const end) {
		dest.append(pos, end);
		return true;
	}
	template<class stringS>
	static inline bool do_string(typename defsT::string_type& dest, stringS const& src) {
		dest.append(src);
		return true;
	}
};
} // anonymous namespace
/** @endcond */ // INTERNAL

/**
	Convert a string from one type (and encoding) to another.
	@note If @a stringD and @a stringS have equivalent character sizes, @a src is directly copied to @a dest (no re-encoding is performed).
	@note If an incomplete sequence was encountered in @a src (@c false is returned), @a dest is guaranteed to contain all valid code points up to the incomplete sequence.
	@returns @c true on success; or @c false if an incomplete sequence was encountered.
	@tparam stringD Destination string type; inferred from @a dest.
	@tparam stringS Source string type; inferred from @a src.
	@param[out] dest Destination string.
	@param src Source string.
	@param append Whether to append to @a dest; defaults to @c false (@a dest is cleared on entry).
*/
template<class stringD, class stringS>
bool convert(stringD& dest, stringS const& src, bool const append=false) {
	if (!append) {
		dest.clear();
	}
	return cvt_impl<cvt_defs<stringD, typename detail::string_traits<stringS>::encoding_utils> >::do_string(dest, src);
}
/**
	Convert a sequence from one encoding to another.
	@note If @a stringD's encoding is equivalent to @a fromU, @c [pos..end] is directly copied to @a dest (no re-encoding is performed).
	@note If an incomplete sequence was encountered (@c false is returned), @a dest is guaranteed to contain all valid code points up to the incomplete sequence.
	@returns true on success; or @c false if an incomplete sequence was encountered.
	@tparam fromU @c EncodingUtils specialization for decoding the sequence.
	@tparam stringD Destination string type; inferred from @a dest.
	@tparam InputIterator Type which satisfies @c InputIterator requirements; inferred from @a pos.
	@param[out] dest Destination string.
	@param pos Start of sequence.
	@param end End of sequence.
	@param append Whether to append to @a dest; defaults to @c false (@a dest is cleared on entry).
*/
template<class fromU, class stringD, typename InputIterator>
bool convert(stringD& dest, InputIterator pos, InputIterator const end, bool const append=false) {
	if (!append) {
		dest.clear();
	}
	return cvt_impl<cvt_defs<stringD, fromU> >::do_sequence(dest, pos, end);
}

/**
	Count the number of times a code unit occurs in a sequence.
	@note This function does not decode the string into code points; it operates with <strong>code units</strong>.
	@returns The number of times @a cu occurs in the sequence.
	@tparam charT Character type; inferred from @a cu.
	@tparam InputIterator Input iterator type; inferred from @a pos.
	@param cu Code unit to count.
	@param pos Start of sequence.
	@param end End of sequence.
*/
template<typename charT, typename InputIterator>
unsigned int unit_occurrences(charT const cu, InputIterator pos, InputIterator const end) {
	unsigned int count=0;
	for (; end!=pos; ++pos) {
		if (cu==*pos) {
			++count;
		}
	}
	return count;
}
/**
	Count the number of times a code unit occurs in a string.
	@note This function does not decode the string into code points; it operates with <strong>code units</strong>.
	@returns The number of times @a cu occurs in @a str.
	@tparam stringT String type; inferred from @a str.
	@param cu Code unit to count.
	@param str String to test.
*/
template<class stringT>
inline unsigned int unit_occurrences(typename stringT::value_type const cu, stringT const& str) {
	return unit_occurrences(cu, str.cbegin(), str.cend());
}

/** @} */ // end of name-group General utilities

/**
	@name Escape utilities
	@note Observe that all escape utilities operate only with ASCII and the backslash.
	@{
*/

/**
	Pair of escapable chars (member @c first) and their replacements (member @c second).
	@warning Both strings must be the same size; behavior is undefined otherwise.
*/
typedef std::pair<char const*, char const*> EscapeablePair;

/**
	Get the escapeable character for its replacement (e.g., 't' -> literal tabulation).
	@returns The escapeable character for @a cu, or @c CHAR_NULL if @a cu was non-matching.
	@tparam charT Character type; inferred from @a cu.
	@param cu Code unit to test.
	@param esc_pair Escapeables and replacements.
*/
template<typename charT>
charT get_escape_char(charT const cu, EscapeablePair const& esc_pair) {
	auto const pos=std::strchr(esc_pair.second, static_cast<int>(cu));
	if (nullptr!=pos) {
		return static_cast<charT>(esc_pair.first[pos-esc_pair.second]);
	} else {
		return static_cast<charT>(CHAR_NULL);
	}
}

/**
	Escape code units in a string.
	@returns The number of units escaped.
	@tparam stringT Input and result string type; inferred from @a result.
	@param[out] result Result string.
	@param str String to escape.
	@param esc_pair Escapeables and replacements.
	@param ignore_invalids Whether to ignore existing non-matching escape sequences. If @c false, will escape the backslash for non-matching sequences.
	@param clear Whether to clear @a result before escaping; @c false by default.
*/
template<class stringT, class stringU=typename detail::string_traits<stringT>::encoding_utils, typename charT=typename detail::string_traits<stringT>::char_type>
unsigned int escape_string(stringT& result, stringT const& str, EscapeablePair const& esc_pair, bool const ignore_invalids=false, bool const clear=false) {
	unsigned int escaped_count=0;
	typename stringT::const_iterator next, last=str.cbegin();
	charT cu;
	char const* es_pos;
	if (clear) {
		result.clear();
	}
	result.reserve(str.size());
	for (auto it=str.cbegin(); str.cend()!=it; it=next) {
		cu=(*it);
		next=it+1;
		if (CHAR_BACKSLASH==cu) {
			// Escape if backslash is trailing or if escaped unit is non-matching (only when ignoring invalids)
			if (str.cend()==next || (!ignore_invalids && nullptr==std::strchr(esc_pair.second, static_cast<int>(*next)))) {
				result.append(last, next); // Append section from last position to backslash (inclusive)
				result.append(1, CHAR_BACKSLASH); // Prior backslash will form an escaped backslash
				last=next;
				++escaped_count;
			}
			next=stringU::next(next, str.cend()); // Can skip entire unit sequence
			if ((it+1)==next) { // Invalid or incomplete sequence
				break;
			}
		} else if ((es_pos=std::strchr(esc_pair.first, static_cast<int>(cu)), nullptr!=es_pos)) {
			result.append(last, it); // Append last position to escapable unit (exclusive)
			result.append(1, CHAR_BACKSLASH);
			result.append(1, static_cast<charT>(esc_pair.second[es_pos-esc_pair.first])); // Append escaped form of unit
			last=next; // Skip over the unit replaced
			++escaped_count;
		} else { // Non-escapeable
			next=stringU::next(it, str.cend()); // Can skip entire unit sequence
			if (next==it) { // Invalid or incomplete sequence
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
	@tparam stringT Input and result string type; inferred from @a str.
	@param str String to escape.
	@param esc_pair Escapeables and replacements.
	@param ignore_invalids Whether to ignore existing non-matching escape sequences. If @c false, will escape the backslash for non-matching sequences.
*/
template<class stringT>
inline stringT escape_string(stringT const& str, EscapeablePair const& esc_pair, bool const ignore_invalids=false) {
	stringT escaped;
	escape_string(escaped, str, esc_pair, ignore_invalids, false);
	return escaped;
}

/** @} */ // end of name-group Escape utilities
/** @} */ // end of doc-group string

} // namespace StringUtils
} // namespace duct

#endif // DUCT_STRINGUTILS_HPP_
