/**
@file EndianStream.cpp
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
#include <duct/EndianStream.hpp>

#include <algorithm>

namespace duct {

EndianStream::EndianStream(Stream* stream, bool autoclose, int order)
	: m_order(order)
{
	m_stream=stream;
	m_autoclose=autoclose;
}

int16_t EndianStream::readInt16() {
	debug_assertp(m_stream!=NULL, this, "Wrapped stream cannot be NULL");
	int16_t v=m_stream->readInt16();
	if (m_order!=DUCT_BYTEORDER) {
		v=bswap_16(v);
	}
	return v;
}

uint16_t EndianStream::readUInt16() {
	debug_assertp(m_stream!=NULL, this, "Wrapped stream cannot be NULL");
	uint16_t v=m_stream->readUInt16();
	if (m_order!=DUCT_BYTEORDER) {
		v=bswap_16(v);
	}
	return v;
}

int32_t EndianStream::readInt32() {
	debug_assertp(m_stream!=NULL, this, "Wrapped stream cannot be NULL");
	int32_t v=m_stream->readInt32();
	if (m_order!=DUCT_BYTEORDER) {
		v=bswap_32(v);
	}
	return v;
}

uint32_t EndianStream::readUInt32() {
	debug_assertp(m_stream!=NULL, this, "Wrapped stream cannot be NULL");
	uint32_t v=m_stream->readUInt32();
	if (m_order!=DUCT_BYTEORDER) {
		v=bswap_32(v);
	}
	return v;
}

int64_t EndianStream::readInt64() {
	debug_assertp(m_stream!=NULL, this, "Wrapped stream cannot be NULL");
	int64_t v=m_stream->readInt64();
	if (m_order!=DUCT_BYTEORDER) {
		v=bswap_64(v);
	}
	return v;
}

uint64_t EndianStream::readUInt64() {
	debug_assertp(m_stream!=NULL, this, "Wrapped stream cannot be NULL");
	uint64_t v=m_stream->readUInt64();
	if (m_order!=DUCT_BYTEORDER) {
		v=bswap_64(v);
	}
	return v;
}

float EndianStream::readFloat() {
	debug_assertp(m_stream!=NULL, this, "Wrapped stream cannot be NULL");
	float v=m_stream->readFloat();
	if (m_order!=DUCT_BYTEORDER) {
		char& b=reinterpret_cast<char&>(v);
		std::reverse(&b, &b+sizeof(float));
	}
	return v;
}

double EndianStream::readDouble() {
	debug_assertp(m_stream!=NULL, this, "Wrapped stream cannot be NULL");
	double v=m_stream->readDouble();
	if (m_order!=DUCT_BYTEORDER) {
		char& b=reinterpret_cast<char&>(v);
		std::reverse(&b, &b+sizeof(double));
	}
	return v;
}

size_t EndianStream::writeInt16(int16_t value) {
	debug_assertp(m_stream!=NULL, this, "Wrapped stream cannot be NULL");
	if (m_order!=DUCT_BYTEORDER) {
		value=bswap_16(value);
	}
	return m_stream->writeInt16(value);
}

size_t EndianStream::writeUInt16(uint16_t value) {
	debug_assertp(m_stream!=NULL, this, "Wrapped stream cannot be NULL");
	if (m_order!=DUCT_BYTEORDER) {
		value=bswap_16(value);
	}
	return m_stream->writeUInt16(value);
}

size_t EndianStream::writeInt32(int32_t value) {
	debug_assertp(m_stream!=NULL, this, "Wrapped stream cannot be NULL");
	if (m_order!=DUCT_BYTEORDER) {
		value=bswap_32(value);
	}
	return m_stream->writeInt32(value);
}

size_t EndianStream::writeUInt32(uint32_t value) {
	debug_assertp(m_stream!=NULL, this, "Wrapped stream cannot be NULL");
	if (m_order!=DUCT_BYTEORDER) {
		value=bswap_32(value);
	}
	return m_stream->writeUInt32(value);
}

size_t EndianStream::writeInt64(int64_t value) {
	debug_assertp(m_stream!=NULL, this, "Wrapped stream cannot be NULL");
	if (m_order!=DUCT_BYTEORDER) {
		value=bswap_64(value);
	}
	return m_stream->writeInt64(value);
}

size_t EndianStream::writeUInt64(uint64_t value) {
	debug_assertp(m_stream!=NULL, this, "Wrapped stream cannot be NULL");
	if (m_order!=DUCT_BYTEORDER) {
		value=bswap_64(value);
	}
	return m_stream->writeUInt64(value);
}
size_t EndianStream::writeFloat(float value) {
	debug_assertp(m_stream!=NULL, this, "Wrapped stream cannot be NULL");
	if (m_order!=DUCT_BYTEORDER) {
		char& b=reinterpret_cast<char&>(value);
		std::reverse(&b, &b+sizeof(float));
	}
	return m_stream->writeFloat(value);
}

size_t EndianStream::writeDouble(double value) {
	debug_assertp(m_stream!=NULL, this, "Wrapped stream cannot be NULL");
	if (m_order!=DUCT_BYTEORDER) {
		char& b=reinterpret_cast<char&>(value);
		std::reverse(&b, &b+sizeof(double));
	}
	return m_stream->writeDouble(value);
}

} // namespace duct
