/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
*/

/*
	The following is a heavily modified version of the Unicode utils in the
	SFML working tree (as of 2011-06-17).

	See https://github.com/LaurentGomila/SFML/tree/67455e1a16f797a6e724c701cd9e6a2481384442
*/

/*
	SFML - Simple and Fast Multimedia Library
	Copyright (C) 2007-2009 Laurent Gomila (laurent.gom@gmail.com)

	This software is provided 'as-is', without any express or implied warranty.
	In no event will the authors be held liable for any damages arising from
	the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented;
	you must not claim that you wrote the original software.
	If you use this software in a product, an acknowledgment
	in the product documentation would be appreciated but is not required.

	2. Altered source versions must be plainly marked as such,
	and must not be misrepresented as being the original software.

	3. This notice may not be removed or altered from any source distribution.
*/

/*
	References:
	http://www.unicode.org
	http://www.unicode.org/Public/PROGRAMS/CVTUTF/ConvertUTF.c
	http://www.unicode.org/Public/PROGRAMS/CVTUTF/ConvertUTF.h
	http://people.w3.org/rishida/scripts/uniview/conversion
*/

// specialization UTF8Utils implementation

/*
	from_latin1 and to_latin1:
	Latin-1 is directly compatible with Unicode encodings, and can thus be
	treated as (a subset of) UTF-32.

	UTF32Utils' decode_wide and encode_wide:
	The encoding of wide characters is undefined. We assume that it is UCS-2
	on Windows and UCS-4 on Unix systems.
*/

namespace {
static std::uint8_t const s_utf8_trailing[256]{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};
} // anonymous namespace

template<class RandomAccessIt>
RandomAccessIt UTF8Utils::decode(
	RandomAccessIt pos,
	RandomAccessIt const end,
	char32& output,
	char32 const replacement
) {
	static std::uint32_t const offsets[6]{
		0x00000000, 0x00003080, 0x000E2080,
		0x03C82080, 0xFA082080, 0x82082080
	};
	unsigned const trailing_units = s_utf8_trailing[
		static_cast<char8_strict>(*pos)
	];
	if (end > pos + trailing_units) {
		output = 0;
		switch (trailing_units) {
		case 5: output += static_cast<char8_strict>(*pos++); output <<= 6;
		case 4: output += static_cast<char8_strict>(*pos++); output <<= 6;
		case 3: output += static_cast<char8_strict>(*pos++); output <<= 6;
		case 2: output += static_cast<char8_strict>(*pos++); output <<= 6;
		case 1: output += static_cast<char8_strict>(*pos++); output <<= 6;
		case 0: output += static_cast<char8_strict>(*pos++);
		}
		output -= offsets[trailing_units];
		if (!DUCT_UNI_IS_CP_VALID(output)) {
			output = replacement;
		}
	} // Else sequence is invalid; pos was not modified, so we can just return
	return pos;
}

template<class OutputIt>
OutputIt UTF8Utils::encode(
	char32 input,
	OutputIt output,
	char32 const replacement
) {
	// UTF-8 first bytes
	static char8_strict const utf8_fbs[7]{
		0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
	};
	if (!DUCT_UNI_IS_CP_VALID(input)) {
		if (DUCT_UNI_IS_CP_VALID(replacement) && CHAR_NULL != replacement) {
			input = replacement;
		} else { // Invalid or null replacement
			return output;
		}
	}
	// Valid code point (input or replaced)
	unsigned to_write = required(input);
	// Extract bytes
	char8_strict bytes[4];
	switch (to_write) {
	case 4: bytes[3] = static_cast<char8_strict>((input | 0x80) & 0xBF); input >>= 6;
	case 3: bytes[2] = static_cast<char8_strict>((input | 0x80) & 0xBF); input >>= 6;
	case 2: bytes[1] = static_cast<char8_strict>((input | 0x80) & 0xBF); input >>= 6;
	case 1: bytes[0] = static_cast<char8_strict> (input | utf8_fbs[to_write]);
	}
	// Add them to the output
	char8_strict const* iter = bytes;
	switch (to_write) {
	case 4: *output++ = *iter++;
	case 3: *output++ = *iter++;
	case 2: *output++ = *iter++;
	case 1: *output++ = *iter++;
	}
	return output;
}

template<class RandomAccessIt>
RandomAccessIt UTF8Utils::next(
	RandomAccessIt const from,
	RandomAccessIt const end
) {
	if (end <= from) {
		// Quick exit: invalid or unsteppable position
		return from;
	}
	using diff_type = typename std::iterator_traits<RandomAccessIt>::difference_type;
	diff_type const units = static_cast<diff_type>(required_first_whole(*from));
	return (std::distance(from, end) < units)
		? from
		: from + units;
}

