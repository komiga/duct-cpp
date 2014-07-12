/**
@file char.hpp
@brief Character types and constants.

@author Timothy Howard
@copyright 2010-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include "./config.hpp"

//#include <cuchar>

namespace duct {

/**
	@addtogroup text
	@{
*/

#ifdef DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI

/**
	@ingroup config

	When defined, force using the C++11 @c char16_t and @c char32_t
	types instead of @c uint16_t and @c uint32_t for the char16 and
	char32 type aliases.
	This should only be used when duct fails to recognize support.
*/
#define DUCT_CONFIG_FORCE_CPP11_CHAR_TYPES

/**
	@ingroup config

	Defined when the proper C++11 @c char16_t and @c char32_t types
	are used for @c char16 and @c char32.

	@note This is always defined when
	#DUCT_CONFIG_FORCE_CPP11_CHAR_TYPES is defined.
*/
#define DUCT_USING_CPP11_CHAR_TYPES

#endif // DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI

/**
	@name Unicode character types

	@note The C++11 @c char16_t and @c char32_t types will be used
	when available; otherwise, @c uint16_t and @c uint32_t are used.

	@note The strictly-sized character types
	(@c char8_strict, @c char16_strict, and @c char32_strict)
	are integral non-character types; in addition, they will probably
	never be unequal in size to the @c char8, @c char16,
	and @c char32 types.

	@sa
		#DUCT_CONFIG_FORCE_CPP11_CHAR_TYPES,
		#DUCT_USING_CPP11_CHAR_TYPES
	@{
*/
/** Strictly-sized 8-bit character. */
using char8_strict = std::uint8_t;
/** Strictly-sized 16-bit character. */
using char16_strict = std::uint16_t;
/** Strictly-sized 32-bit character. */
using char32_strict = std::uint32_t;

/**
	(Very likely an) 8-bit character; UTF-8 code unit.

	@note This is always aliased to @c char and merely exists for
	consistency.
*/
using char8 = char;

#ifdef DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI

/** 16-bit character; UTF-16 code unit. */
using char16 = system_dependent;
/** 32-bit character; UTF-32 code unit; Unicode code point. */
using char32 = system_dependent;

#else

#if	(defined(DUCT_CONFIG_FORCE_CPP11_CHAR_TYPES)) || \
	((DUCT_COMPILER & DUCT_FLAG_COMPILER_GCC) \
		>= DUCT_FLAG_COMPILER_GCC44) || \
	((DUCT_COMPILER & DUCT_FLAG_COMPILER_CLANG) \
		>= DUCT_FLAG_COMPILER_CLANG29)/* || \
	// FIXME: VC2011 lies; currently aliases to uint16_t and uint32_t.
	// What about the CTP?
	((DUCT_COMPILER & DUCT_FLAG_COMPILER_VC) \
		>= DUCT_FLAG_COMPILER_VC2011)*/

	using char16 = char16_t;
	using char32 = char32_t;

	#define DUCT_USING_CPP11_CHAR_TYPES
#else
	using char16 = char16_strict;
	using char32 = char32_strict;
#endif

#endif // DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI

/** @} */ // end of name-group Unicode character types

/**
	@name Unicode character utilities
	@{
*/

/**
	Check if a code point is a valid Unicode character.

	Non-characters and reserved characters include:
	- Surrogate pairs:
	  @verbatim U+D800 … U+DFFF @endverbatim
	- Unicode 3.2:
	  @verbatim U+FDD0 … U+FDEF @endverbatim
	- Last two code points on each plane:
	  @verbatim U+xxFFFE, U+xxFFFF @endverbatim
	- Any code point above @c U+10FFFF

	@returns
	- @c true if the code point is valid;
	- @c false otherwise.
	@param c Code point to test.
*/
#define DUCT_UNI_IS_CP_VALID(c) ( \
	((c) < 0xD800 || (c) > 0xDFFF) && \
	(c) <= 0x10FFFF && \
	!DUCT_UNI_IS_CP_NONCHAR((c)) \
)
/**
	Check if a code point is a Unicode non-character.

	@returns
	- @c true if the code point is a non-character;
	- @c false otherwise.
	@param c Code point to test.
*/
#define DUCT_UNI_IS_CP_NONCHAR(c) ( \
	((c) >= 0xFDD0 && (c) <= 0xFDEF) || \
	(((c) & 0xFFFE) == 0xFFFE && (c) <= 0x10FFFF) \
)
/**
	Check if a code point is a surrogate.

	@returns
	- @c true if @a c is a surrogate;
	- @c false otherwise.
	@param c Code point or UTF-16 code unit to test.
*/
#define DUCT_UNI_IS_SURROGATE(c) ( \
	((c) & 0xFFFFF800) == 0xD800 \
)

/**
	Check if a UTF-8 code unit is a single encoding unit.

	@returns
	- @c true if @a c is a single encoding unit;
	- @c false otherwise.
	@param c UTF-8 code unit to test.
*/
#define DUCT_UTF8_IS_SINGLE(c) ( \
	((c) & 0x80) == 0 \
)

/**
	Check if a UTF-8 code unit is a lead unit.

	@returns
	- @c true if @a c is a lead unit;
	- @c false otherwise.
	@param c UTF-8 code unit to test.
*/
#define DUCT_UTF8_IS_LEAD(c) ( \
	static_cast<std::uint8_t>((c) - 0xC0) < 0x3E \
)

