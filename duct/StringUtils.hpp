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

#include <type_traits>

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
	static inline bool do_range(typename defsT::string_type& dest, InputIterator const pos, InputIterator const end) {
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
	static inline bool do_range(typename defsT::string_type& dest, InputIterator const pos, InputIterator const end) {
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
bool convert(stringD& dest, stringS const& src, bool append=false) {
	if (!append) {
		dest.clear();
	}
	return cvt_impl<cvt_defs<stringD, typename detail::string_traits<stringS>::encoding_utils> >::do_string(dest, src);
}

/**
	Convert a range from one encoding to another.
	@note If @a stringD's encoding is equivalent to @a fromU, @c [pos..end] is directly copied to @a dest (no re-encoding is performed).
	@note If an incomplete sequence was encountered (@c false is returned), @a dest is guaranteed to contain all valid code points up to the incomplete sequence.
	@returns true on success; or @c false if an incomplete sequence was encountered.
	@tparam fromU @c EncodingUtils specialization for decoding the range.
	@tparam stringD Destination string type; inferred from @a dest.
	@tparam InputIterator Type which satisfies @c InputIterator requirements.
	@param[out] dest Destination string.
	@param pos Beginning input iterator.
	@param end Ending input iterator.
	@param append Whether to append to @a dest; defaults to @c false (@a dest is cleared on entry).
*/
template<class fromU, class stringD, typename InputIterator>
bool convert(stringD& dest, InputIterator const pos, InputIterator const end, bool append=false) {
	if (!append) {
		dest.clear();
	}
	return cvt_impl<cvt_defs<stringD, fromU> >::do_range(dest, pos, end);
}

/**
	Escape flags for @c escape_string(stringT const&, unsigned int).
*/
enum EscapeFlags {
	/**
		Escape some other stuff.
		Includes characters @c '\\t', @c '\\"', @c '\\'', and @c '\\\\'.
	*/
	ESCAPE_OTHER=1<<0,
	/**
		Escape ductScript control characters.
		Includes characters @c '{', @c '}' and @c '='.
	*/
	ESCAPE_CONTROL=1<<1,
	/**
		Escape newlines and linefeeds.
		Includes characters @c '\\n' and @c '\\r'.
	*/
	ESCAPE_LINE=1<<2,
	/**
		All escape flags.
	*/
	ESCAPE_ALL=ESCAPE_OTHER|ESCAPE_CONTROL|ESCAPE_LINE
};

/**
	Get the escape character for a character.
	@returns The escape character, or @c CHAR_NULL if the character is not escapable.
	@tparam charT Character type; inferred from @a c.
	@param c Character to escape.
	@param flags @c EscapeFlags to use; defaults to @c ESCAPE_ALL.
*/
template<typename charT>
charT get_escape_char(charT const c, unsigned int flags=ESCAPE_ALL) {
	if (flags&ESCAPE_OTHER) {
		switch (c) {
		case CHAR_T: return static_cast<charT>(CHAR_TAB);
		case CHAR_QUOTE:
		case CHAR_APOSTROPHE:
		case CHAR_BACKSLASH:
			return c;
		}
	}
	if (flags&ESCAPE_CONTROL) {
		switch (c) {
		case CHAR_OPENBRACE:
		case CHAR_CLOSEBRACE:
		case CHAR_EQUALSIGN:
			return c;
		}
	}
	if (flags&ESCAPE_LINE) {
		switch (c) {
		case CHAR_N: return static_cast<charT>(CHAR_NEWLINE);
		case CHAR_R: return static_cast<charT>(CHAR_CARRIAGERETURN);
		}
	}
	return CHAR_NULL;
}

/**
	Escape characters in a string.
	@note If an existing escape sequence in @a str is invalid, it will be interpreted as escapable, replacing the single backslash with an escaped backslash.
	@returns The escaped string.
	@tparam stringT the string type; inferred from @a str.
	@param str String to escape.
	@param flags Escape flags to use; see @c EscapeFlags.
	@param consider_encapsulation When @c true and when string is encapsulated by quotes, will ignore ESCAPE_CONTROL and ESCAPE_LINE; defaults to @c false.
*/
template<class stringT, typename charT=typename detail::string_traits<stringT>::char_type>
stringT escape_string(stringT const& str, unsigned int const flags, bool const consider_encapsulation=false) {
	if (str.empty()
		|| ((flags&ESCAPE_OTHER)==0
		&& ( flags&ESCAPE_CONTROL)==0
		&& ( flags&ESCAPE_LINE)==0)) {
		return str;
	}
	std::size_t const length=str.size();
	bool isquoted=consider_encapsulation && (2<=length && CHAR_QUOTE==str[0] && CHAR_QUOTE==str[length-1] && !str.compare(length-2, 2, "\\\""));
	stringT result;
	result.reserve(length);
	for (unsigned int i=0; length>i; ++i) {
		charT c=str[i];
		if ((flags&ESCAPE_OTHER)!=0) {
			switch (c) {
			case CHAR_TAB:
				if (!isquoted) {
					result+=CHAR_BACKSLASH;
					result+=CHAR_T;
				} else {
					result+=c;
				}
				continue;
			case CHAR_QUOTE:
				if (0<i && (length-1)>i) {
					result+=CHAR_BACKSLASH;
					result+=CHAR_QUOTE;
				} else {
					result+=CHAR_QUOTE;
				}
				continue;
			case CHAR_BACKSLASH:
				if (length!=(i+1)) {
					charT c2=get_escape_char(str[i+1], flags);
					switch (c2) {
					case CHAR_BACKSLASH: // We don't want to see the slash again; the continue below makes the i-uppage 2
						i++;
						result+=CHAR_BACKSLASH;
						result+=CHAR_BACKSLASH;
						continue;
					case CHAR_NULL: // Invalid escape sequence; TODO: Wait, what? Test this.
						result+=CHAR_BACKSLASH;
						result+=CHAR_BACKSLASH;
						continue;
					default:
						result+=CHAR_BACKSLASH;
						result+=c2;
						i++; // Already a valid escape sequence
						continue;
					}
				} else {
					result+=CHAR_BACKSLASH;
					result+=CHAR_BACKSLASH;
					continue;
				}
				break;
			}
		}
		if ((flags&ESCAPE_CONTROL)!=0 && !isquoted) {
			switch (c) {
			case CHAR_OPENBRACE:
				result+=CHAR_BACKSLASH;
				result+=CHAR_OPENBRACE;
				continue;
			case CHAR_CLOSEBRACE:
				result+=CHAR_BACKSLASH;
				result+=CHAR_CLOSEBRACE;
				continue;
			case CHAR_EQUALSIGN:
				result+=CHAR_BACKSLASH;
				result+=CHAR_EQUALSIGN;
				continue;
			}
		}
		if ((flags&ESCAPE_LINE)!=0 && !isquoted) {
			switch (c) {
			case CHAR_NEWLINE:
				result+=CHAR_BACKSLASH;
				result+=CHAR_N;
				continue;
			case CHAR_CARRIAGERETURN:
				result+=CHAR_BACKSLASH;
				result+=CHAR_R;
				continue;
			}
		}
		result+=c;
	}
	return result;
}

/** @} */ // end of name-group General utilities
/** @} */ // end of doc-group string

} // namespace StringUtils
} // namespace duct

#endif // DUCT_STRINGUTILS_HPP_