template<class RandomAccessIt>
RandomAccessIt UTF8Utils::prev(
	RandomAccessIt from,
	RandomAccessIt const begin
) {
	RandomAccessIt pos = from;
	if (begin >= from) {
		// Quick exit: invalid or unsteppable position
		return from;
	}
	// Move out of head unit or trail sequence
	--pos;
	while (begin < pos && DUCT_UTF8_IS_TRAIL(*pos)) {
		--pos;
	}
	if (DUCT_UTF8_IS_TRAIL(*pos)) {
		// Incomplete sequence; begin == pos
		return from;
	} else {
		// Good; true == DUCT_UTF8_IS_HEAD(*pos)
		return pos;
	}
}

inline unsigned UTF8Utils::required_first(
	char_type const first
) {
	return s_utf8_trailing[static_cast<char8_strict>(first)];
}

inline unsigned UTF8Utils::required_first_whole(
	char_type const first
) {
	return 1u + s_utf8_trailing[static_cast<char8_strict>(first)];
}

inline unsigned UTF8Utils::required(
	char32 const c
) {
	// If greater than 0x10FFFF, the character is invalid and would be
	// replaced with a single unit or skipped
	if 		(c <  0x80 || c > 0x10FFFF) return 1u;
	else if (c <  0x800)		return 2u;
	else if (c <  0x10000)		return 3u;
	else   /*c <= 0x10FFFF*/	return 4u;
}

template<class RandomAccessIt>
std::size_t UTF8Utils::count(
	RandomAccessIt begin,
	RandomAccessIt const end,
	bool const count_incomplete
) {
	std::size_t length = 0;
	using diff_type = typename std::iterator_traits<RandomAccessIt>::difference_type;
	diff_type units;
	while (end > begin) {
		units = static_cast<diff_type>(required_first_whole(*begin));
		if (std::distance(begin, end) < units) { // Incomplete sequence
			length += (count_incomplete ? 1 : 0);
			break;
		} else {
			begin += units;
			++length;
		}
	}
	return length;
}

template<
	class RandomAccessIt,
	class OutputIt
>
OutputIt UTF8Utils::from_utf8(
	RandomAccessIt begin,
	RandomAccessIt const end,
	OutputIt output
) {
	while (end > begin) {
		*output++ = *begin++;
	}
	return output;
}

template<
	class RandomAccessIt,
	class OutputIt
>
OutputIt UTF8Utils::from_utf16(
	RandomAccessIt begin,
	RandomAccessIt const end,
	OutputIt output
) {
	char32 codepoint;
	while (end > begin) {
		begin = UTF16Utils::decode(begin, end, codepoint);
		output = encode(codepoint, output);
	}
	return output;
}

template<
	class RandomAccessIt,
	class OutputIt
>
OutputIt UTF8Utils::from_utf32(
	RandomAccessIt begin,
	RandomAccessIt const end,
	OutputIt output
) {
	while (end > begin) {
		output = encode(*begin++, output);
	}
	return output;
}

template<class OutU, class RandomAccessIt, class OutputIt>
inline OutputIt UTF8Utils::to_other(
	RandomAccessIt begin,
	RandomAccessIt const end,
	OutputIt output
) {
	return OutU::from_utf8(begin, end, output);
}

template<
	class RandomAccessIt,
	class OutputIt
>
OutputIt UTF8Utils::to_utf8(
	RandomAccessIt begin,
	RandomAccessIt const end,
	OutputIt output
) {
	while (end > begin) {
		*output++ = *begin++;
	}
	return output;
}

template<
	class RandomAccessIt,
	class OutputIt
>
OutputIt UTF8Utils::to_utf16(
	RandomAccessIt begin,
	RandomAccessIt const end,
	OutputIt output
) {
	char32 codepoint;
	while (end > begin) {
		begin = decode(begin, end, codepoint);
		output = UTF16Utils::encode(codepoint, output);
	}
	return output;
}

template<
	class RandomAccessIt,
	class OutputIt
>
OutputIt UTF8Utils::to_utf32(
	RandomAccessIt begin,
	RandomAccessIt const end,
	OutputIt output
) {
	char32 codepoint;
	while (end > begin) {
		begin = decode(begin, end, codepoint);
		*output++ = codepoint;
	}
	return output;
}

// specialization UTF16Utils implementation

