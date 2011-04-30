/**
@file endianstream.cpp
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
#include <duct/endianstream.hpp>

namespace duct {

EndianStream::EndianStream(Stream* stream, bool autoclose, int order) : _order(order) {
	_stream=stream;
	_autoclose=autoclose;
}

short EndianStream::readShort() {
	debug_assertp(_stream!=NULL, this, "Wrapped stream not set");
	short s=_stream->readShort();
	if (_order!=DUCT_BYTEORDER) {
		s=bswap_16(s);
	}
	return s;
}

int EndianStream::readInt() {
	debug_assertp(_stream!=NULL, this, "Wrapped stream not set");
	int i=_stream->readInt();
	if (_order!=DUCT_BYTEORDER) {
		i=bswap_32(i);
	}
	return i;
}

float EndianStream::readFloat() {
	debug_assertp(_stream!=NULL, this, "Wrapped stream not set");
	float f=_stream->readFloat();
	if (_order!=DUCT_BYTEORDER) {
		f=(float)bswap_32((unsigned int)f);
	}
	return f;
}

void EndianStream::writeShort(short value) {
	debug_assertp(_stream!=NULL, this, "Wrapped stream not set");
	if (_order!=DUCT_BYTEORDER) {
		value=bswap_16(value);
	}
	_stream->writeShort(value);
}

void EndianStream::writeInt(int value) {
	debug_assertp(_stream!=NULL, this, "Wrapped stream not set");
	if (_order!=DUCT_BYTEORDER) {
		value=bswap_32(value);
	}
	_stream->writeInt(value);
}

void EndianStream::writeFloat(float value) {
	debug_assertp(_stream!=NULL, this, "Wrapped stream not set");
	if (_order!=DUCT_BYTEORDER) {
		value=(float)bswap_32((unsigned int)value);
	}
	_stream->writeFloat(value);
}

/*Stream* EndianStream::littleEndian(Stream* stream, bool autoclose) {
	if (isSystemBigEndian()) {
		debug_print("EndianStream::littleEndian Platform is big-endian, returning new EndianStream");
		return new EndianStream(stream, autoclose, true);
	} else {
		debug_print("EndianStream::littleEndian Platform is little-endian, returning no-swap EndianStream");
		//return stream;
		return new EndianStream(stream, autoclose, false);
	}
}

Stream* EndianStream::bigEndian(Stream* stream, bool autoclose) {
	if (isSystemBigEndian()) {
		debug_print("EndianStream::bigEndian Platform is big-endian, returning no-swap EndianStream");
		return new EndianStream(stream, autoclose, false);
	} else {
		debug_print("EndianStream::bigEndian Platform is little-endian, returning new EndianStream");
		return new EndianStream(stream, autoclose, true);
	}
}*/

} // namespace duct

