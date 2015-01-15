/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Text encoding utilities.
*/

// TODO: Update naïve fromX() and toX() to consider incomplete sequences

#pragma once

#include "./config.hpp"
#include "./char.hpp"
#include "./traits.hpp"

#include <iterator>
#include <locale>

namespace duct {

/**
	@addtogroup text
	@{
*/

/**
	Encoding types.
*/
enum Encoding : unsigned {
	/** UTF-8. */
	UTF8 = 1u,
	/** UTF-16. */
	UTF16 = 2u,
	/** UTF-32. */
	UTF32 = 4u
};

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

/**
	@name Encoding utilities

	@note All @c <em>something</em>It template type parameters must satisfy
	(some of) the requirements respective their names. All functions are
	guaranteed to work with pointers to plain data.

	@warning The behavior of a function is undefined if a position is outside
	its valid range (unless otherwise specified).
	@{
*/

/**
	String encoding utilities template.

	@note BOMs are not handled by these utilities.
	@tparam Spec_ Byte size of the encoding's code unit.
*/
template<unsigned Spec_>
class EncodingUtils;

/** UTF-8 encoding utilities; convenience alias. */
using UTF8Utils = EncodingUtils<Encoding::UTF8>;
/** UTF-16 encoding utilities; convenience alias. */
using UTF16Utils = EncodingUtils<Encoding::UTF16>;
/** UTF-32 encoding utilities; convenience alias. */
using UTF32Utils = EncodingUtils<Encoding::UTF32>;

/**
	UTF-8 encoding utilities.
*/
template<>
class EncodingUtils<Encoding::UTF8> final
	: public traits::restrict_all
{
public:
/** @name Types and traits */ /// @{
	/** Internal character type. */
	using char_type = char8;
	/** Internal character type (strict). */
	using strict_char_type = char8_strict;
	/** Size of internal character type. */
	static constexpr std::size_t
	char_size = sizeof(char_type);
	/** Maximum number of units required to encode a code point. */
	static constexpr std::size_t
	max_units = 4u;
	/** Utility's ID. */
	static constexpr Encoding
	id = Encoding::UTF8;
/// @}

/** @name Base operations */ /// @{
	/**
		Decode a single sequence to a code point.

		@returns
		- The next input position on success; or
		- @a pos if either the input was incomplete or if @a pos had already
		  overrun.
		@a end (@a output is not modified).
		@param pos Position to decode from.
		@param end Ending input iterator.
		@param output Output code point.
		@param replacement Replacement code point. @a output will be set to
		this if the decoded code point is invalid.
	*/
	template<class RandomAccessIt>
	static RandomAccessIt
	decode(
		RandomAccessIt pos,
		RandomAccessIt const end,
		char32& output,
		char32 const replacement = CHAR_NULL
	);
	/**
		Encode a single code point to a sequence.
		@returns The next output position; or @a output if both @a input was
		invalid and @a replacement was either invalid or equal to @c CHAR_NULL.
		@param input Input code point.
		@param output Output iterator.
		@param replacement Replacement code point. If invalid or equal
		to @c CHAR_NULL (default) when @a input is invalid, nothing will
		be outputted (returns @a output).
	*/
	template<class OutputIt>
	static OutputIt
	encode(
		char32 input,
		OutputIt output,
		char32 const replacement = CHAR_NULL
	);
	/**
		Advance to the next sequence.
		@returns
		- The stepped iterator; or
		- @a from if either the sequence was incomplete or @c from>=end.
		@param from Position to advance from.
		@param end End of boundary.
	*/
	template<class RandomAccessIt>
	static RandomAccessIt
	next(
		RandomAccessIt const from,
		RandomAccessIt const end
	);
	/**
		Step backwards to nearest sequence.
		@note Behavior clarifications (assuming valid, steppable input):
		- If @a from is a trail unit, the result will be positioned at the lead
		  unit for the sequence containing @a from.
		- If @a from is a lead unit, the result will be positioned at the lead
		  unit for the sequence preceeding @a from.
		@returns
		- The backwards-stepped iterator; or
		- @a from if the lead unit for the preceeding sequence was not
		  reachable (@a begin is a trail unit).
		@param from Position to step from.
		@param begin Front of boundary.
	*/
	template<class RandomAccessIt>
	static RandomAccessIt
	prev(
		RandomAccessIt from,
		RandomAccessIt const begin
	);
	/**
		Get the number of code units required to complete an encoded sequence
		based on its first unit.
		@note This <em>does not</em> count the first unit (@a first);
		see @c required_first_whole().
		@returns The number of code units required to complete the sequence.
		@param first First unit in a code unit sequence.
	*/
	inline static unsigned
	required_first(
		char_type const first
	);
	/**
		Get the number of code units required to store an encoded sequence
		based on its first unit.
		@note This @em does count the first unit (@a first); it is
		equivalent to:
		  @code required_first(first)+1 @endcode
		@returns The number of code units required to store the sequence.
		@param first First unit in a code unit sequence.
	*/
	inline static unsigned
	required_first_whole(
		char_type const first
	);
	/**
		Get the number of code units required to encode a code point.
		@returns The number of code units required.
		@param c A code point.
	*/
	inline static unsigned
	required(
		char32 const c
	);
	/**
		Count the number of code points in a code unit range.
		@warning An incomplete trailing sequence will not be counted unless
		@c count_incomplete==true.
		@warning The input range is not checked for invalid code points; it
		will provide exactly the required number of code points to store the
		input range if it were entirely valid.
		@returns The number of code points in the range.
		@param begin Beginning iterator.
		@param end Ending iterator.
		@param count_incomplete Whether count a trailing incomplete
		sequence; @c false by default. Set to @c true to be safe when encoding
		with a non-null replacement character; keep @c false for accuracy.
	*/
	template<class RandomAccessIt>
	static std::size_t
	count(
		RandomAccessIt begin,
		RandomAccessIt const end,
		bool const count_incomplete = false
	);
/// @}

/** @name Conversion between Unicode encodings */ /// @{
	/**
		Convert from UTF-8 code unit range.
		@returns The last output position.
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output iterator.
	*/
	template<class RandomAccessIt, class OutputIt>
	static OutputIt
	from_utf8(
		RandomAccessIt begin,
		RandomAccessIt const end,
		OutputIt output
	);
	/**
		Convert from UTF-16 code unit range.
		@returns The last output position.
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output iterator.
	*/
	template<class RandomAccessIt, class OutputIt>
	static OutputIt
	from_utf16(
		RandomAccessIt begin,
		RandomAccessIt const end,
		OutputIt output
	);
	/**
		Convert from UTF-32 code unit range.
		@returns The last output position.
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output iterator.
	*/
	template<class RandomAccessIt, class OutputIt>
	static OutputIt
	from_utf32(
		RandomAccessIt begin,
		RandomAccessIt const end,
		OutputIt output
	);
	/**
		Convert code unit range to another Unicode encoding.
		@returns The last output position.
		@tparam OutU @c EncodingUtils specialization to use for encoding.
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output iterator.
	*/
	template<class OutU, class RandomAccessIt, class OutputIt>
	inline static OutputIt
	to_other(
		RandomAccessIt begin,
		RandomAccessIt const end,
		OutputIt output
	);
	/**
		Convert code unit range to UTF-8.
		@returns The last output position.
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output iterator.
	*/
	template<class RandomAccessIt, class OutputIt>
	static OutputIt
	to_utf8(
		RandomAccessIt begin,
		RandomAccessIt const end,
		OutputIt output
	);
	/**
		Convert code unit range to UTF-16.
		@returns The last output position.
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output iterator.
	*/
	template<class RandomAccessIt, class OutputIt>
	static OutputIt
	to_utf16(
		RandomAccessIt begin,
		RandomAccessIt const end,
		OutputIt output
	);
	/**
		Convert code unit range to UTF-32.
		@returns The last output position.
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output iterator.
	*/
	template<class RandomAccessIt, class OutputIt>
	static OutputIt
	to_utf32(
		RandomAccessIt begin,
		RandomAccessIt const end,
		OutputIt output
	);
/// @}
};

/**
	UTF-16 encoding utilities.
*/
template<>
class EncodingUtils<Encoding::UTF16> final
	: public traits::restrict_all
{
public:
/** @name Types and traits */ /// @{
	/** @copydoc UTF8Utils::char_type */
	using char_type = char16;
	/** @copydoc UTF8Utils::strict_char_type */
	using strict_char_type = char16_strict;
	/** @copydoc UTF8Utils::char_size */
	static constexpr std::size_t
	char_size = sizeof(char_type);
	/** @copydoc UTF8Utils::max_units */
	static constexpr std::size_t
	max_units = 2u;
	/** @copydoc UTF8Utils::id */
	static constexpr Encoding
	id = Encoding::UTF16;
/// @}

/** @name Base operations */ /// @{
	/** @copydoc UTF8Utils::decode(RandomAccessIt,RandomAccessIt const,
		char32&,char32) */
	template<class RandomAccessIt>
	static RandomAccessIt
	decode(
		RandomAccessIt pos,
		RandomAccessIt const end,
		char32& output,
		char32 const replacement = CHAR_NULL
	);
	/** @copydoc UTF8Utils::encode(char32,OutputIt,char32 const) */
	template<class OutputIt>
	static OutputIt
	encode(
		char32 input,
		OutputIt output,
		char32 const replacement = CHAR_NULL
	);

	/** @copydoc UTF8Utils::next(RandomAccessIt const,RandomAccessIt const) */
	template<class RandomAccessIt>
	static RandomAccessIt
	next(
		RandomAccessIt const from,
		RandomAccessIt const end
	);
	/** @copydoc UTF8Utils::prev(RandomAccessIt,RandomAccessIt const) */
	template<class RandomAccessIt>
	static RandomAccessIt
	prev(
		RandomAccessIt from,
		RandomAccessIt const begin
	);

	/** @copydoc UTF8Utils::required_first(char const) */
	inline static unsigned
	required_first(
		char_type const first
	);
	/** @copydoc UTF8Utils::required_first_whole(char const) */
	inline static unsigned
	required_first_whole(
		char_type const first
	);
	/** @copydoc UTF8Utils::required(char32 const) */
	inline static unsigned
	required(
		char32 const c
	);

	/** @copydoc UTF8Utils::count(RandomAccessIt,RandomAccessIt const,
		bool const) */
	template<class RandomAccessIt>
	static std::size_t
	count(
		RandomAccessIt begin,
		RandomAccessIt const end,
		bool const count_incomplete = false
	);
/// @}

/** @name Conversion between Unicode encodings */ /// @{
	/** @copydoc UTF8Utils::from_utf8(RandomAccessIt,RandomAccessIt const,
		OutputIt) */
	template<class RandomAccessIt, class OutputIt>
	static OutputIt
	from_utf8(
		RandomAccessIt begin,
		RandomAccessIt const end,
		OutputIt output
	);
	/** @copydoc UTF8Utils::from_utf16(RandomAccessIt,RandomAccessIt const,
		OutputIt) */
	template<class RandomAccessIt, class OutputIt>
	static OutputIt
	from_utf16(
		RandomAccessIt begin,
		RandomAccessIt const end,
		OutputIt output
	);
	/** @copydoc UTF8Utils::from_utf32(RandomAccessIt,RandomAccessIt const,
		OutputIt) */
	template<class RandomAccessIt, class OutputIt>
	static OutputIt
	from_utf32(
		RandomAccessIt begin,
		RandomAccessIt const end,
		OutputIt output
	);
	
	/** @copydoc UTF8Utils::to_other(RandomAccessIt,RandomAccessIt const,
		OutputIt) */
	template<class OutU, class RandomAccessIt, class OutputIt>
	inline static OutputIt
	to_other(
		RandomAccessIt begin,
		RandomAccessIt const end,
		OutputIt output
	);
	/** @copydoc UTF8Utils::to_utf8(RandomAccessIt,RandomAccessIt const,
		OutputIt) */
	template<class RandomAccessIt, class OutputIt>
	static OutputIt
	to_utf8(
		RandomAccessIt begin,
		RandomAccessIt const end,
		OutputIt output
	);
	/** @copydoc UTF8Utils::to_utf16(RandomAccessIt,RandomAccessIt const,
		OutputIt) */
	template<class RandomAccessIt, class OutputIt>
	static OutputIt
	to_utf16(
		RandomAccessIt begin,
		RandomAccessIt const end,
		OutputIt output
	);
	/** @copydoc UTF8Utils::to_utf32(RandomAccessIt,RandomAccessIt const,
		OutputIt) */
	template<class RandomAccessIt, class OutputIt>
	static OutputIt
	to_utf32(
		RandomAccessIt begin,
		RandomAccessIt const end,
		OutputIt output
	);
/// @}
};

/**
	UTF-32 encoding utilities.
*/
template<>
class EncodingUtils<Encoding::UTF32> final
	: public traits::restrict_all
{
public:
/** @name Types and traits */ /// @{
	/** @copydoc UTF8Utils::char_type */
	using char_type = char32;
	/** @copydoc UTF8Utils::strict_char_type */
	using strict_char_type = char32_strict;
	/** @copydoc UTF8Utils::char_size */
	static constexpr std::size_t 
	char_size = sizeof(char_type);
	/** @copydoc UTF8Utils::max_units */
	static constexpr std::size_t 
	max_units = 1u;
	/** @copydoc UTF8Utils::id */
	static constexpr Encoding
	id = Encoding::UTF32;
/// @}

/** @name Base operations */ /// @{
	/**
		Decode a single code point from a code point range.
		@returns
		- The next input position on success; or
		- @a begin if @a begin had already overrun @a end (@a output is not
		  modified).
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output code point.
		@param replacement Replacement code point. @a output will be set to
		this if the decoded code point is invalid.
	*/
	template<class RandomAccessIt>
	inline static RandomAccessIt
	decode(
		RandomAccessIt begin,
		RandomAccessIt const end,
		char32& output,
		char32 const replacement = CHAR_NULL
	);
	/**
		Copy code point to output iterator.
		@note This will step at most @c 1 element.
		@returns
		- The next output position; or
		- @a output if both @a input was invalid and @a replacement was either
		  invalid or equal to @c CHAR_NULL.
		@param input Input code point.
		@param output Output iterator.
		@param replacement Replacement code point. If invalid or equal
		to @c CHAR_NULL (default) when @a input is invalid, nothing will be
		outputted (returns @a output).
	*/
	template<class OutputIt>
	inline static OutputIt
	encode(
		char32 input,
		OutputIt output,
		char32 const replacement = CHAR_NULL
	);
	/**
		Advance to next code point.
		@returns The stepped iterator; or @a from if @c from>=end.
		@param from Position to advance from.
		@param end End of boundary.
	*/
	template<class RandomAccessIt>
	inline static RandomAccessIt
	next(
		RandomAccessIt const from,
		RandomAccessIt const end
	);
	/**
		Step backwards to nearest sequence.
		@returns The backwards-stepped iterator; or @a from if @c from<=begin.
		@param from Position to step from.
		@param begin Front of boundary.
	*/
	template<class RandomAccessIt>
	inline static RandomAccessIt
	prev(
		RandomAccessIt from,
		RandomAccessIt const begin
	);
	/**
		Get the number of code units required to complete an encoded sequence
		based on its first unit.
		@note This <em>does not</em> count the first unit (@a first);
		see @c required_first_whole().
		@returns <strong>Always @c 0</strong>.
		@param first First unit in a code unit sequence.
	*/
	inline static unsigned
	required_first(
		char_type const first
	);
	/**
		Get the number of code units required to store an encoded sequence
		based on its first unit.
		@note This @em does count the first unit (@a first); it is
		equivalent to:
		  @code required_first(first)+1 @endcode
		@returns <strong>Always @c 1</strong>.
		@param first First unit in a code unit sequence.
	*/
	inline static unsigned
	required_first_whole(
		char_type const first
	);
	/**
		Get the number of code units required to encode a code point.
		@returns <strong>Always @c 1</strong>.
		@param c A code point.
	*/
	inline static unsigned
	required(
		char32 const c
	);
	/**
		Count the number of code points in a UTF-32 code point range.
		@warning The input range is not checked for invalid code points; it
		will provide exactly the required number of code points to store the
		input range if it were entirely valid.
		@returns <strong>Always</strong> @c std::distance(begin, end).
		@param begin Beginning iterator.
		@param end Ending iterator.
		@param count_incomplete Unused.
	*/
	template<class RandomAccessIt>
	inline static std::size_t
	count(
		RandomAccessIt begin,
		RandomAccessIt const end,
		bool const count_incomplete = false
	);

	/**
		Decode locale-encoded unit to a code point.
		@returns The converted code point.
		@param input Beginning input iterator.
		@param locale Locale to use for decoding; global locale by default.
	*/
	template<class RandomAccessIt>
	static char_type
	decode_locale(
		RandomAccessIt input,
		std::locale const& locale=std::locale()
	);
	/**
		Encode a code point to a locale encoding.
		@returns The last output position.
		@param input A code point to encode.
		@param output Output iterator.
		@param replacement Replacement character. If this is @c CHAR_NULL
		(default), the non-convertable code point is skipped.
		@param locale Locale to use for encoding; global locale by default.
	*/
	template<class OutputIt>
	static OutputIt
	encode_locale(
		char32 input,
		OutputIt output,
		char8 const replacement = CHAR_NULL,
		std::locale const& locale=std::locale()
	);

	/**
		Decode wide-char unit (uses UTF-16 or UTF-32 depending on the system)
		to a code point.
		@returns The converted code point.
		@param input Input iterator.
	*/
	template<class RandomAccessIt>
	inline static char_type
	decode_wide(
		RandomAccessIt input
	);
	/**
		Encode a code point to the system's wide-char encoding (uses UTF-16 or
		UTF-32).
		@returns The last output position.
		@param input Input code point.
		@param output Output iterator.
		@param replacement Replacement wide-char. If this is @c CHAR_NULL
		(default), the non-convertable code point is skipped.
	*/
	template<class OutputIt>
	static OutputIt
	encode_wide(
		char32 input,
		OutputIt output,
		char32 const replacement = CHAR_NULL
	);
/// @}

/** @name Conversion between Unicode encodings */ /// @{
	/** @copydoc UTF8Utils::from_utf8(RandomAccessIt,RandomAccessIt const,
		OutputIt) */
	template<class RandomAccessIt, class OutputIt>
	static OutputIt
	from_utf8(
		RandomAccessIt begin,
		RandomAccessIt const end,
		OutputIt output
	);
	/** @copydoc UTF8Utils::from_utf16(RandomAccessIt,RandomAccessIt const,
		OutputIt) */
	template<class RandomAccessIt, class OutputIt>
	static OutputIt
	from_utf16(
		RandomAccessIt begin,
		RandomAccessIt const end,
		OutputIt output
	);
	/** @copydoc UTF8Utils::from_utf32(RandomAccessIt,RandomAccessIt const,
		OutputIt) */
	template<class RandomAccessIt, class OutputIt>
	static OutputIt
	from_utf32(
		RandomAccessIt begin,
		RandomAccessIt const end,
		OutputIt output
	);
	
	/** @copydoc UTF8Utils::to_other(RandomAccessIt,RandomAccessIt const,
		OutputIt) */
	template<class OutU, class RandomAccessIt, class OutputIt>
	inline static OutputIt
	to_other(
		RandomAccessIt begin,
		RandomAccessIt const end,
		OutputIt output
	);
	/** @copydoc UTF8Utils::to_utf8(RandomAccessIt,RandomAccessIt const,
		OutputIt) */
	template<class RandomAccessIt, class OutputIt>
	static OutputIt
	to_utf8(
		RandomAccessIt begin,
		RandomAccessIt const end,
		OutputIt output
	);
	/** @copydoc UTF8Utils::to_utf16(RandomAccessIt,RandomAccessIt const,
		OutputIt) */
	template<class RandomAccessIt, class OutputIt>
	static OutputIt
	to_utf16(
		RandomAccessIt begin,
		RandomAccessIt const end,
		OutputIt output
	);
	/** @copydoc UTF8Utils::to_utf32(RandomAccessIt,RandomAccessIt const,
		OutputIt) */
	template<class RandomAccessIt, class OutputIt>
	static OutputIt
	to_utf32(
		RandomAccessIt begin,
		RandomAccessIt const end,
		OutputIt output
	);
/// @}
};

/** @} */ // end of name-group Utilities
/** @} */ // end of doc-group text

#include "./impl/EncodingUtils.inl"

// End of SFML-sourced code

} // namespace duct
