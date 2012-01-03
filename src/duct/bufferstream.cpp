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
	: m_buffer(buffer), m_pos(0), m_size(size)
{
	setFlags(flags);
	setEncoding(encoding);
}

void BufferStream::setBuffer(void* buffer, size_t size) {
	m_buffer=buffer;
	m_size=size;
}

void* BufferStream::getBuffer() {
	return m_buffer;
}

size_t BufferStream::read(void* data, size_t size) {
	debug_assertp(m_buffer!=NULL, this, "Cannot read from closed stream");
	debug_assertp(m_flags&STREAM_READABLE, this, "Stream is not readable");
	debug_assertp(m_pos<m_size, this, "Cannot read past the size of the buffer");
	memcpy(data, (char*)m_buffer+m_pos, size);
	m_pos+=size;
	return size;
}

size_t BufferStream::write(void const* data, size_t size) {
	debug_assertp(m_buffer!=NULL, this, "Cannot write to closed stream");
	debug_assertp(m_flags&STREAM_WRITEABLE, this, "Stream is not writeable");
	debug_assertp(m_pos+size<=m_size, this, "Cannot write past the size of the buffer");
	memcpy((char*)m_buffer+m_pos, data, size);
	m_pos+=size;
	return size;
}

void BufferStream::flush() {/* Do nothing */}

bool BufferStream::eof() const {
	if (m_buffer)
		return m_pos==m_size;
	return true;
}

size_t BufferStream::size() const {
	return m_size;
}

unsigned long BufferStream::pos() const {
	return m_pos;
}

unsigned long BufferStream::seek(unsigned long pos) {
	debug_assertp(m_buffer!=NULL, this, "Cannot seek closed stream");
	debug_assertp(pos<=m_size, this, "Cannot seek past the size of the buffer");
	m_pos=pos;
	return m_pos;
}

void BufferStream::close() {
	if (m_buffer!=NULL) {
		m_buffer=NULL;
		m_pos=0;
		m_size=0;
	}
}

// class ReadOnlyBufferStream implementation

ReadOnlyBufferStream::ReadOnlyBufferStream(void const* buffer, size_t size, unsigned int flags, char const* encoding)
	: m_buffer(buffer), m_pos(0), m_size(size)
{
	setFlags(flags);
	setEncoding(encoding);
}

void ReadOnlyBufferStream::setBuffer(void const* buffer, size_t size) {
	m_buffer=buffer;
	m_size=size;
}

void const* ReadOnlyBufferStream::getBuffer() {
	return m_buffer;
}

size_t ReadOnlyBufferStream::read(void* data, size_t size) {
	debug_assertp(m_buffer!=NULL, this, "Cannot read from closed stream");
	debug_assertp(m_flags&STREAM_READABLE, this, "Stream is not readable");
	debug_assertp(m_pos<m_size, this, "Cannot read past the size of the buffer");
	memcpy(data, (char*)m_buffer+m_pos, size);
	m_pos+=size;
	return size;
}

size_t ReadOnlyBufferStream::write(void const*, size_t) {
	debug_assertp(false, this, "Cannot write to read-only buffer stream");
	return 0;
}

void ReadOnlyBufferStream::flush() {/* Do nothing */}

bool ReadOnlyBufferStream::eof() const {
	if (m_buffer)
		return m_pos==m_size;
	return true;
}

size_t ReadOnlyBufferStream::size() const {
	return m_size;
}

unsigned long ReadOnlyBufferStream::pos() const {
	return m_pos;
}

unsigned long ReadOnlyBufferStream::seek(unsigned long pos) {
	debug_assertp(m_buffer!=NULL, this, "Cannot seek closed stream");
	debug_assertp(pos<=m_size, this, "Cannot seek past the size of the buffer");
	m_pos=pos;
	return m_pos;
}

void ReadOnlyBufferStream::close() {
	if (m_buffer!=NULL) {
		m_buffer=NULL;
		m_pos=0;
		m_size=0;
	}
}

void ReadOnlyBufferStream::setFlags(unsigned int flags) {
	m_flags=flags&~STREAM_WRITEABLE;
}

} // namespace duct