/**
	Check if a UTF-8 code unit is a trail unit.

	@returns
	- @c true if @a c is a trail unit;
	- @c false otherwise.
	@param c UTF-8 code unit to test.
*/
#define DUCT_UTF8_IS_TRAIL(c) ( \
	((c) & 0xC0) == 0x80 \
)

/**
	Check if a UTF-8 code unit is a single or lead unit.

	@returns
	- @c true if @a c is a single or lead unit;
	- @c false otherwise.
	@param c UTF-8 code unit to test.
*/
#define DUCT_UTF8_IS_HEAD(c) ( \
	DUCT_UTF8_IS_SINGLE(c) || DUCT_UTF8_IS_LEAD(c) \
)

/**
	Check if a UTF-16 code unit is a non-surrogate (that is, encodes
	a code point standalone).

	@note This is the inverse of @c DUCT_UTF16_IS_SURROGATE.

	@returns
	- @c true if @a c is a non-surrogate;
	- @c false otherwise.
	@param c UTF-16 code unit to test.
*/
#define DUCT_UTF16_IS_SINGLE(c) ( \
	!DUCT_UTF16_IS_SURROGATE(c) \
)
/**
	Check if a UTF-16 code units is a lead unit (either a
	non-surrogate or a lead surrogate).

	@returns
	- @c true if @a c is a non-surrogate or a lead surrogate;
	- @c false otherwise.
	@param c UTF-16 code unit to test.
*/
#define DUCT_UTF16_IS_LEAD(c) ( \
	DUCT_UTF16_IS_SINGLE(c) || DUCT_UTF16_IS_LEAD_SURROGATE(c) \
)
/**
	Check if a UTF-16 code unit is a surrogate.

	@returns
	- @c true if @a c is a surrogate;
	- @c false otherwise.
	@param c UTF-16 code unit to test.
	@sa DUCT_UNI_IS_SURROGATE
*/
#define DUCT_UTF16_IS_SURROGATE(c) \
	DUCT_UNI_IS_SURROGATE(c)
/**
	Check if a UTF-16 code unit is a lead surrogate.

	@returns
	- @c true if @a c is a lead surrogate;
	- @c false otherwise.
	@param c UTF-16 code unit to test.
*/
#define DUCT_UTF16_IS_LEAD_SURROGATE(c) ( \
	((c) & 0xFFFFFC00) == 0xD800 \
)
/**
	Check if a UTF-16 code unit is a trail surrogate.

	@returns
	- @c true if @a c is a trail surrogate;
	- @c false otherwise.
	@param c UTF-16 code unit to test.
*/
#define DUCT_UTF16_IS_TRAIL_SURROGATE(c) ( \
	((c) & 0xFFFFFC00) == 0xDC00 \
)

/** @} */ // end of name-group Unicode character utilities

/**
	Various character constants.

	@warning The special-use constants @c CHAR_SENTINEL
	and @c CHAR_EOF are invalid Unicode characters which can be
	unsafely narrowed to a valid UTF-8 code unit (@c U+FF, y with
	diaeresis). These are only useful for indicating state with UTF-16
	and UTF-32.
*/
enum Chars {
	/** NUL. */
	CHAR_NULL = '\0',
	/** Replacement character for decoding UTF-x encoded text. */
	CHAR_SENTINEL = 0xFFFFFFFF,
	/** End of file/stream/sequence/etc. */
	CHAR_EOF = 0xFFFF,
	/** Replacement character (U+FFFD). */
	CHAR_REPLACEMENT = 0xFFFD,
	/** Newline (linefeed). */
	CHAR_NEWLINE = '\n',
	/** Carriage return. */
	CHAR_CARRIAGERETURN = '\r',
	/** Tabulation. */
	CHAR_TAB = '\t',
	/** Decimal point (period). */
	CHAR_DECIMALPOINT = '.',
	/** Quotation mark. */
	CHAR_QUOTE = '\"',
	/** Apostrophe. */
	CHAR_APOSTROPHE = '\'',
	/** Forward-slash. */
	CHAR_SLASH = '/',
	/** Back-slash. */
	CHAR_BACKSLASH = '\\',
	/** Asterisk. */
	CHAR_ASTERISK = '*',
	/** Open brace. */
	CHAR_OPENBRACE = '{',
	/** Close brace. */
	CHAR_CLOSEBRACE = '}',
	/** Open bracket. */
	CHAR_OPENBRACKET = '[',
	/** Close bracket. */
	CHAR_CLOSEBRACKET = ']',
	/** Equality sign. */
	CHAR_EQUALSIGN = '=',
	/** Comma. */
	CHAR_COMMA = ',',
	/** Semicolon. */
	CHAR_SEMICOLON = ';',
	/** Plus sign. */
	CHAR_PLUS = '+',
	/** Dash (hyphen/minus sign). */
	CHAR_DASH = '-',
	/** Lowercase N. */
	CHAR_N = 'n',
	/** Lowercase R. */
	CHAR_R = 'r',
	/** Lowercase T. */
	CHAR_T = 't',
	/** Lowercase F. */
	CHAR_F = 'f'
};

/** @} */ // end of doc-group text

} // namespace duct
