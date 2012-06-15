/**
@file EncodingUtils.hpp
@brief Text encoding utilities.

@author Tim Howard
@copyright 2010-2012 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

// TODO: Update naïve fromX() and toX() to consider incomplete sequences

#ifndef DUCT_ENCODINGUTILS_HPP_
#define DUCT_ENCODINGUTILS_HPP_

#include "./config.hpp"
#include "./char.hpp"

#include <stdexcept>
#include <string>
#include <locale>
#include <string.h>

namespace duct {

/**
	@addtogroup string
	@{
*/

/**
	Encoding types.
*/
enum Encoding : unsigned int {
	/** UTF-8. */
	UTF8=1u,
	/** UTF-16. */
	UTF16=2u,
	/** UTF-32. */
	UTF32=4u
};

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

/**
	@name Encoding utilities
	@note All @c <em>something</em>Iterator template type parameters must satisfy (some of) the requirements respective their names. All functions are guaranteed to work with pointers to raw data.
	@warning Unless otherwise stated, the behavior of all functions is undefined if a position is less than the beginning of its range or greater than the end of its range.
	@{
*/

/**
	String encoding utilities template.
	@note BOMs are not handled by these utilities.
	@tparam spec_ Byte size of the encoding's code unit.
*/
template<unsigned int _spec> class EncodingUtils;

/** UTF-8 encoding utilities; convenience typedef. */
typedef EncodingUtils<Encoding::UTF8> UTF8Utils;
/** UTF-16 encoding utilities; convenience typedef. */
typedef EncodingUtils<Encoding::UTF16> UTF16Utils;
/** UTF-32 encoding utilities; convenience typedef. */
typedef EncodingUtils<Encoding::UTF32> UTF32Utils;

