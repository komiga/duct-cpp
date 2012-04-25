/**
@file StreamWrapper.cpp
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
#include <duct/StreamWrapper.hpp>

namespace duct {

// class StreamWrapper implementation

StreamWrapper::StreamWrapper()
	: m_stream(NULL), m_autoclose(false)
{}

StreamWrapper::~StreamWrapper() {
	//debug_calledp(this);
	if (m_autoclose && m_stream) {
		close();
	}
}

void StreamWrapper::setStream(Stream* stream) {
	if (m_autoclose && m_stream) {
		close();
	}
	m_stream=stream;
}

Stream* StreamWrapper::getStream() const {
	return m_stream;
}

void StreamWrapper::setAutoClose(bool autoclose) {
	m_autoclose=autoclose;
}

bool StreamWrapper::getAutoClose() const {
	return m_autoclose;
}

// Base functions passing down to the wrapped stream
size_t StreamWrapper::read(void* data, size_t size) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->read(data, size);
}

size_t StreamWrapper::write(void const* data, size_t size) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->write(data, size);
}

int8_t StreamWrapper::readInt8() {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->readInt8();
}

uint8_t StreamWrapper::readUInt8() {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->readUInt8();
}

int16_t StreamWrapper::readInt16() {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->readInt16();
}

uint16_t StreamWrapper::readUInt16() {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->readUInt16();
}

int32_t StreamWrapper::readInt32() {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->readInt32();
}

uint32_t StreamWrapper::readUInt32() {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->readUInt32();
}

int64_t StreamWrapper::readInt64() {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->readInt64();
}

uint64_t StreamWrapper::readUInt64() {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->readUInt64();
}

float StreamWrapper::readFloat() {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->readFloat();
}

double StreamWrapper::readDouble() {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->readDouble();
}

UChar32 StreamWrapper::readChar() {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->readChar();
}

size_t StreamWrapper::readString(icu::UnicodeString& str, size_t length) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->readString(str, length);
}

size_t StreamWrapper::readLine(icu::UnicodeString& str) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->readLine(str);
}

size_t StreamWrapper::readCString(icu::UnicodeString& str, size_t maxlength) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->readCString(str, maxlength);
}

size_t StreamWrapper::writeInt8(int8_t value) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->writeInt8(value);
}

size_t StreamWrapper::writeUInt8(uint8_t value) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->writeUInt8(value);
}

size_t StreamWrapper::writeInt16(int16_t value) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->writeInt16(value);
}

size_t StreamWrapper::writeUInt16(uint16_t value) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->writeUInt16(value);
}

size_t StreamWrapper::writeInt32(int32_t value) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->writeInt32(value);
}

size_t StreamWrapper::writeUInt32(uint32_t value) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->writeUInt32(value);
}

size_t StreamWrapper::writeInt64(int64_t value) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->writeInt64(value);
}

size_t StreamWrapper::writeUInt64(uint64_t value) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->writeUInt64(value);
}

size_t StreamWrapper::writeFloat(float value) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->writeFloat(value);
}

size_t StreamWrapper::writeDouble(double value) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->writeDouble(value);
}

size_t StreamWrapper::writeChar16(UChar value) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->writeChar16(value);
}

size_t StreamWrapper::writeChar32(UChar32 value) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->writeChar32(value);
}

size_t StreamWrapper::writeString(icu::UnicodeString const& str) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->writeString(str);
}

size_t StreamWrapper::writeLine(icu::UnicodeString const& str) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->writeLine(str);
}

size_t StreamWrapper::writeCString(icu::UnicodeString const& str) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->writeCString(str);
}

void StreamWrapper::flush() {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	m_stream->flush();
}

bool StreamWrapper::eof() const {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->eof();
}

size_t StreamWrapper::size() const {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->size();
}

unsigned long StreamWrapper::pos() const {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->pos();
}

unsigned long StreamWrapper::seek(unsigned long pos) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->seek(pos);
}

unsigned long StreamWrapper::skip(long change) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->skip(change);
}

void StreamWrapper::close() {
	if (m_stream) {
		m_stream->close();
		m_stream=NULL;
	}
}

void StreamWrapper::setFlags(unsigned int flags) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	m_stream->setFlags(flags);
}

unsigned int StreamWrapper::getFlags() const {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->getFlags();
}

bool StreamWrapper::setEncoding(char const* encoding) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->setEncoding(encoding);
}

bool StreamWrapper::setEncoding(icu::UnicodeString const& encoding) {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->setEncoding(encoding);
}

char const* StreamWrapper::getEncoding() const {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->getEncoding();
}

UConverter* StreamWrapper::getConv() {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	return m_stream->getConv();
}

void StreamWrapper::closeConv() {
	debug_assertp(m_stream, this, "Wrapped stream cannot be NULL");
	m_stream->closeConv();
}

} // namespace duct
