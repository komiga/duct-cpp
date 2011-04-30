/**
@file unistrcompare.hpp
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

duct++ UnicodeString* comparison class.
*/

#ifndef _DUCT_UNISTRCOMPARE_HPP
#define _DUCT_UNISTRCOMPARE_HPP

#include <unicode/unistr.h>
#include <duct/config.hpp>

namespace duct {

/**
	UnicodeString* comparison class.
*/
class DUCT_API UnicodeStringPCompare {
public:
	bool operator()(const UnicodeString* x, const UnicodeString* y) const {
		return x->compare(*y)<0;
	};
};

} // namespace duct

#endif // _DUCT_UNISTRCOMPARE_HPP

