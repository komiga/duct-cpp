/**
@file CoverStream.cpp
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
#include <duct/CoverStream.hpp>

namespace duct {

// class CoverStream implementation

CoverStream::CoverStream()
	: m_stream(NULL)
{/* Do nothing */}

CoverStream::~CoverStream() {
	if (m_autoclose && m_stream) {
		close();
	}
}

void CoverStream::setStream(Stream* stream) {
	if (m_autoclose && m_stream) {
		close();
	}
	m_stream=stream;
}

Stream* CoverStream::getStream() const {
	return m_stream;
}

void CoverStream::setAutoClose(bool autoclose) {
	m_autoclose=autoclose;
}

bool CoverStream::getAutoClose() const {
	return m_autoclose;
}

// Base functions passing down to the covered stream
void CoverStream::flush() {
	debug_assertp(m_stream, this, "Covered stream cannot be NULL");
	m_stream->flush();
}

bool CoverStream::eof() const {
	debug_assertp(m_stream, this, "Covered stream cannot be NULL");
	return m_stream->eof();
}

size_t CoverStream::size() const {
	debug_assertp(m_stream, this, "Covered stream cannot be NULL");
	return m_stream->size();
}

unsigned long CoverStream::pos() const {
	debug_assertp(m_stream, this, "Covered stream cannot be NULL");
	return m_stream->pos();
}

unsigned long CoverStream::seek(unsigned long pos) {
	debug_assertp(m_stream, this, "Covered stream cannot be NULL");
	return m_stream->seek(pos);
}

unsigned long CoverStream::skip(long change) {
	debug_assertp(m_stream, this, "Covered stream cannot be NULL");
	return m_stream->skip(change);
}

void CoverStream::close() {
	if (m_stream) {
		m_stream->close();
		m_stream=NULL;
	}
}

} // namespace duct
