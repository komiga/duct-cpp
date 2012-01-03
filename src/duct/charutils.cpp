/**
@file charutils.cpp
@author Tim Howard

@section LICENSE
Copyright (c) 2010-2012 Tim Howard

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

#include <duct/debug.hpp>
#include <duct/charutils.hpp>

namespace duct {

namespace CharUtils {

UChar const g_uchar_esc_quotation[]={'\\', '\"'};

UChar32 getEscapeChar(UChar32 c) {
	switch (c) {
	case CHAR_N:
		return CHAR_NEWLINE;
	case CHAR_R:
		return CHAR_CARRIAGERETURN;
	case CHAR_T:
		return CHAR_TAB;
	case CHAR_APOSTROPHE:
	case CHAR_QUOTE:
	case CHAR_BACKSLASH:
	case CHAR_OPENBRACE:
	case CHAR_CLOSEBRACE:
	case CHAR_EQUALSIGN:
		return c;
	default:
		return CHAR_EOF;
	}
}

void escapeString(icu::UnicodeString& result, icu::UnicodeString const& str, unsigned int format) {
	if (str.isEmpty()
		|| ((format&FMT_STRING_ESCAPE_OTHER)==0
		&& ( format&FMT_STRING_ESCAPE_CONTROL)==0
		&& ( format&FMT_STRING_ESCAPE_NEWLINE)==0)) {
		result.setTo(str);
		return;
	}
	result.remove();
	int32_t length=str.countChar32(0);
	bool isquoted=(length>=2 && str.char32At(0)=='\"' && str.char32At(length-1)=='\"' && !str.endsWith(g_uchar_esc_quotation, 2));
	for (int i=0; i<length; ++i) {
		UChar32 c=str.char32At(i);
		if ((format&FMT_STRING_ESCAPE_OTHER)!=0) {
			switch (c) {
			case CHAR_TAB:
				if (!isquoted) {
					result+=CHAR_BACKSLASH;
					result+='t';
				} else {
					result+=c;
				}
				continue;
			case CHAR_QUOTE:
				if (/*!isquoted && */(i>0 && i<(length-1))) {
					result+=CHAR_BACKSLASH;
					result+='\"';
				} else {
					result+=CHAR_QUOTE;
				}
				continue;
			case CHAR_BACKSLASH:
				if ((i+1)!=length) {
					UChar32 c2=getEscapeChar(str.char32At(i+1));
					switch (c2) {
					case CHAR_BACKSLASH:
						i++; // we don't want to see the slash again; the continue below makes the i-uppage 2
						result+=CHAR_BACKSLASH;
						result+=CHAR_BACKSLASH;
						continue;
					case CHAR_EOF:
						result+=CHAR_BACKSLASH;
						result+=CHAR_BACKSLASH;
						continue;
					default:
						result+=CHAR_BACKSLASH;
						result+=c2;
						i++; // already a valid escape sequence
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
		if ((format&FMT_STRING_ESCAPE_CONTROL)!=0 && !isquoted) {
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
		if ((format&FMT_STRING_ESCAPE_NEWLINE)!=0 && !isquoted) {
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
}

} // namespace CharUtils

} // namespace duct

