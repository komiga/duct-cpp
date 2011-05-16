/**
@file stream.cpp
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

#include <stdlib.h>
#include <string.h>
#include <duct/debug.hpp>
#include <duct/stream.hpp>

namespace duct {

// stream ByteSink for writing a UnicodeString as UTF8

/*class _UTF8ByteSink : public ByteSink {
public:
	_UTF8ByteSink(Stream* stream) {
		__stream=stream;
		__size=0;
	}
	
	void Append(const char* bytes, int32_t n) {
		__stream->write((void*)bytes, (size_t)n);
		__size+=n;
	}
	
	size_t size() {
		return __size;
	}
	
private:
	Stream* __stream;
	size_t __size;
};*/

// class Stream implementation

Stream::Stream() : _conv(NULL) {
}

Stream::~Stream() {
	closeConv();
}

char Stream::readByte() {
	char c;
	read(&c, 1);
	return c;
}

short Stream::readShort() {
	short s;
	read(&s, 2);
	return s;
}

int Stream::readInt() {
	int i;
	read(&i, 4);
	return i;
}

float Stream::readFloat() {
	float f;
	read(&f, 4);
	return f;
}

UChar32 Stream::readChar() {
	ucnv_resetToUnicode(_conv);
	char in[4];
	char* inp;
	UChar out[]={U_SENTINEL, U_SENTINEL};
	UChar* outp=out;
	int pending=1; // assume one UChar by default
	int size=ucnv_getMinCharSize(_conv);
	UErrorCode err=U_ZERO_ERROR;
	while (pending>0) {
		if (eof()) {
			debug_assertp(false, this, "Failed to read character: eof reached");
		}
		inp=in; // reset inp
		read(in, size);
		ucnv_toUnicode(_conv, &outp, out+2, (const char**)&inp, (const char*)(in+size), NULL, false, &err);
		if (U_FAILURE(err)) {
			printf("Stream::readString ERROR: %s\n", u_errorName(err));
			debug_assertp(false, this, "Failed to convert character sequence");
		}
		err=U_ZERO_ERROR;
		pending=ucnv_toUCountPending(_conv, &err);
		//printf("outp:%p pending:%d size:%d\n", (void*)outp, pending, size);
		size=1; // reset to byte size
	}
	UChar32 c;
	size_t count=outp==(out+2) ? 2 : 1;
	U16_GET(out, 0, 0, count, c);
	return c;
}

size_t Stream::readString(UnicodeString& str, size_t length) {
	str.remove(); // make sure the string is empty
	if (length>0) {
		UChar32 buf[512];
		unsigned long bpos=pos();
		unsigned int count=0;
		for (unsigned int i=0; i<length; ++i) {
			if (count==512) {
				str+=UnicodeString::fromUTF32(buf, count);
				count=0;
			}
			buf[count++]=readChar();
		}
		if (count>0) {
			str+=UnicodeString::fromUTF32(buf, count);
		}
		return (size_t)(pos()-bpos);
	}
	return 0;
}

size_t Stream::readLine(UnicodeString& str) {
	unsigned long bpos=pos();
	str.remove();
	UChar32 c=readChar();
	if (c!='\n') { // if the char is non-linefeed, continue reading
		UChar32 buf[512];
		buf[0]=c;
		size_t count=1;
		while (c!='\n' && !eof()) {
			if (count==512) {
				str+=UnicodeString::fromUTF32(buf, count);
				count=0;
			}
			c=readChar();
			if (c!='\r')
				buf[count++]=c;
		}
		if (count>0) {
			str+=UnicodeString::fromUTF32(buf, count+(c=='\n' ? -1 : 0));
		}
	}
	return (size_t)(pos()-bpos);
}

size_t Stream::readCString(UnicodeString& str, size_t maxlength) {
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
				str+=UnicodeString::fromUTF32(buf, count);
				count=0;
			}
			c=readChar();
			buf[count++]=c;
			tcount++;
		}
		if (count>0) {
			str+=UnicodeString::fromUTF32(buf, count+(c=='\0' ? -1 : 0));
		}
	}
	return (size_t)(pos()-bpos);
}

void Stream::writeByte(char value) {
	write(&value, 1);
}

void Stream::writeShort(short value) {
	write(&value, 2);
}

void Stream::writeInt(int value) {
	write(&value, 4);
}

void Stream::writeFloat(float value) {
	write(&value, 4);
}

