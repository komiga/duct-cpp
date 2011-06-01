/**
@file archive.cpp
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

#include <duct/archive.hpp>

namespace duct {

// class Archive implementation

bool Archive::open(bool deserialize_, bool readable, bool writeable) {
	close();
	if ((!readable && !writeable) || _path.isEmpty()) {
		return false;
	}
	_stream=FileStream::openFile(_path, readable, writeable);
	if (!_stream) {
		return false;
	}
	_readable=readable;
	_writeable=writeable;
	if (deserialize_) {
		if (!deserialize()) {
			return false;
		}
	}
	return true;
}

void Archive::close() {
	if ((_readable || _writeable) && _stream!=NULL) {
		_stream->close();
		_stream=NULL;
	}
	_readable=false;
	_writeable=false;
}

bool Archive::save() {
	if (!_readable && _writeable) {
		return save(true);
	} else if (_readable || _writeable) {
		bool readable=_readable, writeable=_writeable;
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
	if (_readable) {
		clear();
		_stream->seek(0);
		if (_stream->size()<4) { // lowest possible size for a header
			return false;
		}
		char check[4];
		_stream->read(check, 4);
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
	if (_writeable) {
		_stream->seek(0);
		_stream->write(getIdentifier(), 4);
		if (!serializeUserspace()) {
			return false;
		}
		return true;
	}
	return false;
}

// class Entry implementation

bool Entry::deserialize(Stream* stream) {
	_flags=(unsigned short)stream->readShort();
	_dataoffset=(size_t)stream->readLong();
	_datasize=(unsigned int)stream->readInt();
	return deserializeUserspace(stream);
}

bool Entry::serialize(Stream* stream) {
	stream->writeShort(_flags);
	stream->writeLong(_dataoffset);
	stream->writeInt(_datasize);
	return serializeUserspace(stream);
}

} // namespace duct

