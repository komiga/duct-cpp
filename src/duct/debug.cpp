/**
@file debug.cpp
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

namespace duct {

#ifndef NDEBUG

void debug_assert__(char const* assertion__, char const* file__, int line__, char const* function__, char const* mesg__) {
	fprintf(stderr, "assertion failure: %s\n", mesg__);
	fprintf(stderr, " in %s:%d: %s: Assertion: `%s`\n", file__, line__, function__, assertion__);
	abort();
}

void debug_assertp__(char const* assertion__, char const* file__, int line__, char const* function__, void const* p__, char const* mesg__) {
	fprintf(stderr, "assertion failure: [%p] %s\n", p__, mesg__);
	fprintf(stderr, " in %s:%d: %s: Assertion: `%s`\n", file__, line__, function__, assertion__);
	abort();
}

#else

void debug_assert__(char const* assertion__, char const* file__, int line__, char const* function__, char const* mesg__) {
	(void)assertion__;
	(void)function__;
	(void)file__;
	(void)line__;
	(void)function__;
	(void)mesg__;
}

void debug_assertp__(char const* assertion__, char const* file__, int line__, char const* function__, void const* p__, char const* mesg__) {
	(void)assertion__;
	(void)function__;
	(void)file__;
	(void)line__;
	(void)function__;
	(void)p__;
	(void)mesg__;
}

#endif // if !NDEBUG

} // namespace duct