/**
	UTF-8 encoding utilities.
*/
template<>
class EncodingUtils<Encoding::UTF8> {
	DUCT_DISALLOW_ALL_CONSTRUCTION(EncodingUtils<Encoding::UTF8>);

public:
	/** Internal character type. */
	typedef char8 char_type;
	/** Internal character type (strict). */
	typedef char8_strict strict_char_type;
	/** Size of internal character type. */
	static constexpr std::size_t char_size=sizeof(char_type);
	/** Utility's ID. */
	static constexpr Encoding id=Encoding::UTF8;

/** @name Base operations */ /// @{
	/**
		Decode a single sequence to a code point.
		@returns Next input position on success; or @a pos if either the input was incomplete or if @a pos had already overrun @a end (@a output is not modified).
		@param pos Position to decode from.
		@param end Ending input iterator.
		@param output Output code point.
		@param replacement Replacement code point. @a output will be set to this only if the decoded code point is invalid.
	*/
	template<typename InputIterator>
	static InputIterator decode(InputIterator pos, InputIterator const end, char32& output, char32 const replacement=CHAR_NULL);
	/**
		Encode a single code point to a sequence.
		@returns The next output position; or @a output if both @a input was invalid and @a replacement was either invalid or equal to @c CHAR_NULL.
		@param input Input code point.
		@param output Output iterator.
		@param replacement Replacement code point. If invalid or equal to @c CHAR_NULL (default) when @a input is invalid, nothing will be outputted (returns @a output).
	*/
	template<typename OutputIterator>
	static OutputIterator encode(char32 input, OutputIterator output, char32 const replacement=CHAR_NULL);
	/**
		Advance to the next sequence.
		@returns The stepped iterator; or @a from if either the sequence was incomplete or @c from>=end.
		@param from Position to advance from.
		@param end End of boundary.
	*/
	template<typename RandomAccessIterator>
	static RandomAccessIterator next(RandomAccessIterator const from, RandomAccessIterator const end);
	/**
		Step backwards to nearest sequence.
		@note Behavior clarifications (assuming valid, steppable input):
		- If @a from is a trail unit, the result will be positioned at the lead unit for the sequence containing @a from.
		- If @a from is a lead unit, the result will be positioned at the lead unit for the sequence preceeding @a from.
		@returns The backwards-stepped iterator; or @a from if the lead unit for the preceeding sequence was not reachable (@a begin is a trail unit).
		@param from Position to step from.
		@param begin Front of boundary.
	*/
	template<typename RandomAccessIterator>
	static RandomAccessIterator prev(RandomAccessIterator from, RandomAccessIterator const begin);
	/**
		Get the number of code units required to complete an encoded sequence based on its first unit.
		@note This <em>does not</em> count the first unit (@a first); see @c required_first_whole().
		@returns The number of code units required to complete the sequence.
		@param first First unit in a code unit sequence.
	*/
	static inline unsigned int required_first(char_type const first);
	/**
		Get the number of code units required to store an encoded sequence based on its first unit.
		@note This @em does count the first unit (@a first); it is equivalent to: @code required_first(first)+1 @endcode
		@returns The number of code units required to store the sequence.
		@param first First unit in a code unit sequence.
	*/
	static inline unsigned int required_first_whole(char_type const first);
	/**
		Get the number of code units required to encode a code point.
		@returns The number of code units required.
		@param c A code point.
	*/
	static inline unsigned int required(char32 const c);
	/**
		Count the number of code points in a code unit range.
		@warning An incomplete trailing sequence will not be counted unless: @code count_incomplete==true @endcode
		@warning The input range is not checked for invalid code points; it will provide exactly the required number of code points to store the input range if it were entirely valid.
		@returns The number of code points in the range.
		@param begin Beginning iterator.
		@param end Ending iterator.
		@param count_incomplete Whether count a trailing incomplete sequence; @c false by default. Set to @c true to be safe when encoding with a non-null replacement character; keep @c false for accuracy.
	*/
	template<typename RandomAccessIterator>
	static std::size_t count(RandomAccessIterator begin, RandomAccessIterator const end, bool const count_incomplete=false);
/// @}

/** @name Conversion between Unicode encodings */ /// @{
	/**
		Convert from UTF-8 code unit range.
		@returns The last output position.
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output iterator.
	*/
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator from_utf8(InputIterator begin, InputIterator const end, OutputIterator output);
	/**
		Convert from UTF-16 code unit range.
		@returns The last output position.
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output iterator.
	*/
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator from_utf16(InputIterator begin, InputIterator const end, OutputIterator output);
	/**
		Convert from UTF-32 code unit range.
		@returns The last output position.
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output iterator.
	*/
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator from_utf32(InputIterator begin, InputIterator const end, OutputIterator output);
	/**
		Convert code unit range to another Unicode encoding.
		@returns The last output position.
		@tparam outU @c EncodingUtils specialization to use for encoding.
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output iterator.
	*/
	template<class outU, typename InputIterator, typename OutputIterator>
	static OutputIterator to_other(InputIterator begin, InputIterator const end, OutputIterator output);
	/**
		Convert code unit range to UTF-8.
		@returns The last output position.
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output iterator.
	*/
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator to_utf8(InputIterator begin, InputIterator const end, OutputIterator output);
	/**
		Convert code unit range to UTF-16.
		@returns The last output position.
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output iterator.
	*/
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator to_utf16(InputIterator begin, InputIterator const end, OutputIterator output);
	/**
		Convert code unit range to UTF-32.
		@returns The last output position.
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output iterator.
	*/
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator to_utf32(InputIterator begin, InputIterator const end, OutputIterator output);
/// @}

/** @name Conversion to other encodings */ /// @{
	/**
		Convert from locale-encoded range.
		@returns The last output position.
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output iterator.
		@param locale Locale to use for decoding; global locale by default.
	*/
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator from_locale(InputIterator begin, InputIterator const end, OutputIterator output, std::locale const& locale=std::locale());
	/**
		Convert from wide-char range.
		@returns The last output position.
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output iterator.
	*/
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator from_wide(InputIterator begin, InputIterator const end, OutputIterator output);
	/**
		Convert from Latin1 range.
		@returns The last output position.
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output iterator.
	*/
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator from_latin1(InputIterator begin, InputIterator const end, OutputIterator output);
	/**
		Convert code unit range to locale.
		@returns The last output position.
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output iterator.
		@param replacement Replacement code unit. If equal to @c CHAR_NULL (default), invalid code points will be ignored.
		@param locale Locale to use for encoding; global locale by default.
	*/
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator to_locale(InputIterator begin, InputIterator const end, OutputIterator output, char8 const replacement=CHAR_NULL, std::locale const& locale=std::locale());
	/**
		Convert code unit range to wide-char.
		@returns The last output position.
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output iterator.
		@param replacement Replacement wide-char. If equal to @c CHAR_NULL (default), invalid code points will be ignored.
	*/
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator to_wide(InputIterator begin, InputIterator const end, OutputIterator output, char32 const replacement=CHAR_NULL);
	/**
		Convert code unit range to Latin1.
		@returns The last output position.
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output iterator.
		@param replacement Replacement code unit. If equal to @c CHAR_NULL (default), invalid code points will be ignored.
	*/
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator to_latin1(InputIterator begin, InputIterator const end, OutputIterator output, char8 const replacement=CHAR_NULL);
/// @}
};

