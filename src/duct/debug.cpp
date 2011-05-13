/**
@file debug.cpp
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

#include <duct/debug.hpp>

namespace duct {

#ifndef NDEBUG
	
	void __debug_assert(const char* __assertion, const char* __file, int __line, const char* __function, const char* __mesg) {
		fprintf(stderr, "assertion failure: %s\n", __mesg);
		fprintf(stderr, " in %s:%d: %s: Assertion: `%s`\n", __file, __line, __function, __assertion);
		abort();
	}
	
	void __debug_assertp(const char* __assertion, const char* __file, int __line, const char* __function, const void* __p, const char* __mesg) {
		fprintf(stderr, "assertion failure: [%p] %s\n", __p, __mesg);
		fprintf(stderr, " in %s:%d: %s: Assertion: `%s`\n", __file, __line, __function, __assertion);
		abort();
	}
	
#else
	
	void __debug_assert(const char* __assertion, const char* __file, int __line, const char* __function, const char* __mesg) {
	}
	
	void __debug_assertp(const char* __assertion, const char* __file, int __line, const char* __function, const void* __p, const char* __mesg) {
	}
	
#endif // if !NDEBUG

} // namespace duct

