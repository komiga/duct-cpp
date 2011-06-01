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

StreamWrapper::StreamWrapper() : _stream(NULL) {
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

char StreamWrapper::readByte() {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readByte();
}

short StreamWrapper::readShort() {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readShort();
}

int StreamWrapper::readInt() {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readInt();
}

long StreamWrapper::readLong() {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readLong();
}

float StreamWrapper::readFloat() {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readFloat();
}

UChar32 StreamWrapper::readChar() {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readChar();
}

size_t StreamWrapper::readString(UnicodeString& str, size_t length) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readString(str, length);
}

size_t StreamWrapper::readLine(UnicodeString& str) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readLine(str);
}

size_t StreamWrapper::readCString(UnicodeString& str, size_t maxlength) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->readCString(str, maxlength);
}

void StreamWrapper::writeByte(char value) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	_stream->writeByte(value);
}

void StreamWrapper::writeShort(short value) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	_stream->writeShort(value);
}

void StreamWrapper::writeInt(int value) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	_stream->writeInt(value);
}

void StreamWrapper::writeLong(long value) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	_stream->writeLong(value);
}

void StreamWrapper::writeFloat(float value) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	_stream->writeFloat(value);
}

size_t StreamWrapper::writeChar16(UChar value) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->writeChar16(value);
}

size_t StreamWrapper::writeChar32(UChar32 value) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->writeChar32(value);
}

size_t StreamWrapper::writeString(const UnicodeString& str) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->writeString(str);
}

size_t StreamWrapper::writeLine(const UnicodeString& str) {
	debug_assertp(_stream, this, "Wrapped stream cannot be NULL");
	return _stream->writeLine(str);
}

size_t StreamWrapper::writeCString(const UnicodeString& str) {
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

bool StreamWrapper::setEncoding(const UnicodeString& encoding) {
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