template<class RandomAccessIt>
RandomAccessIt UTF16Utils::decode(
	RandomAccessIt pos,
	RandomAccessIt const end,
	char32& output,
	char32 const replacement
) {
	if (end <= pos) {
		// Don't try to do anything if we're given an overrun
		return pos;
	}
	RandomAccessIt orig_pos = pos;
	char16 first = *pos++;
	if (DUCT_UTF16_IS_LEAD_SURROGATE(first)) {
		if (end > pos) {
			char16 second = *pos++;
			if (DUCT_UTF16_IS_TRAIL_SURROGATE(second)) {
				// Trail surrogate; we have a pair! Decode that sasquatch
				output = static_cast<char32>(
					((first - 0xD800) << 10) + (second - 0xDC00) + 0x10000);
			} else { // Invalid character
				output = replacement;
				return pos;
			}
		} else { // Incomplete sequence
			// Prevent output from being modified
			return orig_pos;
		}
	} else {
		// Not a surrogate; directly copy (trail surrogate is captured as
		// invalid below)
		output = first;
	}
	if (!DUCT_UNI_IS_CP_VALID(output)) {
		output = replacement;
	}
	return pos;
}

template<class OutputIt>
OutputIt UTF16Utils::encode(
	char32 input,
	OutputIt output,
	char32 const replacement
) {
	if (!DUCT_UNI_IS_CP_VALID(input)) {
		if (DUCT_UNI_IS_CP_VALID(replacement) && CHAR_NULL != replacement) {
			input = replacement;
		} else { // Invalid or null replacement
			return output;
		}
	}
	if (input < 0xFFFE) {
		// Directly convertible to a single unit
		*output++ = static_cast<char16>(input);
	} else {
		// Must be converted to two units
		input -= 0x10000;
		*output++ = static_cast<char16>((input >> 10) + 0xD800);
		*output++ = static_cast<char16>((input & 0x3FFUL) + 0xDC00);
	}
	return output;
}

template<class RandomAccessIt>
RandomAccessIt UTF16Utils::next(
	RandomAccessIt const from,
	RandomAccessIt const end
) {
	if (end <= from) {
		// Quick exit: invalid or unsteppable position
		return from;
	}
	using diff_type = typename std::iterator_traits<RandomAccessIt>::difference_type;
	diff_type const units = DUCT_UTF16_IS_LEAD_SURROGATE(*from) ? 2 : 1;
	return (std::distance(from, end) < units)
		? from
		: from + units;
}

template<class RandomAccessIt>
RandomAccessIt UTF16Utils::prev(
	RandomAccessIt from,
	RandomAccessIt const begin
) {
	RandomAccessIt pos = from;
	if (begin >= from) {
		// Quick exit: invalid or unsteppable position
		return from;
	}
	// Two cases: @a from is either a lead unit or a trail surrogate
	--pos;
	if (DUCT_UTF16_IS_LEAD(*pos)) {
		// Already at the preceeding sequence
		return pos;
	} else if (begin < pos) {
		// Step back once more; true == DUCT_UTF16_IS_TRAIL_SURROGATE(*pos)
		return --pos;
	} else {
		// Incomplete sequence
		return from;
	}
}

inline unsigned UTF16Utils::required_first(
	char_type const first
) {
	return DUCT_UTF16_IS_SURROGATE(first) ? 1 : 0;
}

inline unsigned UTF16Utils::required_first_whole(
	char_type const first
) {
	return DUCT_UTF16_IS_SURROGATE(first) ? 2 : 1;
}

inline unsigned UTF16Utils::required(
	char32 const c
) {
	return (0xFFFF >= c) ? 1 : 2;
}

template<class RandomAccessIt>
std::size_t UTF16Utils::count(
	RandomAccessIt begin,
	RandomAccessIt const end,
	bool const count_incomplete
) {
	std::size_t length = 0;
	using diff_type = typename std::iterator_traits<RandomAccessIt>::difference_type;
	diff_type units;
	while (end > begin) {
		units = DUCT_UTF16_IS_LEAD_SURROGATE(*begin) ? 2 : 1;
		if (std::distance(begin, end) < units) {
			// Incomplete sequence
			length += (count_incomplete ? 1 : 0);
			break;
		} else {
			begin += units;
			++length;
		}
	}
	return length;
}

template<
	class RandomAccessIt,
	class OutputIt
>
OutputIt UTF16Utils::from_utf8(
	RandomAccessIt begin,
	RandomAccessIt const end,
	OutputIt output
) {
	char32 codepoint;
	while (end > begin) {
		begin = UTF8Utils::decode(begin, end, codepoint);
		output = encode(codepoint, output);
	}
	return output;
}

