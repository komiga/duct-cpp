/**
@file filestream.cpp
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
#include <duct/filestream.hpp>
#include <stdio.h>
#include <stdarg.h>
#include <string>

namespace duct {

// class FileStream implementation

FileStream::FileStream() { /* Do nothing. */ }

FileStream::FileStream(const char* path, const char* encoding) {
	setEncoding(encoding);
	init(path, STREAM_WRITEABLE|STREAM_READABLE);
}

FileStream::FileStream(const UnicodeString& path, const char* encoding) {
	setEncoding(encoding);
	std::string temp;
	path.toUTF8String(temp);
	init(temp.c_str(), STREAM_WRITEABLE|STREAM_READABLE);
}

FileStream::FileStream(const char* path, bool readable, bool writeable, const char* encoding) {
	setEncoding(encoding);
	init(path, (readable ? STREAM_READABLE : 0)|(writeable ? STREAM_WRITEABLE : 0));
}

FileStream::FileStream(const UnicodeString& path, bool readable, bool writeable, const char* encoding) {
	setEncoding(encoding);
	std::string temp;
	path.toUTF8String(temp);
	init(temp.c_str(), (readable ? STREAM_READABLE : 0)|(writeable ? STREAM_WRITEABLE : 0));
}

FileStream::FileStream(const char* path, unsigned int flags, const char* encoding) {
	setEncoding(encoding);
	init(path, flags);
}

FileStream::FileStream(const UnicodeString& path, unsigned int flags, const char* encoding) {
	setEncoding(encoding);
	std::string temp;
	path.toUTF8String(temp);
	init(temp.c_str(), flags);
}

FileStream::~FileStream() {
	//debug_calledp(this);
	close();
}

bool FileStream::isOpen() const {
	return _file!=NULL;
}

size_t FileStream::read(void* data, size_t size) {
	debug_assertp(_file!=NULL, this, "Cannot read from closed stream");
	debug_assertp(_flags&STREAM_READABLE, this, "Stream is not readable");
	debug_assertp(_pos<_size, this, "Cannot read past the eof");
	size_t size_read=fread(data, 1, size, _file);
	debug_assertp(size_read==size, this, "Error reading from stream");
	_pos+=size_read;
	return size;
}

size_t FileStream::write(const void* data, size_t size) {
	debug_assertp(_file!=NULL, this, "Cannot write to closed stream");
	debug_assertp(_flags&STREAM_WRITEABLE, this, "Stream is not writeable");
	size_t size_written=fwrite(data, 1, size, _file);
	debug_assertp(size_written==size, this, "Error writing to stream");
	_pos+=size_written;
	if (_pos>_size)
		_size=_pos;
	return size_written;
}

void FileStream::flush() {
	debug_assertp(_file!=NULL, this, "Cannot flush closed stream");
	fflush(_file);
}

bool FileStream::eof() const {
	if (_file)
		return _pos==_size;
	return true;
}

size_t FileStream::size() const {
	return _size;
}

unsigned long FileStream::pos() const {
	return _pos;
}

unsigned long FileStream::seek(unsigned long pos) {
	debug_assertp(_file!=NULL, this, "Cannot seek closed stream");
	if (fseek(_file, pos, SEEK_SET)==0)
		_pos=pos;
	return _pos;
}

void FileStream::close() {
	//debug_calledp(this);
	if (_file!=NULL) {
		if (_flags&STREAM_WRITEABLE)
			flush();
		fclose(_file);
		_file=NULL;
		_pos=0;
		_size=0;
	}
}

int FileStream::scanf(const char* format, ...) {
	va_list vl;
	va_start(vl, format);
	int ni=vfscanf(_file, format, vl);
	va_end(vl);
	_pos=ftell(_file);
	return ni;
}

void FileStream::setFlags(unsigned int) {
	debug_printp(this, "warning: Flags cannot be set for FileStream");
	/* Do not set flags after stream has opened. Unless the file is reopened... */
}

void FileStream::init(const char* path, unsigned int flags) {
	if ((flags&FILESTREAM_APPEND))
		flags |= STREAM_WRITEABLE;
	debug_assertp((flags&STREAM_WRITEABLE) || (flags&STREAM_READABLE), this, "Stream must be writeable, readable, or both");
	const char* mode=NULL;
	if ((flags&FILESTREAM_APPEND) && (flags&STREAM_READABLE)) {
		mode="ab+";
	} else if ((flags&STREAM_WRITEABLE) && (flags&STREAM_READABLE)) {
		mode="rb+";
	} else if ((flags&FILESTREAM_APPEND)) {
		mode="ab";
	} else if (flags&STREAM_WRITEABLE) {
		mode="wb";
	} else if (flags&STREAM_READABLE) {
		mode="rb";
	}
	FILE* f=fopen(path, mode);
	init(f, flags);
}

void FileStream::init(FILE* file, unsigned int flags) {
	//debug_assertp(file!=NULL, this, "Cannot initialize on null pointer");
	_file=file;
	_pos=0;
	_flags=flags;
	// Determine size by seeking to the end of the file, and then back to the beginning
	if (_file) {
		fseek(_file, 0L, SEEK_END);
		_size=ftell(_file);
		fseek(_file, 0L, SEEK_SET);
	}
}

FileStream* FileStream::openFile(const char* path, bool readable, bool writeable, const char* encoding) {
	FileStream* fs=new FileStream(path, readable, writeable, encoding);
	if (fs->isOpen()) {
		return fs;
	} else {
		delete fs;
		return NULL;
	}
}

FileStream* FileStream::openFile(const UnicodeString& path, bool readable, bool writeable, const char* encoding) {
	FileStream* fs=new FileStream(path, readable, writeable, encoding);
	if (fs->isOpen()) {
		return fs;
	} else {
		delete fs;
		return NULL;
	}
}

FileStream* FileStream::openFile(const char* path, unsigned int flags, const char* encoding) {
	FileStream* fs=new FileStream(path, flags, encoding);
	if (fs->isOpen()) {
		return fs;
	} else {
		delete fs;
		return NULL;
	}
}

FileStream* FileStream::openFile(const UnicodeString& path, unsigned int flags, const char* encoding) {
	FileStream* fs=new FileStream(path, flags, encoding);
	if (fs->isOpen()) {
		return fs;
	} else {
		delete fs;
		return NULL;
	}
}

FileStream* FileStream::readFile(const char* path, const char* encoding) {
	FileStream* fs=new FileStream(path, true, false, encoding);
	if (fs->isOpen()) {
		return fs;
	} else {
		delete fs;
		return NULL;
	}
}

FileStream* FileStream::readFile(const UnicodeString& path, const char* encoding) {
	FileStream* fs=new FileStream(path, true, false, encoding);
	if (fs->isOpen()) {
		return fs;
	} else {
		delete fs;
		return NULL;
	}
}

FileStream* FileStream::writeFile(const char* path, const char* encoding) {
	FileStream* fs=new FileStream(path, false, true, encoding);
	if (fs->isOpen()) {
		return fs;
	} else {
		delete fs;
		return NULL;
	}
}

FileStream* FileStream::writeFile(const UnicodeString& path, const char* encoding) {
	FileStream* fs=new FileStream(path, false, true, encoding);
	if (fs->isOpen()) {
		return fs;
	} else {
		delete fs;
		return NULL;
	}
}

} // namespace duct