/**
	UTF-16 encoding utilities.
*/
template<>
class EncodingUtils<Encoding::UTF16> {
	DUCT_DISALLOW_ALL_CONSTRUCTION(EncodingUtils<Encoding::UTF16>);

public:
	/** @copydoc UTF8Utils::char_type */
	typedef char16 char_type;
	/** @copydoc UTF8Utils::strict_char_type */
	typedef char16_strict strict_char_type;
	/** @copydoc UTF8Utils::char_size */
	static constexpr std::size_t char_size=sizeof(char_type);
	/** @copydoc UTF8Utils::id */
	static constexpr Encoding id=Encoding::UTF16;

/** @name Base operations */ /// @{
	/** @copydoc UTF8Utils::decode(InputIterator,InputIterator const,char32&,char32) */
	template<typename InputIterator>
	static InputIterator decode(InputIterator pos, InputIterator const end, char32& output, char32 const replacement=CHAR_NULL);
	/** @copydoc UTF8Utils::encode(char32,OutputIterator,char32 const) */
	template<typename OutputIterator>
	static OutputIterator encode(char32 input, OutputIterator output, char32 const replacement=CHAR_NULL);

	/** @copydoc UTF8Utils::next(RandomAccessIterator const,RandomAccessIterator const) */
	template<typename RandomAccessIterator>
	static RandomAccessIterator next(RandomAccessIterator const from, RandomAccessIterator const end);
	/** @copydoc UTF8Utils::prev(RandomAccessIterator,RandomAccessIterator const) */
	template<typename RandomAccessIterator>
	static RandomAccessIterator prev(RandomAccessIterator from, RandomAccessIterator const begin);

	/** @copydoc UTF8Utils::required_first(char const) */
	static inline unsigned int required_first(char_type const first);
	/** @copydoc UTF8Utils::required_first_whole(char const) */
	static inline unsigned int required_first_whole(char_type const first);
	/** @copydoc UTF8Utils::required(char32 const) */
	static inline unsigned int required(char32 const c);

