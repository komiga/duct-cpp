/**
@file unistrarray.cpp
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
#include <duct/unistrarray.hpp>

namespace duct {

// class StringArray implementation

StringArray::StringArray() {
}

StringArray::StringArray(const char* str) {
	set(str);
}

StringArray::StringArray(const char** data, int size) {
	set(data, size);
}

StringArray::StringArray(size_t num, ...) {
	va_list ap;
	va_start(ap, num);
	setVL(num, ap);
	va_end(ap);
}

StringArray::~StringArray() {
}

void StringArray::set(const char* str) {
	release();
	_data=new icu::UnicodeString*[_size=1];
	_data[0]=new icu::UnicodeString(str);
	_static=false;
}

void StringArray::set(const char** data, int size) {
	release();
	int i=0;
	if (size<0) {
		while (data[i]!=NULL) {
			++i;
		}
		size=i;
	}
	_size=size;
	_data=new icu::UnicodeString*[size];
	for (i=0; i<size; ++i) {
		_data[i]=new icu::UnicodeString(data[i]);
	}
	_static=false;
}

void StringArray::setVLCStrings(unsigned int num, va_list ap) {
	release();
	_size=num;
	_data=new icu::UnicodeString*[_size];
	for (unsigned int i=0; i<num; ++i) {
		_data[i]=new icu::UnicodeString(va_arg(ap, const char*));
	}
	_static=false;
}

void StringArray::setVCStrings(unsigned int num, ...) {
	va_list ap;
	va_start(ap, num);
	setVLCStrings(num, ap);
	va_end(ap);
}

StringArray* StringArray::withCStrings(unsigned int num, ...) {
	StringArray* sa=new StringArray();
	va_list ap;
	va_start(ap, num);
	sa->setVLCStrings(num, ap);
	va_end(ap);
	return sa;
}

} // namespace duct

