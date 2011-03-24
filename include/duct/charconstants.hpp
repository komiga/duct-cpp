/**
@file charconstants.hpp
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

Common character constants.
*/

#ifndef _DUCT_CHARCONSTANTS_HPP
#define _DUCT_CHARCONSTANTS_HPP

#include <unicode/unistr.h>
#include <duct/config.hpp>

namespace duct {

/**
	EOF character.
*/
const UChar32 CHAR_EOF=U_SENTINEL;
/**
	Newline (linefeed).
*/
const UChar32 CHAR_NEWLINE='\n';
/**
	Carriage return.
*/
const UChar32 CHAR_CARRIAGERETURN='\r';
/**
	Decimal point (period).
*/
const UChar32 CHAR_DECIMALPOINT='.';
/**
	Double-quote.
*/
const UChar32 CHAR_QUOTE='\"';
/**
	Forward-slash.
*/
const UChar32 CHAR_SLASH='/';
/**
	Back-slash.
*/
const UChar32 CHAR_BACKSLASH='\\';
/**
	Asterisk.
*/
const UChar32 CHAR_ASTERISK='*';
/**
	Open brace.
*/
const UChar32 CHAR_OPENBRACE='{';
/**
	Close brace.
*/
const UChar32 CHAR_CLOSEBRACE='}';
/**
	Open bracket.
*/
const UChar32 CHAR_OPENBRACKET='[';
/**
	Close bracket.
*/
const UChar32 CHAR_CLOSEBRACKET=']';
/**
	Equality sign.
*/
const UChar32 CHAR_EQUALSIGN='=';
/**
	Semicolon.
*/
const UChar32 CHAR_SEMICOLON=';';
/**
	Dash (hyphen/minus sign).
*/
const UChar32 CHAR_DASH='-';

} // namespace duct

#endif // _DUCT_CHARCONSTANTS_HPP

