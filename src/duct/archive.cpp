/**
@file archive.cpp
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

#include <duct/archive.hpp>

namespace duct {

// class Archive implementation

bool Archive::open(bool deserialize_, bool readable, bool writeable) {
	close();
	if ((!readable && !writeable) || m_path.isEmpty()) {
		return false;
	}
	m_stream=FileStream::openFile(m_path, readable, writeable);
	if (!m_stream) {
		return false;
	}
	m_readable=readable;
	m_writeable=writeable;
	if (deserialize_) {
		if (!deserialize()) {
			return false;
		}
	}
	return true;
}

void Archive::close() {
	if (m_stream!=NULL) {
		m_stream->close();
		delete m_stream;
		m_stream=NULL;
	}
	m_readable=false;
	m_writeable=false;
}

bool Archive::save() {
	if (!m_readable && m_writeable) {
		return save(true);
	} else if (m_readable || m_writeable) {
		bool readable=m_readable, writeable=m_writeable;
		bool rc=save(false);
		if (rc) {
			return open(false, readable, writeable);
		}
	} else {
		return save(false);
	}
	return false;
}

bool Archive::save(bool keepopen) {
	if (!open(false, false, true)) { // reopen with write-access
		return false;
	}
	if (!writeEntries()) {
		return false;
	}
	if (!serialize()) {
		return false;
	}
	if (!keepopen) {
		close();
	}
	return true;
}

bool Archive::deserialize() {
	if (m_readable) {
		clear();
		m_stream->seek(0);
		if (m_stream->size()<4) { // lowest possible size for a header
			return false;
		}
		char check[4];
		m_stream->read(check, 4);
		if (strncmp(check, getIdentifier(), 4)!=0) {
			return false;
		}
		if (!deserializeUserspace()) {
			return false;
		}
		return true;
	}
	return false;
}

bool Archive::serialize() {
	if (m_writeable) {
		m_stream->seek(0);
		m_stream->write(getIdentifier(), 4);
		if (!serializeUserspace()) {
			return false;
		}
		return true;
	}
	return false;
}

// class Entry implementation

bool Entry::deserialize(Stream* stream) {
	m_flags=stream->readUInt16();
	m_dataoffset=stream->readUInt64();
	m_datasize=stream->readUInt32();
	return deserializeUserspace(stream);
}

bool Entry::serialize(Stream* stream) {
	stream->writeUInt16(m_flags);
	stream->writeUInt64(m_dataoffset);
	stream->writeUInt32(m_datasize);
	return serializeUserspace(stream);
}

} // namespace duct