	/** @copydoc UTF8Utils::count(RandomAccessIterator,RandomAccessIterator const,bool const) */
	template<typename RandomAccessIterator>
	static std::size_t count(RandomAccessIterator begin, RandomAccessIterator const end, bool const count_incomplete=false);
/// @}

/** @name Conversion between Unicode encodings */ /// @{
	/** @copydoc UTF8Utils::from_utf8(InputIterator,InputIterator const,OutputIterator) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator from_utf8(InputIterator begin, InputIterator const end, OutputIterator output);
	/** @copydoc UTF8Utils::from_utf16(InputIterator,InputIterator const,OutputIterator) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator from_utf16(InputIterator begin, InputIterator const end, OutputIterator output);
	/** @copydoc UTF8Utils::from_utf32(InputIterator,InputIterator const,OutputIterator) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator from_utf32(InputIterator begin, InputIterator const end, OutputIterator output);
	
	/** @copydoc UTF8Utils::to_other(InputIterator,InputIterator const,OutputIterator) */
	template<class outU, typename InputIterator, typename OutputIterator>
	static OutputIterator to_other(InputIterator begin, InputIterator const end, OutputIterator output);
	/** @copydoc UTF8Utils::to_utf8(InputIterator,InputIterator const,OutputIterator) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator to_utf8(InputIterator begin, InputIterator const end, OutputIterator output);
	/** @copydoc UTF8Utils::to_utf16(InputIterator,InputIterator const,OutputIterator) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator to_utf16(InputIterator begin, InputIterator const end, OutputIterator output);
	/** @copydoc UTF8Utils::to_utf32(InputIterator,InputIterator const,OutputIterator) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator to_utf32(InputIterator begin, InputIterator const end, OutputIterator output);
/// @}

/** @name Conversion to other encodings */ /// @{
	/** @copydoc UTF8Utils::from_locale(InputIterator,InputIterator const,OutputIterator,std::locale const&) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator from_locale(InputIterator begin, InputIterator const end, OutputIterator output, std::locale const& locale=std::locale());
	/** @copydoc UTF8Utils::from_wide(InputIterator,InputIterator const,OutputIterator) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator from_wide(InputIterator begin, InputIterator const end, OutputIterator output);
	/** @copydoc UTF8Utils::from_latin1(InputIterator,InputIterator const,OutputIterator) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator from_latin1(InputIterator begin, InputIterator const end, OutputIterator output);

	/** @copydoc UTF8Utils::to_locale(InputIterator,InputIterator const,OutputIterator,char8 const,std::locale const&) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator to_locale(InputIterator begin, InputIterator const end, OutputIterator output, char8 const replacement=CHAR_NULL, std::locale const& locale=std::locale());
	/** @copydoc UTF8Utils::to_wide(InputIterator,InputIterator const,OutputIterator,char32 const) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator to_wide(InputIterator begin, InputIterator const end, OutputIterator output, char32 const replacement=CHAR_NULL);
	/** @copydoc UTF8Utils::to_latin1(InputIterator,InputIterator const,OutputIterator,char8 const) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator to_latin1(InputIterator begin, InputIterator const end, OutputIterator output, char8 const replacement=CHAR_NULL);
/// @}
};

/**
	UTF-32 encoding utilities.
*/
template<>
class EncodingUtils<Encoding::UTF32> {
	DUCT_DISALLOW_ALL_CONSTRUCTION(EncodingUtils<Encoding::UTF32>);

public:
	/** @copydoc UTF8Utils::char_type */
	typedef char32 char_type;
	/** @copydoc UTF8Utils::strict_char_type */
	typedef char32_strict strict_char_type;
	/** @copydoc UTF8Utils::char_size */
	static constexpr std::size_t char_size=sizeof(char_type);
	/** @copydoc UTF8Utils::id */
	static constexpr Encoding id=Encoding::UTF32;

/** @name Base operations */ /// @{
	/**
		Decode a single code point from a code point range.
		@returns Next input position on success; or @a begin if @a begin had already overrun @a end (@a output is not modified).
		@param begin Beginning input iterator.
		@param end Ending input iterator.
		@param output Output code point.
		@param replacement Replacement code point. @a output will be set to this only if the decoded code point is invalid.
	*/
	template<typename InputIterator>
	static InputIterator decode(InputIterator begin, InputIterator const end, char32& output, char32 const replacement=CHAR_NULL);
	/**
		Copy code point to output iterator.
		@note This will step at most @c 1 element.
		@returns The next output position; or @a output if both @a input was invalid and @a replacement was either invalid or equal to @c CHAR_NULL.
		@param input Input code point.
		@param output Output iterator.
		@param replacement Replacement code point. If invalid or equal to @c CHAR_NULL (default) when @a input is invalid, nothing will be outputted (returns @a output).
	*/
	template<typename OutputIterator>
	static OutputIterator encode(char32 input, OutputIterator output, char32 const replacement=CHAR_NULL);
	/**
		Advance to next code point.
		@returns The stepped iterator; or @a from if @c from>=end.
		@param from Position to advance from.
		@param end End of boundary.
	*/
	template<typename RandomAccessIterator>
	static inline RandomAccessIterator next(RandomAccessIterator const from, RandomAccessIterator const end);
	/**
		Step backwards to nearest sequence.
		@returns The backwards-stepped iterator; or @a from if @c from<=begin.
		@param from Position to step from.
		@param begin Front of boundary.
	*/
	template<typename RandomAccessIterator>
	static inline RandomAccessIterator prev(RandomAccessIterator from, RandomAccessIterator const begin);
	/**
		Get the number of code units required to complete an encoded sequence based on its first unit.
		@note This <em>does not</em> count the first unit (@a first); see @c required_first_whole().
		@returns <strong>Always @c 0</strong>.
		@param first First unit in a code unit sequence.
	*/
	static inline unsigned int required_first(char_type const first);
	/**
		Get the number of code units required to store an encoded sequence based on its first unit.
		@note This @em does count the first unit (@a first); it is equivalent to: @code required_first(first)+1 @endcode
		@returns <strong>Always @c 1</strong>.
		@param first First unit in a code unit sequence.
	*/
	static inline unsigned int required_first_whole(char_type const first);
	/**
		Get the number of code units required to encode a code point.
		@returns <strong>Always @c 1</strong>.
		@param c A code point.
	*/
	static inline unsigned int required(char32 const c);
	/**
		Count the number of code points in a UTF-32 code point range.
		@warning The input range is not checked for invalid code points; it will provide exactly the required number of code points to store the input range if it were entirely valid.
		@returns <strong>Always:</strong> @c end-begin.
		@param begin Beginning iterator.
		@param end Ending iterator.
		@param count_incomplete Unused.
	*/
	template<typename RandomAccessIterator>
	static inline std::size_t count(RandomAccessIterator begin, RandomAccessIterator const end, bool const count_incomplete=false);

