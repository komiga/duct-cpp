/**
@file filestream.cpp
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
#include <duct/filestream.hpp>

#include <stdio.h>
#include <stdarg.h>
#include <string>

namespace duct {

// class FileStream implementation

FileStream::FileStream() {/* Do nothing */}

FileStream::FileStream(char const* path, char const* encoding) {
	setEncoding(encoding);
	init(path, STREAM_WRITEABLE|STREAM_READABLE);
}

FileStream::FileStream(std::string const& path, char const* encoding) {
	setEncoding(encoding);
	init(path.c_str(), STREAM_WRITEABLE|STREAM_READABLE);
}

FileStream::FileStream(icu::UnicodeString const& path, char const* encoding) {
	setEncoding(encoding);
	std::string temp;
	path.toUTF8String(temp);
	init(temp.c_str(), STREAM_WRITEABLE|STREAM_READABLE);
}

FileStream::FileStream(char const* path, bool readable, bool writeable, char const* encoding) {
	setEncoding(encoding);
	init(path, (readable ? STREAM_READABLE : 0)|(writeable ? STREAM_WRITEABLE : 0));
}

FileStream::FileStream(std::string const& path, bool readable, bool writeable, char const* encoding) {
	setEncoding(encoding);
	init(path.c_str(), (readable ? STREAM_READABLE : 0)|(writeable ? STREAM_WRITEABLE : 0));
}

FileStream::FileStream(icu::UnicodeString const& path, bool readable, bool writeable, char const* encoding) {
	setEncoding(encoding);
	std::string temp;
	path.toUTF8String(temp);
	init(temp.c_str(), (readable ? STREAM_READABLE : 0)|(writeable ? STREAM_WRITEABLE : 0));
}

FileStream::FileStream(char const* path, unsigned int flags, char const* encoding) {
	setEncoding(encoding);
	init(path, flags);
}

FileStream::FileStream(std::string const& path, unsigned int flags, char const* encoding) {
	setEncoding(encoding);
	init(path.c_str(), flags);
}

FileStream::FileStream(icu::UnicodeString const& path, unsigned int flags, char const* encoding) {
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
	return m_file!=NULL;
}

size_t FileStream::read(void* data, size_t size) {
	debug_assertp(m_file!=NULL, this, "Cannot read from closed stream");
	debug_assertp(m_flags&STREAM_READABLE, this, "Stream is not readable");
	debug_assertp(m_pos<m_size, this, "Cannot read past the eof");
	size_t size_read=fread(data, 1, size, m_file);
	debug_assertp(size_read==size, this, "Error reading from stream");
	m_pos+=size_read;
	return size;
}

size_t FileStream::write(void const* data, size_t size) {
	debug_assertp(m_file!=NULL, this, "Cannot write to closed stream");
	debug_assertp(m_flags&STREAM_WRITEABLE, this, "Stream is not writeable");
	size_t size_written=fwrite(data, 1, size, m_file);
	debug_assertp(size_written==size, this, "Error writing to stream");
	m_pos+=size_written;
	if (m_pos>m_size)
		m_size=m_pos;
	return size_written;
}

void FileStream::flush() {
	debug_assertp(m_file!=NULL, this, "Cannot flush closed stream");
	fflush(m_file);
}

bool FileStream::eof() const {
	if (m_file)
		return m_pos==m_size;
	return true;
}

size_t FileStream::size() const {
	return m_size;
}

unsigned long FileStream::pos() const {
	return m_pos;
}

unsigned long FileStream::seek(unsigned long pos) {
	debug_assertp(m_file!=NULL, this, "Cannot seek closed stream");
	if (fseek(m_file, pos, SEEK_SET)==0)
		m_pos=pos;
	return m_pos;
}

void FileStream::close() {
	//debug_calledp(this);
	if (m_file!=NULL) {
		if (m_flags&STREAM_WRITEABLE)
			flush();
		fclose(m_file);
		m_file=NULL;
		m_pos=0;
		m_size=0;
	}
}

/*int FileStream::scanf(char const* format, ...) {
	va_list vl;
	va_start(vl, format);
	#ifdef DUCT_PLATFORM_WINDOWS
	// TODO
	#else
	int ni=vfscanf(m_file, format, vl);
	#endif
	va_end(vl);
	m_pos=ftell(m_file);
	return ni;
}*/

