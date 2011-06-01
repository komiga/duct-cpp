/**
@file inline/archive.inl
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

// class Archive inline implementation

inline
Archive::Archive(const icu::UnicodeString& path) : _stream(NULL), _path(path), _readable(false), _writeable(false) {
}

inline
Archive::~Archive() {
	close();
}

inline
Stream* Archive::getStream() {
	return _stream;
}

inline
const icu::UnicodeString& Archive::getPath() const {
	return _path;
}

inline
void Archive::setPath(const icu::UnicodeString& path) {
	_path.setTo(path);
}

inline
bool Archive::isOpen() const {
	return (_readable || _writeable);
}

inline
bool Archive::isReadable() const {
	return _readable;
}

inline
bool Archive::isWriteable() const {
	return _writeable;
}

inline
size_t Archive::getMetadataSize() const {
	return 4; // identifier
}

// class Entry inline implementation

inline
Entry::Entry() : _opened(false), _flags(ENTRYFLAG_NONE), _dataoffset(0), _datasize(0) {
}

inline
Entry::~Entry() {
}

inline
bool Entry::isOpen() const {
	return _opened;
}

inline
unsigned int Entry::getFlags() const {
	return _flags;
}

inline
void Entry::setFlags(unsigned int flags) {
	_flags=flags;
}

inline
bool Entry::isCompressed() const {
	return (_flags&ENTRYFLAG_COMPRESSED)!=0;
}

inline
void Entry::setCompressed(bool compressed) {
	if (compressed) {
		_flags|=ENTRYFLAG_COMPRESSED;
	} else {
		_flags&=~ENTRYFLAG_COMPRESSED;
	}
}

inline
size_t Entry::getDataOffset() const {
	return _dataoffset;
}

inline
unsigned int Entry::getDataSize() const {
	return _datasize;
}

inline
unsigned int Entry::getConstMetadataSize() {
	return 14;
}

inline
unsigned int Entry::getMetadataSize() const {
	return 14;
}

