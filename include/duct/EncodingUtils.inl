/**
@file EncodingUtils.inl
@author Tim Howard

@section LICENSE
Copyright (c) 2010-2011 Tim Howard

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
*/

/*
	The following is a heavily modified version of the Unicode utils in the SFML working tree (as of 17/06/2011)
	See:
	* http://github.com/LaurentGomila/SFML/blob/67455e1a16f797a6e724c701cd9e6a2481384442/include/SFML/System/Utf.hpp
	* http://github.com/LaurentGomila/SFML/blob/67455e1a16f797a6e724c701cd9e6a2481384442/include/SFML/System/Utf.inl
*/

/*
	SFML - Simple and Fast Multimedia Library
	Copyright (C) 2007-2009 Laurent Gomila (laurent.gom@gmail.com)

	This software is provided 'as-is', without any express or implied warranty.
	In no event will the authors be held liable for any damages arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it freely,
	subject to the following restrictions:

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

template<typename InputIterator>
InputIterator UTF8Utils::decode(InputIterator pos, InputIterator const end, char32& output, char32 const replacement) {
	static uint32_t const offsets[6]={0x00000000, 0x00003080, 0x000E2080, 0x03C82080, 0xFA082080, 0x82082080};
	unsigned int trailing_units=g_utf8_trailing[static_cast<char8_strict>(*pos)];
	if (end>pos+trailing_units) {
		output=0;
		switch (trailing_units) {
			case 5: output+=static_cast<char8_strict>(*pos++); output<<=6;
			case 4: output+=static_cast<char8_strict>(*pos++); output<<=6;
			case 3: output+=static_cast<char8_strict>(*pos++); output<<=6;
			case 2: output+=static_cast<char8_strict>(*pos++); output<<=6;
			case 1: output+=static_cast<char8_strict>(*pos++); output<<=6;
			case 0: output+=static_cast<char8_strict>(*pos++);
		}
		output-=offsets[trailing_units];
		if (!DUCT_UNI_IS_CP_VALID(output)) {
			output=replacement;
		}
	} // Else sequence is invalid; pos was not modified, so we can just return
	return pos;
}

template<typename OutputIterator>
OutputIterator UTF8Utils::encode(char32 input, OutputIterator output, char32 const replacement) {
	static char8_strict const utf8_first_bytes[7]={0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};
	if (!DUCT_UNI_IS_CP_VALID(input)) { // Invalid character
		if (DUCT_UNI_IS_CP_VALID(replacement) && CHAR_NULL!=replacement) { // Valid and non-null replacement
			input=replacement;
		} else { // Invalid or null replacement
			return output;
		}
	}
	// Valid code point (input or replaced)
	unsigned int to_write=required(input);
	// Extract bytes
	char8_strict bytes[4];
	switch (to_write) {
		case 4: bytes[3]=static_cast<char8_strict>((input|0x80)&0xBF); input>>=6;
		case 3: bytes[2]=static_cast<char8_strict>((input|0x80)&0xBF); input>>=6;
		case 2: bytes[1]=static_cast<char8_strict>((input|0x80)&0xBF); input>>=6;
		case 1: bytes[0]=static_cast<char8_strict> (input|utf8_first_bytes[to_write]);
	}
	// Add them to the output
	char8_strict const* iter=bytes;
	switch (to_write) {
		case 4: *output++=*iter++;
		case 3: *output++=*iter++;
		case 2: *output++=*iter++;
		case 1: *output++=*iter++;
	}
	return output;
}

template<typename RandomAccessIterator>
RandomAccessIterator UTF8Utils::next(RandomAccessIterator const from, RandomAccessIterator const end) {
	unsigned int units=required_first_whole(*from);
	return ((end-from)<units)
		? from
		: from+units;
}

template<typename RandomAccessIterator>
RandomAccessIterator UTF8Utils::prev(RandomAccessIterator from, RandomAccessIterator const begin) {
	RandomAccessIterator pos=from;
	if (begin<=from) { // Quick exit: invalid or unsteppable position
		return from;
	} else if (DUCT_UTF8_IS_LEAD(*pos)) { // Already aligned to a sequence; move outside it
		--pos;
	}
	while (begin<pos && DUCT_UTF8_IS_TRAIL(*pos)) {
		--pos;
	}
	if (DUCT_UTF8_IS_TRAIL(*pos)) { // Incomplete sequence; begin==pos
		return from;
	} else { // Good; true==DUCT_UTF8_IS_LEAD(*pos)
		return pos;
	}
}

inline unsigned int UTF8Utils::required_first(char_type const first) {
	return g_utf8_trailing[static_cast<char8_strict>(first)];
}

inline unsigned int UTF8Utils::required_first_whole(char_type const first) {
	return 1u+g_utf8_trailing[static_cast<char8_strict>(first)];
}

inline unsigned int UTF8Utils::required(char32 const c) {
	if 		(c< 0x80 || c>0x10FFFF) return 1u; // If greater than 0x10FFFF, the character is invalid and would be replaced with a single unit or skipped
	else if (c< 0x800)		return 2u;
	else if (c< 0x10000)	return 3u;
	else   /*c<=0x10FFFF*/	return 4u;
}