void FileStream::setFlags(unsigned int) {
	debug_printp(this, "warning: Flags cannot be set for FileStream");
	/* Do not set flags after stream has opened. Unless the file is reopened... */
}

void FileStream::init(char const* path, unsigned int flags) {
	if ((flags&FILESTREAM_APPEND))
		flags |= STREAM_WRITEABLE;
	debug_assertp((flags&STREAM_WRITEABLE) || (flags&STREAM_READABLE), this, "Stream must be writeable, readable, or both");
	char const* mode=NULL;
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
	m_file=file;
	m_pos=0;
	m_flags=flags;
	// Determine size by seeking to the end of the file, and then back to the beginning
	if (m_file) {
		fseek(m_file, 0L, SEEK_END);
		m_size=ftell(m_file);
		fseek(m_file, 0L, SEEK_SET);
	}
}

FileStream* FileStream::openFile(char const* path, bool readable, bool writeable, char const* encoding) {
	FileStream* fs=new FileStream(path, readable, writeable, encoding);
	if (fs->isOpen()) {
		return fs;
	} else {
		delete fs;
		return NULL;
	}
}

FileStream* FileStream::openFile(std::string const& path, bool readable, bool writeable, char const* encoding) {
	FileStream* fs=new FileStream(path.c_str(), readable, writeable, encoding);
	if (fs->isOpen()) {
		return fs;
	} else {
		delete fs;
		return NULL;
	}
}

FileStream* FileStream::openFile(icu::UnicodeString const& path, bool readable, bool writeable, char const* encoding) {
	FileStream* fs=new FileStream(path, readable, writeable, encoding);
	if (fs->isOpen()) {
		return fs;
	} else {
		delete fs;
		return NULL;
	}
}

FileStream* FileStream::openFile(char const* path, unsigned int flags, char const* encoding) {
	FileStream* fs=new FileStream(path, flags, encoding);
	if (fs->isOpen()) {
		return fs;
	} else {
		delete fs;
		return NULL;
	}
}

FileStream* FileStream::openFile(std::string const& path, unsigned int flags, char const* encoding) {
	FileStream* fs=new FileStream(path.c_str(), flags, encoding);
	if (fs->isOpen()) {
		return fs;
	} else {
		delete fs;
		return NULL;
	}
}

FileStream* FileStream::openFile(icu::UnicodeString const& path, unsigned int flags, char const* encoding) {
	FileStream* fs=new FileStream(path, flags, encoding);
	if (fs->isOpen()) {
		return fs;
	} else {
		delete fs;
		return NULL;
	}
}

FileStream* FileStream::readFile(char const* path, char const* encoding) {
	FileStream* fs=new FileStream(path, true, false, encoding);
	if (fs->isOpen()) {
		return fs;
	} else {
		delete fs;
		return NULL;
	}
}

FileStream* FileStream::readFile(std::string const& path, char const* encoding) {
	FileStream* fs=new FileStream(path.c_str(), true, false, encoding);
	if (fs->isOpen()) {
		return fs;
	} else {
		delete fs;
		return NULL;
	}
}

FileStream* FileStream::readFile(icu::UnicodeString const& path, char const* encoding) {
	FileStream* fs=new FileStream(path, true, false, encoding);
	if (fs->isOpen()) {
		return fs;
	} else {
		delete fs;
		return NULL;
	}
}

FileStream* FileStream::writeFile(char const* path, char const* encoding) {
	FileStream* fs=new FileStream(path, false, true, encoding);
	if (fs->isOpen()) {
		return fs;
	} else {
		delete fs;
		return NULL;
	}
}

FileStream* FileStream::writeFile(std::string const& path, char const* encoding) {
	FileStream* fs=new FileStream(path.c_str(), false, true, encoding);
	if (fs->isOpen()) {
		return fs;
	} else {
		delete fs;
		return NULL;
	}
}

FileStream* FileStream::writeFile(icu::UnicodeString const& path, char const* encoding) {
	FileStream* fs=new FileStream(path, false, true, encoding);
	if (fs->isOpen()) {
		return fs;
	} else {
		delete fs;
		return NULL;
	}
}

} // namespace duct

