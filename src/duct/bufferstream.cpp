/**
@file bufferstream.cpp
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
#include <duct/bufferstream.hpp>

#include <string.h>

namespace duct {

// class BufferStream implementation

BufferStream::BufferStream(void* buffer, size_t size, unsigned int flags, char const* encoding)
	: _buffer(buffer), _pos(0), _size(size)
{
	setFlags(flags);
	setEncoding(encoding);
}

void BufferStream::setBuffer(void* buffer, size_t size) {
	_buffer=buffer;
	_size=size;
}

void* BufferStream::getBuffer() {
	return _buffer;
}

size_t BufferStream::read(void* data, size_t size) {
	debug_assertp(_buffer!=NULL, this, "Cannot read from closed stream");
	debug_assertp(_flags&STREAM_READABLE, this, "Stream is not readable");
	debug_assertp(_pos<_size, this, "Cannot read past the size of the buffer");
	memcpy(data, (char*)_buffer+_pos, size);
	_pos+=size;
	return size;
}

size_t BufferStream::write(void const* data, size_t size) {
	debug_assertp(_buffer!=NULL, this, "Cannot write to closed stream");
	debug_assertp(_flags&STREAM_WRITEABLE, this, "Stream is not writeable");
	debug_assertp(_pos+size<=_size, this, "Cannot write past the size of the buffer");
	memcpy((char*)_buffer+_pos, data, size);
	_pos+=size;
	return size;
}

void BufferStream::flush() {
	/* Do nothing. */
}

bool BufferStream::eof() const {
	if (_buffer)
		return _pos==_size;
	return true;
}

size_t BufferStream::size() const {
	return _size;
}

unsigned long BufferStream::pos() const {
	return _pos;
}

unsigned long BufferStream::seek(unsigned long pos) {
	debug_assertp(_buffer!=NULL, this, "Cannot seek closed stream");
	debug_assertp(pos<=_size, this, "Cannot seek past the size of the buffer");
	_pos=pos;
	return _pos;
}

void BufferStream::close() {
	if (_buffer!=NULL) {
		_buffer=NULL;
		_pos=0;
		_size=0;
	}
}

// class ReadOnlyBufferStream implementation

ReadOnlyBufferStream::ReadOnlyBufferStream(void const* buffer, size_t size, unsigned int flags, char const* encoding)
	: _buffer(buffer), _pos(0), _size(size)
{
	setFlags(flags);
	setEncoding(encoding);
}

void ReadOnlyBufferStream::setBuffer(void const* buffer, size_t size) {
	_buffer=buffer;
	_size=size;
}

void const* ReadOnlyBufferStream::getBuffer() {
	return _buffer;
}

size_t ReadOnlyBufferStream::read(void* data, size_t size) {
	debug_assertp(_buffer!=NULL, this, "Cannot read from closed stream");
	debug_assertp(_flags&STREAM_READABLE, this, "Stream is not readable");
	debug_assertp(_pos<_size, this, "Cannot read past the size of the buffer");
	memcpy(data, (char*)_buffer+_pos, size);
	_pos+=size;
	return size;
}

size_t ReadOnlyBufferStream::write(void const*, size_t) {
	debug_assertp(false, this, "Cannot write to read-only buffer stream");
	return 0;
}

void ReadOnlyBufferStream::flush() {
	/* Do nothing. */
}

bool ReadOnlyBufferStream::eof() const {
	if (_buffer)
		return _pos==_size;
	return true;
}

size_t ReadOnlyBufferStream::size() const {
	return _size;
}

unsigned long ReadOnlyBufferStream::pos() const {
	return _pos;
}

unsigned long ReadOnlyBufferStream::seek(unsigned long pos) {
	debug_assertp(_buffer!=NULL, this, "Cannot seek closed stream");
	debug_assertp(pos<=_size, this, "Cannot seek past the size of the buffer");
	_pos=pos;
	return _pos;
}

void ReadOnlyBufferStream::close() {
	if (_buffer!=NULL) {
		_buffer=NULL;
		_pos=0;
		_size=0;
	}
}

void ReadOnlyBufferStream::setFlags(unsigned int flags) {
	_flags=flags&~STREAM_WRITEABLE;
}

} // namespace duct