template<
	class RandomAccessIt,
	class OutputIt
>
OutputIt UTF16Utils::from_utf16(
	RandomAccessIt begin,
	RandomAccessIt const end,
	OutputIt output
) {
	while (end > begin) {
		*output++ = *begin++;
	}
	return output;
}

template<
	class RandomAccessIt,
	class OutputIt
>
OutputIt UTF16Utils::from_utf32(
	RandomAccessIt begin,
	RandomAccessIt const end,
	OutputIt output
) {
	while (end > begin) {
		output = encode(*begin++, output);
	}
	return output;
}

template<class OutU, class RandomAccessIt, class OutputIt>
inline OutputIt UTF16Utils::to_other(
	RandomAccessIt begin,
	RandomAccessIt const end,
	OutputIt output
) {
	return OutU::from_utf16(begin, end, output);
}

template<
	class RandomAccessIt,
	class OutputIt
>
OutputIt UTF16Utils::to_utf8(
	RandomAccessIt begin,
	RandomAccessIt const end,
	OutputIt output
) {
	char32 codepoint;
	while (end > begin) {
		begin = decode(begin, end, codepoint);
		output = UTF8Utils::encode(codepoint, output);
	}
	return output;
}

template<
	class RandomAccessIt,
	class OutputIt
>
OutputIt UTF16Utils::to_utf16(
	RandomAccessIt begin,
	RandomAccessIt const end,
	OutputIt output
) {
	while (end > begin) {
		*output++ = *begin++;
	}
	return output;
}

template<
	class RandomAccessIt,
	class OutputIt
>
OutputIt UTF16Utils::to_utf32(
	RandomAccessIt begin,
	RandomAccessIt const end,
	OutputIt output
) {
	char32 codepoint;
	while (end > begin) {
		begin = decode(begin, end, codepoint);
		*output++ = codepoint;
	}
	return output;
}

// specialization UTF32Utils implementation

template<class RandomAccessIt>
inline RandomAccessIt UTF32Utils::decode(
	RandomAccessIt pos,
	RandomAccessIt const end,
	char32& output,
	char32 const replacement
) {
	if (end > pos) { // Don't try to do anything if we're given an overrun
		output = *pos++;
		if (!DUCT_UNI_IS_CP_VALID(output)) {
			output = replacement;
		}
	}
	return pos;
}

template<class OutputIt>
inline OutputIt UTF32Utils::encode(
	char32 input,
	OutputIt output,
	char32 const replacement
) {
	if (DUCT_UNI_IS_CP_VALID(input)) {
		*output++ = input;
	} else if (CHAR_NULL != replacement && DUCT_UNI_IS_CP_VALID(replacement)) {
		*output++ = replacement;
	}
	return output;
}

template<class RandomAccessIt>
inline RandomAccessIt UTF32Utils::next(
	RandomAccessIt const from,
	RandomAccessIt const end
) {
	return (end > from)
		? from + 1
		: from;
}

template<class RandomAccessIt>
inline RandomAccessIt UTF32Utils::prev(
	RandomAccessIt from,
	RandomAccessIt const begin
) {
	return (begin < from)
		? from - 1
		: from;
}

inline unsigned UTF32Utils::required_first(
	char_type const
) {
	return 0;
}

inline unsigned UTF32Utils::required_first_whole(
	char_type const
) {
	return 1;
}

inline unsigned UTF32Utils::required(
	char32 const
) {
	return 1;
}

template<class RandomAccessIt>
inline std::size_t UTF32Utils::count(
	RandomAccessIt begin,
	RandomAccessIt const end,
	bool const
) {
	return std::distance(begin, end);
}

template<class RandomAccessIt>
char32 UTF32Utils::decode_locale(
	RandomAccessIt input,
	std::locale const& locale
) {
	// On Windows, gcc's standard library (glibc++) has almost no support
	// for Unicode stuff. As a consequence, in this context we can only
	// use the default locale and ignore the one passed as parameter.
	#if defined(DUCT_PLATFORM_SYSTEM_WINDOWS) &&			\
		 (defined(__GLIBCPP__) || defined (__GLIBCXX__)) &&	\
		!(defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION))
		wchar_t character = 0;
		mbtowc(&character, &input, 1);
		return static_cast<char32>(character);
	#else
		// Get the facet of the locale which deals with character conversion
		std::ctype<wchar_t> const& facet
			= std::use_facet<std::ctype<wchar_t> >(locale);
		// Use the facet to convert each character of the input string
		return static_cast<char32>(facet.widen(input));
	#endif
}

