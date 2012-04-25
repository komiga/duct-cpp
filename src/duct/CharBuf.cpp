/**
@file CharBuf.cpp
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
#include <duct/CharBuf.hpp>

#include <math.h>
#include <stdlib.h>
#include <unicode/numfmt.h>
#include <unicode/ustring.h>

namespace duct {

// class CharBuf implementation

CharBuf::CharBuf()
	: m_buffer(NULL), m_bufsize(0), m_buflength(0), m_bufstring(), m_cached(false)
{/* Do nothing */}

CharBuf::~CharBuf() {
	if (m_buffer) {
		free(m_buffer);
		m_buffer=NULL;
	}
}

void CharBuf::addChar(UChar32 c) {
	const size_t BUFFER_INITIAL_SIZE=68;
	const double BUFFER_MULTIPLIER=1.75;
	if (!m_buffer) {
		m_bufsize=BUFFER_INITIAL_SIZE;
		m_buffer=(UChar32*)malloc(m_bufsize*4);
		debug_assertp(m_buffer, this, "Unable to allocate buffer");
		m_buflength=0;
	} else if (m_buflength>=m_bufsize) {
		size_t newsize=(size_t)ceil(m_bufsize*BUFFER_MULTIPLIER);
		if (newsize<m_buflength) {
			newsize=(size_t)ceil(m_buflength*BUFFER_MULTIPLIER);
		}
		m_bufsize=newsize;
		void* temp=realloc(m_buffer, newsize*4);
		debug_assertp(temp, this, "Unable to allocate buffer");
		m_buffer=(UChar32*)temp;
	}
	m_buffer[m_buflength++]=c;
	m_cached=false;
}

icu::UnicodeString const& CharBuf::cacheString() {
	if (!m_cached) {
		if (m_buffer && m_buflength>0) {
			//m_bufstring=icu::UnicodeString::fromUTF32(m_buffer, m_buflength);
			int32_t capacity;
			if (m_buflength<=16) { // assumed upper size for small-sized buffers
				capacity=16;
			} else {
				capacity=m_buflength+(m_buflength>>4)+4;
			}
			UChar* strbuf;
			int32_t newsize;
			UErrorCode err;
			do {
				strbuf=m_bufstring.getBuffer(capacity);
				err=U_ZERO_ERROR;
				u_strFromUTF32WithSub(strbuf, m_bufstring.getCapacity(), &newsize, m_buffer, m_buflength, 0xFFFD, NULL, &err);
				m_bufstring.releaseBuffer(newsize);
				if (err==U_BUFFER_OVERFLOW_ERROR) {
					capacity=newsize+1;  // for the terminating \n
					continue; // repeat for the last char
				} else if (U_FAILURE(err)) {
					m_bufstring.setToBogus();
					debug_printp_source(this, "Failed to convert buffer to string; err:");
					debug_print(u_errorName(err));
				}
				break;
			} while (1);
		} else {
			m_bufstring.remove();
		}
		m_cached=true;
	}
	return m_bufstring;
}

void CharBuf::reset() {
	m_buflength=0;
	m_cached=false;
}

bool CharBuf::compare(UChar32 c) const {
	for (unsigned int i=0; i<m_buflength; ++i) {
		if (m_buffer[i]!=c)
			return false;
	}
	return true;
}

bool CharBuf::compare(const CharacterSet& charset) const {
	for (unsigned int i=0; i<m_buflength; ++i) {
		if (!charset.contains(m_buffer[i]))
			return false;
	}
	return true;
}

bool CharBuf::toString(icu::UnicodeString& str) {
	cacheString();
	if (!m_bufstring.isBogus()) {
		str.setTo(m_bufstring);
		return true;
	}
	return false;
}

icu::UnicodeString const& CharBuf::toString() {
	return cacheString();
}

int32_t CharBuf::toInt() {
	int32_t value=0;
	toInt(value);
	return value;
}

bool CharBuf::toInt(int32_t& value) {
	if (!m_cached) {
		cacheString();
	}
	UErrorCode status=U_ZERO_ERROR;
	icu::NumberFormat* nf=icu::NumberFormat::createInstance(status);
	icu::Formattable formattable;
	nf->parse(m_bufstring, formattable, status);
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
	if (!m_cached) {
		cacheString();
	}
	UErrorCode status=U_ZERO_ERROR;
	icu::NumberFormat* nf=icu::NumberFormat::createInstance(status);
	icu::Formattable formattable;
	nf->parse(m_bufstring, formattable, status);
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
	if (!m_cached) {
		cacheString();
	}
	UErrorCode status=U_ZERO_ERROR;
	icu::NumberFormat* nf=icu::NumberFormat::createInstance(status);
	icu::Formattable formattable;
	nf->parse(m_bufstring, formattable, status);
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
	if (!m_cached) {
		cacheString();
	}
	UErrorCode status=U_ZERO_ERROR;
	icu::NumberFormat* nf=icu::NumberFormat::createInstance(status);
	icu::Formattable formattable;
	nf->parse(m_bufstring, formattable, status);
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
