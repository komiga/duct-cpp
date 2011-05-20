/**
@file charbuf.cpp
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

#include <math.h>
#include <stdlib.h>
#include <unicode/numfmt.h>
#include <unicode/ustring.h>
#include <duct/debug.hpp>
#include <duct/charbuf.hpp>

namespace duct {

// class CharBuf implementation

CharBuf::CharBuf() : _buffer(NULL), _bufsize(0), _buflength(0), _cached(false) {
}

CharBuf::~CharBuf() {
	if (_buffer) {
		free(_buffer);
		_buffer=NULL;
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
		size_t newsize=(size_t)ceil(_bufsize*BUFFER_MULTIPLIER);
		if (newsize<_buflength) {
			newsize=(size_t)ceil(_buflength*BUFFER_MULTIPLIER);
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
			//_bufstring=UnicodeString::fromUTF32(_buffer, _buflength);
			int32_t capacity;
			if (_buflength<=16) { // assumed upper size for small-sized buffers
				capacity=16;
			} else {
				capacity=_buflength+(_buflength>>4)+4;
			}
			UChar* strbuf;
			int32_t newsize;
			UErrorCode err;
			do {
				strbuf=_bufstring.getBuffer(capacity);
				err=U_ZERO_ERROR;
				u_strFromUTF32WithSub(strbuf, _bufstring.getCapacity(), &newsize, _buffer, _buflength, 0xFFFD, NULL, &err);
				_bufstring.releaseBuffer(newsize);
				if (err==U_BUFFER_OVERFLOW_ERROR) {
					capacity=newsize+1;  // for the terminating \n
					continue; // repeat for the last char
				} else if (U_FAILURE(err)) {
					_bufstring.setToBogus();
					debug_printp_source(this, "Failed to convert buffer to string; err:");
					debug_print(u_errorName(err));
				}
				break;
			} while (1);
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

bool CharBuf::compare(UChar32 c) const {
	for (unsigned int i=0; i<_buflength; ++i) {
		if (_buffer[i]!=c)
			return false;
	}
	return true;
}

bool CharBuf::compare(const CharacterSet& charset) const {
	for (unsigned int i=0; i<_buflength; ++i) {
		if (!charset.contains(_buffer[i]))
			return false;
	}
	return true;
}

bool CharBuf::toString(UnicodeString& str) {
	cacheString();
	if (!_bufstring.isBogus()) {
		str.setTo(_bufstring);
		return true;
	}
	return false;
}

const UnicodeString& CharBuf::toString() {
	return cacheString();
}

int32_t CharBuf::toInt() {
	int32_t value=0;
	toInt(value);
	return value;
}

bool CharBuf::toInt(int32_t& value) {
	if (!_cached) {
		cacheString();
	}
	UErrorCode status=U_ZERO_ERROR;
	NumberFormat* nf=NumberFormat::createInstance(status);
	Formattable formattable;
	nf->parse(_bufstring, formattable, status);
	delete nf;
	if (U_FAILURE(status)) {
		debug_printp_source(this, u_errorName(status));
		return false;
	} else {
		value=formattable.getLong();
		return true;
	}
}

int64_t CharBuf::toLong() {
	int64_t value=0;
	toLong(value);
	return value;
}

bool CharBuf::toLong(int64_t& value) {
	if (!_cached) {
		cacheString();
	}
	UErrorCode status=U_ZERO_ERROR;
	NumberFormat* nf=NumberFormat::createInstance(status);
	Formattable formattable;
	nf->parse(_bufstring, formattable, status);
	delete nf;
	if (U_FAILURE(status)) {
		debug_printp_source(this, u_errorName(status));
		return false;
	} else {
		value=formattable.getInt64();
		return true;
	}
}

float CharBuf::toFloat() {
	float value=0.0;
	toFloat(value);
	return value;
}

bool CharBuf::toFloat(float& value) {
	if (!_cached) {
		cacheString();
	}
	UErrorCode status=U_ZERO_ERROR;
	NumberFormat* nf=NumberFormat::createInstance(status);
	Formattable formattable;
	nf->parse(_bufstring, formattable, status);
	delete nf;
	if (U_FAILURE(status)) {
		debug_printp_source(this, u_errorName(status));
		return false;
	} else {
		value=(float)formattable.getDouble();
		return true;
	}
}

double CharBuf::toDouble() {
	double value=0.0;
	toDouble(value);
	return value;
}

bool CharBuf::toDouble(double& value) {
	if (!_cached) {
		cacheString();
	}
	UErrorCode status=U_ZERO_ERROR;
	NumberFormat* nf=NumberFormat::createInstance(status);
	Formattable formattable;
	nf->parse(_bufstring, formattable, status);
	delete nf;
	if (U_FAILURE(status)) {
		debug_printp_source(this, u_errorName(status));
		return false;
	} else {
		value=formattable.getDouble();
		return true;
	}
}

} // namespace duct