template<class OutputIt>
OutputIt UTF32Utils::encode_locale(
	char32 input,
	OutputIt output,
	char8 const replacement,
	std::locale const& locale
) {
	// On Windows, gcc's standard library (glibc++) has almost no support
	// for Unicode stuff. As a consequence, in this context we can only
	// use the default locale and ignore the one passed as parameter.
	#if defined(DUCT_PLATFORM_SYSTEM_WINDOWS) &&			\
		 (defined(__GLIBCPP__) || defined (__GLIBCXX__)) &&	\
		!(defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION))
		char8 character = 0;
		if (wctomb(&character, static_cast<wchar_t>(input)) >= 0) {
			*output++ = character;
		} else if (replacement) {
			*output++ = replacement;
		}
		return output;
	#else
		// Get the facet of the locale which deals with character conversion
		std::ctype<wchar_t> const& facet
			= std::use_facet<std::ctype<wchar_t> >(locale);
		// Use the facet to convert each character of the input string
		*output++ = facet.narrow(static_cast<wchar_t>(input), replacement);
		return output;
	#endif
}

template<class RandomAccessIt>
inline char32 UTF32Utils::decode_wide(
	RandomAccessIt input
) {
	// For both UCS-2 and UCS-4, a copy is sufficient (UCS-2 is a subset of
	// UCS-4, and UCS-4 *is* UTF-32).
	return input;
}

template<class OutputIt>
OutputIt UTF32Utils::encode_wide(
	char32 input,
	OutputIt output,
	char32 const replacement
) {
	switch (sizeof(wchar_t)) {
		// UCS-4 == UTF-32
		case 4: {
			*output++ = static_cast<wchar_t>(input);
			break;
		}
		// UCS-2: Need to check if the source code point fits
		default: {
			// TODO: encode the code point to two code units?
			if ((input <= 0xFFFF) && ((input < 0xD800) || (input > 0xDFFF))) {
				*output++ = static_cast<wchar_t>(input);
			} else if (
				replacement &&
				 (replacement <= 0xFFFF) &&
				((replacement <  0xD800) || (replacement > 0xDFFF))
			) {
				*output++ = replacement;
			}
			break;
		}
	}
	return output;
}

template<
	class RandomAccessIt,
	class OutputIt
>
OutputIt UTF32Utils::from_utf8(
	RandomAccessIt begin,
	RandomAccessIt const end,
	OutputIt output
) {
	char32 codepoint;
	while (end > begin) {
		begin = UTF8Utils::decode(begin, end, codepoint);
		*output++ = codepoint;
	}
	return output;
}

template<
	class RandomAccessIt,
	class OutputIt
>
OutputIt UTF32Utils::from_utf16(
	RandomAccessIt begin,
	RandomAccessIt const end,
	OutputIt output
) {
	char32 codepoint;
	while (end > begin) {
		begin = UTF16Utils::decode(begin, end, codepoint);
		*output++ = codepoint;
	}
	return output;
}

template<
	class RandomAccessIt,
	class OutputIt
>
OutputIt UTF32Utils::from_utf32(
	RandomAccessIt begin,
	RandomAccessIt const end,
	OutputIt output
) {
	while (end > begin) {
		*output++ = *begin++;
	}
	return output;
}

template<class OutU, class RandomAccessIt, class OutputIt>
inline OutputIt UTF32Utils::to_other(
	RandomAccessIt begin,
	RandomAccessIt const end,
	OutputIt output
) {
	return OutU::from_utf32(begin, end, output);
}

template<
	class RandomAccessIt,
	class OutputIt
>
OutputIt UTF32Utils::to_utf8(
	RandomAccessIt begin,
	RandomAccessIt const end,
	OutputIt output
) {
	while (end > begin) {
		output = UTF8Utils::encode(*begin++, output);
	}
	return output;
}

template<
	class RandomAccessIt,
	class OutputIt
>
OutputIt UTF32Utils::to_utf16(
	RandomAccessIt begin,
	RandomAccessIt const end,
	OutputIt output
) {
	while (end > begin) {
		output = UTF16Utils::encode(*begin++, output);
	}
	return output;
}

template<
	class RandomAccessIt,
	class OutputIt
>
OutputIt UTF32Utils::to_utf32(
	RandomAccessIt begin,
	RandomAccessIt const end,
	OutputIt output
) {
	while (end > begin) {
		*output++ = *begin++;
	}
	return output;
}

// End of SFML-sourced code