	/**
		Decode locale-encoded unit to a code point.
		@returns The converted code point.
		@param input Beginning input iterator.
		@param locale Locale to use for decoding; global locale by default.
	*/
	template<typename InputIterator>
	static char_type decode_locale(InputIterator input, std::locale const& locale=std::locale());
		/**
		Encode a code point to a locale encoding.
		@returns The last output position.
		@param input A code point to encode.
		@param output Output iterator.
		@param replacement Replacement character. If this is @c CHAR_NULL (default), the non-convertable code point is skipped.
		@param locale Locale to use for encoding; global locale by default.
	*/
	template<typename OutputIterator>
	static OutputIterator encode_locale(char32 input, OutputIterator output, char8 const replacement=CHAR_NULL, std::locale const& locale=std::locale());

	/**
		Decode wide-char unit (uses UTF-16 or UTF-32 depending on the system) to a code point.
		@returns The converted code point.
		@param input Input iterator.
	*/
	template<typename InputIterator>
	static char_type decode_wide(InputIterator input);
	/**
		Encode a code point to the system's wide-char encoding (uses UTF-16 or UTF-32).
		@returns The last output position.
		@param input Input code point.
		@param output Output iterator.
		@param replacement Replacement wide-char. If this is @c CHAR_NULL (default), the non-convertable code point is skipped.
	*/
	template<typename OutputIterator>
	static OutputIterator encode_wide(char32 input, OutputIterator output, char32 const replacement=CHAR_NULL);
/// @}

/** @name Conversion between Unicode encodings */ /// @{
	/** @copydoc UTF8Utils::from_utf8(InputIterator,InputIterator const,OutputIterator) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator from_utf8(InputIterator begin, InputIterator const end, OutputIterator output);
	/** @copydoc UTF8Utils::from_utf16(InputIterator,InputIterator const,OutputIterator) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator from_utf16(InputIterator begin, InputIterator const end, OutputIterator output);
	/** @copydoc UTF8Utils::from_utf32(InputIterator,InputIterator const,OutputIterator) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator from_utf32(InputIterator begin, InputIterator const end, OutputIterator output);
	
	/** @copydoc UTF8Utils::to_other(InputIterator,InputIterator const,OutputIterator) */
	template<class outU, typename InputIterator, typename OutputIterator>
	static OutputIterator to_other(InputIterator begin, InputIterator const end, OutputIterator output);
	/** @copydoc UTF8Utils::to_utf8(InputIterator,InputIterator const,OutputIterator) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator to_utf8(InputIterator begin, InputIterator const end, OutputIterator output);
	/** @copydoc UTF8Utils::to_utf16(InputIterator,InputIterator const,OutputIterator) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator to_utf16(InputIterator begin, InputIterator const end, OutputIterator output);
	/** @copydoc UTF8Utils::to_utf32(InputIterator,InputIterator const,OutputIterator) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator to_utf32(InputIterator begin, InputIterator const end, OutputIterator output);
/// @}

/** @name Conversion to other encodings */ /// @{
	/** @copydoc UTF8Utils::from_locale(InputIterator,InputIterator const,OutputIterator,std::locale const&) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator from_locale(InputIterator begin, InputIterator const end, OutputIterator output, std::locale const& locale=std::locale());
	/** @copydoc UTF8Utils::from_wide(InputIterator,InputIterator const,OutputIterator) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator from_wide(InputIterator begin, InputIterator const end, OutputIterator output);
	/** @copydoc UTF8Utils::from_latin1(InputIterator,InputIterator const,OutputIterator) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator from_latin1(InputIterator begin, InputIterator const end, OutputIterator output);

	/** @copydoc UTF8Utils::to_locale(InputIterator,InputIterator const,OutputIterator,char8 const,std::locale const&) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator to_locale(InputIterator begin, InputIterator const end, OutputIterator output, char8 const replacement=CHAR_NULL, std::locale const& locale=std::locale());
	/** @copydoc UTF8Utils::to_wide(InputIterator,InputIterator const,OutputIterator,char32 const) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator to_wide(InputIterator begin, InputIterator const end, OutputIterator output, char32 const replacement=CHAR_NULL);
	/** @copydoc UTF8Utils::to_latin1(InputIterator,InputIterator const,OutputIterator,char8 const) */
	template<typename InputIterator, typename OutputIterator>
	static OutputIterator to_latin1(InputIterator begin, InputIterator const end, OutputIterator output, char8 const replacement=CHAR_NULL);
/// @}
};

/** @} */ // end of name-group Utilities
/** @} */ // end of doc-group string

#include "./impl/EncodingUtils.inl"

// End of SFML-sourced code

} // namespace duct

#endif // DUCT_ENCODINGUTILS_HPP_
