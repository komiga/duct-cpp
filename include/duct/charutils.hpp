/**
@file charutils.hpp
@author Tim Howard

@section LICENSE
Copyright (c) 2010 Tim Howard

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

Character enum and character utils.
*/

#ifndef _DUCT_CHARUTILS_HPP
#define _DUCT_CHARUTILS_HPP

#include <unicode/unistr.h>
#include <duct/config.hpp>
#include <duct/variables.hpp>

namespace duct {

enum Characters {
	/**
		EOF character.
	*/
	CHAR_EOF=U_SENTINEL,
	/**
		Newline (linefeed).
	*/
	CHAR_NEWLINE='\n',
	/**
		Carriage return.
	*/
	CHAR_CARRIAGERETURN='\r',
	/**
		Tabulation.
	*/
	CHAR_TAB='\t',
	/**
		Decimal point (period).
	*/
	CHAR_DECIMALPOINT='.',
	/**
		Quotation mark.
	*/
	CHAR_QUOTE='\"',
	/**
		Apostrophe.
	*/
	CHAR_APOSTROPHE='\'',
	/**
		Forward-slash.
	*/
	CHAR_SLASH='/',
	/**
		Back-slash.
	*/
	CHAR_BACKSLASH='\\',
	/**
		Asterisk.
	*/
	CHAR_ASTERISK='*',
	/**
		Open brace.
	*/
	CHAR_OPENBRACE='{',
	/**
		Close brace.
	*/
	CHAR_CLOSEBRACE='}',
	/**
		Open bracket.
	*/
	CHAR_OPENBRACKET='[',
	/**
		Close bracket.
	*/
	CHAR_CLOSEBRACKET=']',
	/**
		Equality sign.
	*/
	CHAR_EQUALSIGN='=',
	/**
		Semicolon.
	*/
	CHAR_SEMICOLON=';',
	/**
		Dash (hyphen/minus sign).
	*/
	CHAR_DASH='-',
	/**
		Lowercase N.
	*/
	CHAR_N='n',
	/**
		Lowercase R.
	*/
	CHAR_R='r',
	/**
		Lowercase T.
	*/
	CHAR_T='t'
};

namespace CharUtils {

/**
	Get the given escape character.
	See the spec for details.
	@returns The character for the given escape character, or CHAR_EOF if the given character was not an escape character.
	@param c The escape character.
*/
DUCT_API UChar32 getEscapeChar(UChar32 c);
/**
	Escape characters in the given string using the given format.
	If an existing escape sequence in the string is invalid, it will be interpreted as escapable, replacing the single backslash with an escaped backslash.
	@returns Nothing.
	@param result The output string.
	@param str The string to escape.
	@param format The format to use. Only FMT_STRING_ESCAPE_* formats are relevant.
*/
DUCT_API void escapeString(UnicodeString& result, const UnicodeString& str, unsigned int format=FMT_STRING_ESCAPE_OTHER);

} // namespace CharUtils

} // namespace duct

#endif // _DUCT_CHARUTILS_HPP

