/**
@file streamwrapper.cpp
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
#include <duct/streamwrapper.hpp>

namespace duct {

// class StreamWrapper implementation

StreamWrapper::StreamWrapper() : _stream(NULL), _autoclose(false) {
}

StreamWrapper::~StreamWrapper() {
	//debug_calledp(this);
	if (_autoclose && _stream) {
		close();
	}
}

void StreamWrapper::setStream(Stream* stream) {
	if (_autoclose && _stream) {
		close();
	}
	_stream=stream;
}

Stream* StreamWrapper::getStream() const {
	return _stream;
}

void StreamWrapper::setAutoClose(bool autoclose) {
	_autoclose=autoclose;
}

bool StreamWrapper::getAutoClose() const {
	return _autoclose;
}

// Base functions passing down to the wrapped stream
size_t StreamWrapper::read(void* data, size_t size) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->read(data, size);
}

size_t StreamWrapper::write(const void* data, size_t size) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->write(data, size);
}

int8_t StreamWrapper::readInt8() {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readInt8();
}

uint8_t StreamWrapper::readUInt8() {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readUInt8();
}

int16_t StreamWrapper::readInt16() {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readInt16();
}

uint16_t StreamWrapper::readUInt16() {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readUInt16();
}

int32_t StreamWrapper::readInt32() {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readInt32();
}

uint32_t StreamWrapper::readUInt32() {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readUInt32();
}

int64_t StreamWrapper::readInt64() {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readInt64();
}

uint64_t StreamWrapper::readUInt64() {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readUInt64();
}

float StreamWrapper::readFloat() {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readFloat();
}

double StreamWrapper::readDouble() {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readDouble();
}

UChar32 StreamWrapper::readChar() {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readChar();
}

size_t StreamWrapper::readString(icu::UnicodeString& str, size_t length) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readString(str, length);
}

size_t StreamWrapper::readLine(icu::UnicodeString& str) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readLine(str);
}

size_t StreamWrapper::readCString(icu::UnicodeString& str, size_t maxlength) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readCString(str, maxlength);
}

size_t StreamWrapper::writeInt8(int8_t value) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->writeInt8(value);
}

size_t StreamWrapper::writeUInt8(uint8_t value) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->writeUInt8(value);
}

size_t StreamWrapper::writeInt16(int16_t value) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->writeInt16(value);
}

size_t StreamWrapper::writeUInt16(uint16_t value) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->writeUInt16(value);
}

size_t StreamWrapper::writeInt32(int32_t value) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->writeInt32(value);
}

size_t StreamWrapper::writeUInt32(uint32_t value) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->writeUInt32(value);
}

size_t StreamWrapper::writeInt64(int64_t value) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->writeInt64(value);
}

size_t StreamWrapper::writeUInt64(uint64_t value) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->writeUInt64(value);
}

size_t StreamWrapper::writeFloat(float value) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->writeFloat(value);
}

size_t StreamWrapper::writeDouble(double value) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->writeDouble(value);
}

size_t StreamWrapper::writeChar16(UChar value) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->writeChar16(value);
}

size_t StreamWrapper::writeChar32(UChar32 value) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->writeChar32(value);
}

size_t StreamWrapper::writeString(const icu::UnicodeString& str) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->writeString(str);
}

size_t StreamWrapper::writeLine(const icu::UnicodeString& str) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->writeLine(str);
}

size_t StreamWrapper::writeCString(const icu::UnicodeString& str) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->writeCString(str);
}

void StreamWrapper::flush() {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	_stream->flush();
}

bool StreamWrapper::eof() const {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->eof();
}

size_t StreamWrapper::size() const {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->size();
}

unsigned long StreamWrapper::pos() const {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->pos();
}

unsigned long StreamWrapper::seek(unsigned long pos) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->seek(pos);
}

unsigned long StreamWrapper::skip(long change) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->skip(change);
}

void StreamWrapper::close() {
	if (_stream) {
		_stream->close();
		_stream=NULL;
	}
}

void StreamWrapper::setFlags(unsigned int flags) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	_stream->setFlags(flags);
}

unsigned int StreamWrapper::getFlags() const {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->getFlags();
}

bool StreamWrapper::setEncoding(const char* encoding) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->setEncoding(encoding);
}

bool StreamWrapper::setEncoding(const icu::UnicodeString& encoding) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->setEncoding(encoding);
}

const char* StreamWrapper::getEncoding() const {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->getEncoding();
}

UConverter* StreamWrapper::getConv() {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->getConv();
}

void StreamWrapper::closeConv() {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	_stream->closeConv();
}

} // namespace duct

