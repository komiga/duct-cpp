/**
@file stream.cpp
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
#include <duct/stream.hpp>

#include <stdlib.h>
#include <string.h>

namespace duct {

// class Stream implementation

Stream::Stream()
	: m_conv(NULL)
{}

Stream::~Stream() {
	closeConv();
}

int8_t Stream::readInt8() {
	int8_t v;
	read(v);
	return v;
}

uint8_t Stream::readUInt8() {
	uint8_t v;
	read(v);
	return v;
}

int16_t Stream::readInt16() {
	int16_t v;
	read(v);
	return v;
}

uint16_t Stream::readUInt16() {
	uint16_t v;
	read(v);
	return v;
}

int32_t Stream::readInt32() {
	int32_t v;
	read(v);
	return v;
}

uint32_t Stream::readUInt32() {
	uint32_t v;
	read(v);
	return v;
}

int64_t Stream::readInt64() {
	int64_t v;
	read(v);
	return v;
}

uint64_t Stream::readUInt64() {
	uint64_t v;
	read(v);
	return v;
}

float Stream::readFloat() {
	float v;
	read(v);
	return v;
}

double Stream::readDouble() {
	double v;
	read(v);
	return v;
}

UChar32 Stream::readChar() {
	ucnv_resetToUnicode(m_conv);
	char in[4];
	char* inp;
	UChar out[]={(UChar)U_SENTINEL, (UChar)U_SENTINEL};
	UChar* outp=out;
	int pending=1; // assume one UChar by default
	int size=ucnv_getMinCharSize(m_conv);
	UErrorCode err=U_ZERO_ERROR;
	while (pending>0) {
		if (eof()) {
			debug_assertp(false, this, "Failed to read character: eof reached");
		}
		inp=in; // reset inp
		read(in, size);
		ucnv_toUnicode(m_conv, &outp, out+2, (char const**)&inp, (char const*)(in+size), NULL, false, &err);
		if (U_FAILURE(err)) {
			printf("Stream::readString ERROR: %s\n", u_errorName(err));
			debug_assertp(false, this, "Failed to convert character sequence");
		}
		err=U_ZERO_ERROR;
		pending=ucnv_toUCountPending(m_conv, &err);
		//printf("outp:%p pending:%d size:%d\n", (void*)outp, pending, size);
		size=1; // reset to byte size
	}
	UChar32 c;
	size_t count=outp==(out+2) ? 2 : 1;
	U16_GET(out, 0, 0, count, c);
	return c;
}

size_t Stream::readString(icu::UnicodeString& str, size_t length) {
	str.remove(); // make sure the string is empty
	if (length>0) {
		UChar32 buf[512];
		unsigned long bpos=pos();
		unsigned int count=0;
		for (unsigned int i=0; i<length; ++i) {
			if (count==512) {
				str+=icu::UnicodeString::fromUTF32(buf, count);
				count=0;
			}
			buf[count++]=readChar();
		}
		if (count>0) {
			str+=icu::UnicodeString::fromUTF32(buf, count);
		}
		return (size_t)(pos()-bpos);
	}
	return 0;
}

size_t Stream::readLine(icu::UnicodeString& str) {
	unsigned long bpos=pos();
	str.remove();
	UChar32 c=readChar();
	if (c!='\n') { // if the char is non-linefeed, continue reading
		UChar32 buf[512];
		buf[0]=c;
		size_t count=1;
		while (c!='\n' && !eof()) {
			if (count==512) {
				str+=icu::UnicodeString::fromUTF32(buf, count);
				count=0;
			}
			c=readChar();
			if (c!='\r')
				buf[count++]=c;
		}
		if (count>0 && !(count==1 && c=='\n')) {
			str+=icu::UnicodeString::fromUTF32(buf, count+(c=='\n' ? -1 : 0));
		}
	}
	return (size_t)(pos()-bpos);
}

size_t Stream::readCString(icu::UnicodeString& str, size_t maxlength) {
	unsigned long bpos=pos();
	str.remove();
	UChar32 c=readChar();
	if (c!='\0') { // if the char is non-null, continue reading
		UChar32 buf[512];
		buf[0]=c;
		size_t count=1;
		size_t tcount=1;
		while (c!='\0' && tcount<maxlength && !eof()) {
			if (count==512) {
				str+=icu::UnicodeString::fromUTF32(buf, count);
				count=0;
			}
			c=readChar();
			buf[count++]=c;
			tcount++;
		}
		if (count>0 && !(count==1 && c=='\0')) {
			str+=icu::UnicodeString::fromUTF32(buf, count+(c=='\0' ? -1 : 0));
		}
	}
	return (size_t)(pos()-bpos);
}

size_t Stream::writeInt8(int8_t value) {
	return write(value);
}

size_t Stream::writeUInt8(uint8_t value) {
	return write(value);
}

size_t Stream::writeInt16(int16_t value) {
	return write(value);
}

size_t Stream::writeUInt16(uint16_t value) {
	return write(value);
}

size_t Stream::writeInt32(int32_t value) {
	return write(value);
}

size_t Stream::writeUInt32(uint32_t value) {
	return write(value);
}

size_t Stream::writeInt64(int64_t value) {
	return write(value);
}

size_t Stream::writeUInt64(uint64_t value) {
	return write(value);
}

size_t Stream::writeFloat(float value) {
	return write(value);
}

size_t Stream::writeDouble(double value) {
	return write(value);
}

size_t Stream::writeChar16(UChar value) {
	icu::UnicodeString tempstr(value);
	char out[8];
	UChar const* in=tempstr.getBuffer();
	UErrorCode err=U_ZERO_ERROR;
	size_t size=ucnv_fromUChars(m_conv, out, sizeof(out), in, 1, &err);
	if (U_FAILURE(err)) {
		printf("Stream::writeChar16 ERROR: %s\n", u_errorName(err));
		debug_assertp(false, this, "Failed to convert character");
		return 0;
	}
	debug_assertp(write((void*)out, size)==size, this, "Failed to write buffer");
	return size;
}

size_t Stream::writeChar32(UChar32 value) {
	icu::UnicodeString tempstr(value);
	char out[16];
	UChar const* in=tempstr.getBuffer();
	UErrorCode err=U_ZERO_ERROR;
	size_t size=ucnv_fromUChars(m_conv, out, sizeof(out), in, tempstr.length(), &err);
	if (U_FAILURE(err)) {
		printf("Stream::writeChar32 ERROR: %s\n", u_errorName(err));
		debug_assertp(false, this, "Failed to convert character");
	}
	debug_assertp(write((void*)out, size)==size, this, "Failed to write buffer");
	return size;
}

size_t Stream::writeString(icu::UnicodeString const& str) {
	size_t count=0;
	UChar const* in=str.getBuffer();
	if (str.length()>0 && in) {
		ucnv_resetFromUnicode(m_conv);
		UErrorCode err=U_ZERO_ERROR;
		char out[512];
		char* outp=out;
		char const* out_limit=out+sizeof(out);
		UChar const* inp=in;
		UChar const* in_limit=in+str.length();
		bool cont_writing=true;
		while (cont_writing) {
			ucnv_fromUnicode(m_conv, &outp, out_limit, &inp, in_limit, NULL, false, &err);
			if (U_SUCCESS(err)) {
				// target filled entirely with source
				size_t size=(outp-out);
				debug_assertp(write((void*)out, size)==size, this, "Failed to write remaining buffer to stream");
				count+=size;
				cont_writing=false;
			} else if (err==U_BUFFER_OVERFLOW_ERROR) {
				// write all the bytes in the buffer and reset the output pointer
				debug_assertp(write((void*)out, sizeof(out))==sizeof(out), this, "Failed to write 512-byte buffer to stream");
				count+=sizeof(out);
				outp=out;
				err=U_ZERO_ERROR;
			} else {
				printf("Stream::writeString ERROR: %s\n", u_errorName(err));
				debug_assertp(false, this, "Failed to convert string");
			}
		}
	}
	return count;
}

size_t Stream::writeLine(icu::UnicodeString const& str) {
	size_t size=writeString(str);
	size+=writeChar16('\n');
	return size;
}

size_t Stream::writeCString(icu::UnicodeString const& str) {
	size_t size=writeString(str);
	size+=writeChar16('\0');
	return size;
}

bool Stream::readAndMatchCString(icu::UnicodeString const& checkstr, size_t maxlength) {
	maxlength=(maxlength==0) ? (checkstr.length()+1) : (maxlength);
	icu::UnicodeString rstr;
	readCString(rstr, maxlength);
	return checkstr.compare(rstr)==0;
}

void Stream::readReservedCString(icu::UnicodeString& result, size_t size) {
	size_t readcount=readCString(result, size);
	skip(size-readcount);
}

bool Stream::readAndMatchReservedCString(icu::UnicodeString const& checkstr, size_t size) {
	icu::UnicodeString rstr;
	size_t readcount=readCString(rstr, size);
	skip(size-readcount);
	return checkstr.compare(rstr)==0;
}

void Stream::writeReservedData(size_t size, unsigned char padvalue) {
	void* data=malloc(size);
	debug_assertp(data!=NULL, this, "Failed to allocate buffer");
	memset(data, padvalue, size);
	write(data, size);
	free(data);
}

void Stream::writeReservedCString(icu::UnicodeString const& str, size_t size, unsigned char padvalue) {
	if ((unsigned int)str.length()<size) { // string is smaller, null and padding needed
		size_t bytes=writeCString(str);
		if (bytes<size) {
			writeReservedData(size-bytes, padvalue);
		}
	} else if ((unsigned int)str.length()==size) { // matching sizes, no null needed
		writeString(str);
	} else { // string is larger than reserved space, truncation and no null needed
		icu::UnicodeString out(str, 0, size);
		writeString(str);
	}
}

unsigned long Stream::skip(long change) {
	return seek(pos()+change);
}

void Stream::setFlags(unsigned int flags) {
	m_flags=flags;
}

unsigned int Stream::getFlags() const {
	return m_flags;
}

bool Stream::setEncoding(char const* codepage) {
	UErrorCode err=U_ZERO_ERROR;
	UConverter* conv=ucnv_open(codepage, &err);
	if (U_SUCCESS(err)) {
		closeConv();
		m_conv=conv;
		return true;
	}
	return false;
}

bool Stream::setEncoding(icu::UnicodeString const& encoding) {
	UErrorCode err=U_ZERO_ERROR;
	icu::UnicodeString temp(encoding);
	UConverter* conv=ucnv_openU(temp.getTerminatedBuffer(), &err);
	if (U_SUCCESS(err)) {
		closeConv();
		m_conv=conv;
		return true;
	}
	return false;
}

char const* Stream::getEncoding() const {
	if (m_conv) {
		UErrorCode err=U_ZERO_ERROR;
		return ucnv_getName(m_conv, &err);
	}
	return NULL;
}

UConverter* Stream::getConv() {
	return m_conv;
}

UConverter const* Stream::getConv() const {
	return m_conv;
}

void Stream::closeConv() {
	if (m_conv!=NULL) {
		ucnv_close(m_conv);
		m_conv=NULL;
	}
}

} // namespace duct