size_t Stream::writeChar16(UChar value) {
	UnicodeString tempstr(value);
	char out[8];
	const UChar* in=tempstr.getBuffer();
	UErrorCode err=U_ZERO_ERROR;
	size_t size=ucnv_fromUChars(_conv, out, sizeof(out), in, 1, &err);
	if (U_FAILURE(err)) {
		printf("Stream::writeChar16 ERROR: %s\n", u_errorName(err));
		debug_assertp(false, this, "Failed to convert character");
		return 0;
	}
	debug_assertp(write((void*)out, size)==size, this, "Failed to write buffer");
	return size;
}

size_t Stream::writeChar32(UChar32 value) {
	UnicodeString tempstr(value);
	char out[16];
	const UChar* in=tempstr.getBuffer();
	UErrorCode err=U_ZERO_ERROR;
	size_t size=ucnv_fromUChars(_conv, out, sizeof(out), in, tempstr.length(), &err);
	if (U_FAILURE(err)) {
		printf("Stream::writeChar32 ERROR: %s\n", u_errorName(err));
		debug_assertp(false, this, "Failed to convert character");
	}
	debug_assertp(write((void*)out, size)==size, this, "Failed to write buffer");
	return size;
}

size_t Stream::writeString(const UnicodeString& str) {
	size_t count=0;
	const UChar* in=str.getBuffer();
	if (str.length()>0 && in) {
		ucnv_resetFromUnicode(_conv);
		UErrorCode err=U_ZERO_ERROR;
		char out[512];
		char* outp=out;
		const char* out_limit=out+sizeof(out);
		const UChar* inp=in;
		const UChar* in_limit=in+str.length();
		bool cont_writing=true;
		while (cont_writing) {
			ucnv_fromUnicode(_conv, &outp, out_limit, &inp, in_limit, NULL, false, &err);
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

size_t Stream::writeLine(const UnicodeString& str) {
	size_t size=writeString(str);
	size+=writeChar16('\n');
	return size;
}

size_t Stream::writeCString(const UnicodeString& str) {
	size_t size=writeString(str);
	size+=writeChar16('\0');
	return size;
}

bool Stream::readAndMatchCString(const UnicodeString& checkstr, size_t maxlength) {
	maxlength=(maxlength==0) ? (checkstr.length()+1) : (maxlength);
	UnicodeString rstr;
	readCString(rstr, maxlength);
	return checkstr.compare(rstr)==0;
}

void Stream::readReservedCString(UnicodeString& result, size_t size) {
	size_t readcount=readCString(result, size);
	skip(size-readcount);
}

bool Stream::readAndMatchReservedCString(const UnicodeString& checkstr, size_t size) {
	UnicodeString rstr;
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

void Stream::writeReservedCString(const UnicodeString& str, size_t size, unsigned char padvalue) {
	if ((unsigned int)str.length()<size) { // string is smaller, null and padding needed
		size_t bytes=writeCString(str);
		if (bytes<size) {
			writeReservedData(size-bytes, padvalue);
		}
	} else if ((unsigned int)str.length()==size) { // matching sizes, no null needed
		writeString(str);
	} else { // string is larger than reserved space, truncation and no null needed
		UnicodeString out(str, 0, size);
		writeString(str);
	}
}

unsigned long Stream::skip(long change) {
	return seek(pos()+change);
}

void Stream::setFlags(unsigned int flags) {
	_flags=flags;
}

unsigned int Stream::getFlags() const {
	return _flags;
}

bool Stream::setEncoding(const char* codepage) {
	UErrorCode err=U_ZERO_ERROR;
	UConverter* conv=ucnv_open(codepage, &err);
	if (U_SUCCESS(err)) {
		closeConv();
		_conv=conv;
		return true;
	}
	return false;
}

bool Stream::setEncoding(const UnicodeString& encoding) {
	UErrorCode err=U_ZERO_ERROR;
	UnicodeString temp(encoding);
	UConverter* conv=ucnv_openU(temp.getTerminatedBuffer(), &err);
	if (U_SUCCESS(err)) {
		closeConv();
		_conv=conv;
		return true;
	}
	return false;
}

const char* Stream::getEncoding() const {
	if (_conv) {
		UErrorCode err=U_ZERO_ERROR;
		return ucnv_getName(_conv, &err);
	}
	return NULL;
}

UConverter* Stream::getConv() {
	return _conv;
}

const UConverter* Stream::getConv() const {
	return _conv;
}

void Stream::closeConv() {
	if (_conv!=NULL) {
		ucnv_close(_conv);
		_conv=NULL;
	}
}

} // namespace duct

