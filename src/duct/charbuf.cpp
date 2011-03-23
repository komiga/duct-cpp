/**
@file charbuf.cpp
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
*/

#include <duct/debug.hpp>
#include <duct/charbuf.hpp>
#include <math.h>
#include <stdlib.h>

namespace duct {

// class CharBuf implementation

CharBuf::CharBuf() : _buffer(NULL), _bufsize(0), _buflength(0), _cached(false) {
}

CharBuf::~CharBuf() {
	if (_buffer) {
		free(_buffer);
	}
}

void CharBuf::addChar(UChar32 c) {
	const size_t BUFFER_INITIAL_SIZE=68;
	const double BUFFER_MULTIPLIER=1.75;
	if (!_buffer) {
		_bufsize=BUFFER_INITIAL_SIZE;
		_buffer=(UChar32*)malloc(_bufsize*4);
		debug_assertp(_buffer, this, "Unable to allocate buffer");
		_buflength=0;
	} else if (_buflength>=_bufsize) {
		size_t newsize=ceil(_bufsize*BUFFER_MULTIPLIER);
		if (newsize<_buflength) {
			newsize=ceil(_buflength*BUFFER_MULTIPLIER);
		}
		_bufsize=newsize;
		void* temp=realloc(_buffer, newsize*4);
		debug_assertp(temp, this, "Unable to allocate buffer");
		_buffer=(UChar32*)temp;
	}
	_buffer[_buflength++]=c;
	_cached=false;
}

const UnicodeString& CharBuf::cacheString() {
	if (!_cached) {
		if (_buffer && _buflength>0) {
			_bufstring=UnicodeString::fromUTF32(_buffer, _buflength);
		} else {
			_bufstring.remove();
		}
		_cached=true;
	}
	return _bufstring;
}

void CharBuf::reset() {
	_buflength=0;
	_cached=false;
}

void CharBuf::asString(UnicodeString& str) {
	str.setTo(cacheString());
}

const UnicodeString& CharBuf::asString() {
	return cacheString();
}

} // namespace duct