template<typename RandomAccessIterator>
std::size_t UTF8Utils::count(RandomAccessIterator begin, RandomAccessIterator const end, bool const count_incomplete) {
	std::size_t length=0;
	unsigned int units;
	while (end>begin) {
		units=required_first_whole(*begin);
		if ((end-begin)<units) { // Incomplete sequence
			length+=(count_incomplete ? 1 : 0);
			break;
		} else {
			begin+=units;
			++length;
		}
	}
	return length;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF8Utils::from_utf8(InputIterator begin, InputIterator const end, OutputIterator output) {
	while (end>begin) {
		*output++=*begin++;
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF8Utils::from_utf16(InputIterator begin, InputIterator const end, OutputIterator output) {
	char32 codepoint;
	while (end>begin) {
		begin=UTF16Utils::decode(begin, end, codepoint);
		output=encode(codepoint, output);
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF8Utils::from_utf32(InputIterator begin, InputIterator const end, OutputIterator output) {
	while (end>begin) {
		output=encode(*begin++, output);
	}
	return output;
}

template<class outU, typename InputIterator, typename OutputIterator>
OutputIterator UTF8Utils::to_other(InputIterator begin, InputIterator const end, OutputIterator output) {
	return outU::from_utf8(begin, end, output);
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF8Utils::to_utf8(InputIterator begin, InputIterator const end, OutputIterator output) {
	while (end>begin) {
		*output++=*begin++;
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF8Utils::to_utf16(InputIterator begin, InputIterator const end, OutputIterator output) {
	char32 codepoint;
	while (end>begin) {
		begin=decode(begin, end, codepoint);
		output=UTF16Utils::encode(codepoint, output);
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF8Utils::to_utf32(InputIterator begin, InputIterator const end, OutputIterator output) {
	char32 codepoint;
	while (end>begin) {
		begin=decode(begin, end, codepoint);
		*output++=codepoint;
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF8Utils::from_locale(InputIterator begin, InputIterator const end, OutputIterator output, std::locale const& locale) {
	while (end>begin) {
		char32 codepoint=UTF32Utils::decode_locale(*begin++, locale);
		output=encode(codepoint, output);
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF8Utils::from_wide(InputIterator begin, InputIterator const end, OutputIterator output) {
	while (end>begin) {
		char32 codepoint=UTF32Utils::decode_wide(*begin++);
		output=encode(codepoint, output);
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF8Utils::from_latin1(InputIterator begin, InputIterator const end, OutputIterator output) {
	// Latin-1 is directly compatible with Unicode encodings,
	// and can thus be treated as (a sub-range of) UTF-32
	while (end>begin) {
		output=encode(*begin++, output);
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF8Utils::to_locale(InputIterator begin, InputIterator const end, OutputIterator output, char8 const replacement, std::locale const& locale) {
	char32 codepoint;
	while (end>begin) {
		begin=decode(begin, end, codepoint);
		output=UTF32Utils::encode_locale(codepoint, output, replacement, locale);
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF8Utils::to_wide(InputIterator begin, InputIterator const end, OutputIterator output, char32 const replacement) {
	char32 codepoint;
	while (end>begin) {
		begin=decode(begin, end, codepoint);
		output=UTF32Utils::encode_wide(codepoint, output, replacement);
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF8Utils::to_latin1(InputIterator begin, InputIterator const end, OutputIterator output, char8 const replacement) {
	// Latin-1 is directly compatible with Unicode encodings,
	// and can thus be treated as (a sub-range of) UTF-32
	char32 codepoint;
	while (end>begin) {
		begin=decode(begin, end, codepoint);
		*output++=codepoint<256 ? static_cast<char>(codepoint) : replacement;
	}
	return output;
}

// specialization UTF16Utils implementation

template<typename InputIterator>
InputIterator UTF16Utils::decode(InputIterator pos, InputIterator const end, char32& output, char32 const replacement) {
	if (end<=pos) { // Don't try to do anything if we're given an overrun
		return pos;
	}
	char16 first=*pos++;
	if (DUCT_UTF16_IS_LEAD_SURROGATE(first)) { // Lead surrogate
		if (end>pos) {
			char16 second=*pos++;
			if (DUCT_UTF16_IS_TRAIL_SURROGATE(second)) { // Trail surrogate; we have a pair! Decode that sasquatch
				output=static_cast<char32>(((first-0xD800)<<10)+(second-0xDC00)+0x10000);
			} else { // Invalid character
				output=replacement;
				return pos; // Skip invalidity check
			}
		} else { // Incomplete sequence
			return --pos; // Prevent output from being modified
		}
	} else { // Not a surrogate; directly copy (trail surrogate is captured as invalid below with DUCT_UNI_IS_CP_VALID)
		output=first;
	}
	if (!DUCT_UNI_IS_CP_VALID(output)) { // Test invalidity
		output=replacement;
	}
	return pos;
}

template<typename OutputIterator>
OutputIterator UTF16Utils::encode(char32 input, OutputIterator output, char32 const replacement) {
	if (!DUCT_UNI_IS_CP_VALID(input)) { // Invalid character
		if (DUCT_UNI_IS_CP_VALID(replacement) && CHAR_NULL!=replacement) { // Valid and non-null replacement
			input=replacement;
		} else { // Invalid or null replacement
			return output;
		}
	}
	if (input<0xFFFE) { // Valid character; directly convertible to a single unit
		*output++=static_cast<char16>(input);
	} else { // Must be converted to two units
		input-=0x10000;
		*output++=static_cast<char16>((input>>10)+0xD800);
		*output++=static_cast<char16>((input&0x3FFUL)+0xDC00);
	}
	return output;
}

template<typename RandomAccessIterator>
RandomAccessIterator UTF16Utils::next(RandomAccessIterator const from, RandomAccessIterator const end) {
	unsigned int units=DUCT_UTF16_IS_LEAD_SURROGATE(*from) ? 2 : 1;
	return ((end-from)<units)
		? from
		: from+units;
}

template<typename RandomAccessIterator>
RandomAccessIterator UTF16Utils::prev(RandomAccessIterator from, RandomAccessIterator const begin) {
	RandomAccessIterator pos=from;
	if (begin>=from) { // Quick exit: invalid or unsteppable position
		return from;
	}
	--pos; // Either @a from is a lead unit or a trail surrogate
	if (DUCT_UTF16_IS_LEAD(*pos)) { // Already at the preceeding sequence
		return pos;
	} else if (begin<pos) { // Step back once more; true==DUCT_UTF16_IS_TRAIL_SURROGATE(*pos)
		return --pos;
	} else { // Incomplete sequence
		return from;
	}
}

inline unsigned int UTF16Utils::required_first(char_type const first) {
	return DUCT_UTF16_IS_SURROGATE(first) ? 1 : 0;
}

inline unsigned int UTF16Utils::required_first_whole(char_type const first) {
	return DUCT_UTF16_IS_SURROGATE(first) ? 2 : 1;
}

inline unsigned int UTF16Utils::required(char32 const c) {
	return (0xFFFF>=c) ? 1 : 2;
}

template<typename RandomAccessIterator>
std::size_t UTF16Utils::count(RandomAccessIterator begin, RandomAccessIterator const end, bool const count_incomplete) {
	std::size_t length=0;
	unsigned int units;
	while (end>begin) {
		units=DUCT_UTF16_IS_LEAD_SURROGATE(*begin) ? 2 : 1;
		if ((end-begin)<units) { // Incomplete sequence
			length+=(count_incomplete ? 1 : 0);
			break;
		} else {
			begin+=units;
			++length;
		}
	}
	return length;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF16Utils::from_utf8(InputIterator begin, InputIterator const end, OutputIterator output) {
	char32 codepoint;
	while (end>begin) {
		begin=UTF8Utils::decode(begin, end, codepoint);
		output=encode(codepoint, output);
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF16Utils::from_utf16(InputIterator begin, InputIterator const end, OutputIterator output) {
	while (end>begin) {
		*output++=*begin++;
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF16Utils::from_utf32(InputIterator begin, InputIterator const end, OutputIterator output) {
	while (end>begin) {
		output=encode(*begin++, output);
	}
	return output;
}

template<class outU, typename InputIterator, typename OutputIterator>
OutputIterator UTF16Utils::to_other(InputIterator begin, InputIterator const end, OutputIterator output) {
	return outU::from_utf16(begin, end, output);
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF16Utils::to_utf8(InputIterator begin, InputIterator const end, OutputIterator output) {
	char32 codepoint;
	while (end>begin) {
		begin=decode(begin, end, codepoint);
		output=UTF8Utils::encode(codepoint, output);
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF16Utils::to_utf16(InputIterator begin, InputIterator const end, OutputIterator output) {
	while (end>begin) {
		*output++=*begin++;
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF16Utils::to_utf32(InputIterator begin, InputIterator const end, OutputIterator output) {
	char32 codepoint;
	while (end>begin) {
		begin=decode(begin, end, codepoint);
		*output++=codepoint;
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF16Utils::from_locale(InputIterator begin, InputIterator const end, OutputIterator output, std::locale const& locale) {
	while (end>begin) {
		char32 codepoint=UTF32Utils::decode_locale(*begin++, locale);
		output=encode(codepoint, output);
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF16Utils::from_wide(InputIterator begin, InputIterator const end, OutputIterator output) {
	while (end>begin) {
		char32 codepoint=UTF32Utils::decode_wide(*begin++);
		output=encode(codepoint, output);
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF16Utils::from_latin1(InputIterator begin, InputIterator const end, OutputIterator output) {
	// Latin-1 is directly compatible with Unicode encodings,
	// and can thus be treated as (a sub-range of) UTF-32
	while (end>begin) {
		*output++=*begin++;
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF16Utils::to_locale(InputIterator begin, InputIterator const end, OutputIterator output, char8 const replacement, std::locale const& locale) {
	char32 codepoint;
	while (end>begin) {
		begin=decode(begin, end, codepoint);
		output=UTF32Utils::encode_locale(codepoint, output, replacement, locale);
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF16Utils::to_wide(InputIterator begin, InputIterator const end, OutputIterator output, char32 const replacement) {
	char32 codepoint;
	while (end>begin) {
		begin=decode(begin, end, codepoint);
		output=UTF32Utils::encode_wide(codepoint, output, replacement);
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF16Utils::to_latin1(InputIterator begin, InputIterator const end, OutputIterator output, char8 const replacement) {
	// Latin-1 is directly compatible with Unicode encodings,
	// and can thus be treated as (a sub-range of) UTF-32
	while (end>begin) {
		*output++=(*begin<256) ? static_cast<char>(*begin) : replacement;
		++begin;
	}
	return output;
}

// specialization UTF32Utils implementation

template<typename InputIterator>
InputIterator UTF32Utils::decode(InputIterator pos, InputIterator const end, char32& output, char32 const replacement) {
	if (end>pos) { // Don't try to do anything if we're given an overrun
		output=*pos++;
		if (!DUCT_UNI_IS_CP_VALID(output)) {
			output=replacement;
		}
	}
	return pos;
}

template<typename OutputIterator>
OutputIterator UTF32Utils::encode(char32 input, OutputIterator output, char32 const replacement) {
	if (DUCT_UNI_IS_CP_VALID(input)) {
		*output++=input;
	} else if (CHAR_NULL!=replacement && DUCT_UNI_IS_CP_VALID(replacement)) {
		*output++=replacement;
	}
	return output;
}

template<typename RandomAccessIterator>
inline RandomAccessIterator UTF32Utils::next(RandomAccessIterator const from, RandomAccessIterator const end) {
	return (end>from)
		? from+1
		: from;
}

template<typename RandomAccessIterator>
inline RandomAccessIterator UTF32Utils::prev(RandomAccessIterator from, RandomAccessIterator const begin) {
	return (begin<from)
		? --from
		: from;
}

inline unsigned int UTF32Utils::required_first(char_type const first) {
	return 0;
}

inline unsigned int UTF32Utils::required_first_whole(char_type const first) {
	return 1;
}

inline unsigned int UTF32Utils::required(char32 const c) {
	return 1;
}

template<typename RandomAccessIterator>
inline std::size_t UTF32Utils::count(RandomAccessIterator begin, RandomAccessIterator const end, bool const) {
	return end-begin;
}

template<typename InputIterator>
char32 UTF32Utils::decode_locale(InputIterator input, std::locale const& locale) {
	// On Windows, gcc's standard library (glibc++) has almost
	// no support for Unicode stuff. As a consequence, in this
	// context we can only use the default locale and ignore
	// the one passed as parameter.
	#if defined(DUCT_PLATFORM_WINDOWS) &&						/* if Windows ... */						  \
	   (defined(__GLIBCPP__) || defined (__GLIBCXX__)) &&		/* ... and standard library is glibc++ ... */ \
	  !(defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION))	/* ... and STLPort is not used on top of it */
		wchar_t character=0;
		mbtowc(&character, &input, 1);
		return static_cast<char32>(character);
	#else
		// Get the facet of the locale which deals with character conversion
		std::ctype<wchar_t> const& facet=std::use_facet<std::ctype<wchar_t> >(locale);
		// Use the facet to convert each character of the input string
		return static_cast<char32>(facet.widen(input));
	#endif
}

template<typename OutputIterator>
OutputIterator UTF32Utils::encode_locale(char32 input, OutputIterator output, char8 const replacement, std::locale const& locale) {
	// On Windows, gcc's standard library (glibc++) has almost
	// no support for Unicode stuff. As a consequence, in this
	// context we can only use the default locale and ignore
	// the one passed as parameter.
	#if defined(DUCT_PLATFORM_WINDOWS) &&						/* if Windows ... */						  \
	   (defined(__GLIBCPP__) || defined (__GLIBCXX__)) &&		/* ... and standard library is glibc++ ... */ \
	  !(defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION))	/* ... and STLPort is not used on top of it */
		char8 character=0;
		if (wctomb(&character, static_cast<wchar_t>(input))>=0) {
			*output++=character;
		} else if (replacement) {
			*output++=replacement;
		}
		return output;
	#else
		// Get the facet of the locale which deals with character conversion
		std::ctype<wchar_t> const& facet=std::use_facet<std::ctype<wchar_t> >(locale);
		// Use the facet to convert each character of the input string
		*output++=facet.narrow(static_cast<wchar_t>(input), replacement);
		return output;
	#endif
}

template<typename InputIterator>
char32 UTF32Utils::decode_wide(InputIterator input) {
	// The encoding of wide characters is not well defined and is left to the system;
	// however we can safely assume that it is UCS-2 on Windows and
	// UCS-4 on Unix systems.
	// InputIterator both cases, a simple copy is enough (UCS-2 is a subset of UCS-4,
	// and UCS-4 *is* UTF-32).
	return input;
}

template<typename OutputIterator>
OutputIterator UTF32Utils::encode_wide(char32 input, OutputIterator output, char32 const replacement) {
	// The encoding of wide characters is not well defined and is left to the system;
	// however we can safely assume that it is UCS-2 on Windows and
	// UCS-4 on Unix systems.
	// For UCS-2 we need to check if the source characters fits in (UCS-2 is a subset of UCS-4).
	// For UCS-4 we can do a direct copy (UCS-4 *is* UTF-32).
	switch (sizeof(wchar_t)) {
		case 4: {
			*output++=static_cast<wchar_t>(input);
			break;
		}
		default: { // TODO: replace with encoding the code point to two code units?
			if ((input<=0xFFFF) && ((input<0xD800) || (input>0xDFFF))) {
				*output++=static_cast<wchar_t>(input);
			} else if (replacement && ((replacement<=0xFFFF) && ((replacement<0xD800) || (replacement>0xDFFF)))) {
				*output++=replacement;
			}
			break;
		}
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF32Utils::from_locale(InputIterator begin, InputIterator const end, OutputIterator output, std::locale const& locale) {
	while (end>begin) {
		*output++=decode_locale(*begin++, locale);
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF32Utils::from_wide(InputIterator begin, InputIterator const end, OutputIterator output) {
	while (end>begin) {
		*output++=decode_wide(*begin++);
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF32Utils::from_latin1(InputIterator begin, InputIterator const end, OutputIterator output) {
	// Latin-1 is directly compatible with Unicode encodings,
	// and can thus be treated as (a sub-range of) UTF-32
	while (end>begin) {
		*output++=*begin++;
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF32Utils::to_locale(InputIterator begin, InputIterator const end, OutputIterator output, char8 const replacement, std::locale const& locale) {
	while (end>begin) {
		output=encode_locale(*begin++, output, replacement, locale);
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF32Utils::to_wide(InputIterator begin, InputIterator const end, OutputIterator output, char32 const replacement) {
	while (end>begin) {
		output=encode_wide(*begin++, output, replacement);
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF32Utils::to_latin1(InputIterator begin, InputIterator const end, OutputIterator output, char8 const replacement) {
	// Latin-1 is directly compatible with Unicode encodings,
	// and can thus be treated as (a sub-range of) UTF-32
	while (end>begin) {
		*output++=(*begin<256) ? static_cast<char>(*begin) : replacement;
		++begin;
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF32Utils::from_utf8(InputIterator begin, InputIterator const end, OutputIterator output) {
	char32 codepoint;
	while (end>begin) {
		begin=UTF8Utils::decode(begin, end, codepoint);
		*output++=codepoint;
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF32Utils::from_utf16(InputIterator begin, InputIterator const end, OutputIterator output) {
	char32 codepoint;
	while (end>begin) {
		begin=UTF16Utils::decode(begin, end, codepoint);
		*output++=codepoint;
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF32Utils::from_utf32(InputIterator begin, InputIterator const end, OutputIterator output) {
	while (end>begin) {
		*output++=*begin++;
	}
	return output;
}

template<class outU, typename InputIterator, typename OutputIterator>
OutputIterator UTF32Utils::to_other(InputIterator begin, InputIterator const end, OutputIterator output) {
	return outU::from_utf32(begin, end, output);
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF32Utils::to_utf8(InputIterator begin, InputIterator const end, OutputIterator output) {
	while (end>begin) {
		output=UTF8Utils::encode(*begin++, output);
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF32Utils::to_utf16(InputIterator begin, InputIterator const end, OutputIterator output) {
	while (end>begin) {
		output=UTF16Utils::encode(*begin++, output);
	}
	return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator UTF32Utils::to_utf32(InputIterator begin, InputIterator const end, OutputIterator output) {
	while (end>begin) {
		*output++=*begin++;
	}
	return output;
}

// End of SFML-sourced code
