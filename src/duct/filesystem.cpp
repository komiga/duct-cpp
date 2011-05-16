/**
@file filesystem.cpp
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

#include <stdio.h>
#include <fcntl.h>
#include <duct/debug.hpp>
#include <duct/filesystem.hpp>

namespace duct {

// class DirStream implementation

DirStream::DirStream(const char* path) {
	_path.append(path);
	init();
}

DirStream::DirStream(const std::string& path) {
	_path.append(path);
	init();
}

DirStream::DirStream(const UnicodeString& path) {
	path.toUTF8String(_path);
	init();
}

DirStream::~DirStream() {
	if (_dir) {
		closedir(_dir);
		_dir=NULL;
		_entry=NULL;
	}
}

bool DirStream::nextEntry() {
	_entry=readdir(_dir);
	return _entry!=NULL;
}

bool DirStream::nextEntry(UnicodeString& result) {
	_entry=readdir(_dir);
	if (_entry) {
		result.setTo(UnicodeString(_entry->d_name));
		return true;
	}
	return false;
}

bool DirStream::entryName(UnicodeString& result) const {
	if (_entry) {
		result.setTo(UnicodeString(_entry->d_name));
		return true;
	}
	return false;
}

PathType DirStream::entryType() const {
	if (_entry) {
		std::string temp(_path);
		temp.append(_entry->d_name);
		return FileSystem::pathType(temp);
	} else {
		return PATHTYPE_NONE;
	}
}

bool DirStream::isOpen() const {
	return _dir!=NULL;
}

bool DirStream::close() {
	if (_dir) {
		closedir(_dir);
		_dir=NULL;
		return true;
	}
	return false;
}

void DirStream::init() {
	char c=_path[_path.length()-1];
	if (c=='\\') {
		_path.replace(_path.length()-1, 1, 1, '/');
	} else if (c!='/') {
		_path.append("/");
	}
	_dir=opendir(_path.c_str());
}

// FileSystem implementation

namespace FileSystem {

bool statPath(const char* path, struct stat* s) {
	return stat(path, s)==0;
}

bool statPath(const std::string& path, struct stat* s) {
	return stat(path.c_str(), s)==0;
}

bool statPath(const UnicodeString& path, struct stat* s) {
	std::string str;
	path.toUTF8String(str);
	return stat(str.c_str(), s)==0;
}

PathType pathType(const char* path) {
	struct stat s;
	if (statPath(path, &s)) {
		return S_ISREG(s.st_mode) ? PATHTYPE_FILE : (S_ISDIR(s.st_mode) ? PATHTYPE_DIR : PATHTYPE_NONE);
	} else {
		return PATHTYPE_NONE;
	}
}

PathType pathType(const std::string& path) {
	struct stat s;
	if (statPath(path, &s)) {
		return S_ISREG(s.st_mode) ? PATHTYPE_FILE : (S_ISDIR(s.st_mode) ? PATHTYPE_DIR : PATHTYPE_NONE);
	} else {
		return PATHTYPE_NONE;
	}
}

PathType pathType(const UnicodeString& path) {
	struct stat s;
	if (statPath(path, &s)) {
		return S_ISREG(s.st_mode) ? PATHTYPE_FILE : (S_ISDIR(s.st_mode) ? PATHTYPE_DIR : PATHTYPE_NONE);
	} else {
		return PATHTYPE_NONE;
	}
}

bool dirExists(const char* path) {
	struct stat s;
	if (statPath(path, &s)) {
		return S_ISDIR(s.st_mode);
	} else {
		return false;
	}
}

bool dirExists(const std::string& path) {
	struct stat s;
	if (statPath(path, &s)) {
		return S_ISDIR(s.st_mode);
	} else {
		return false;
	}
}

bool dirExists(const UnicodeString& path) {
	struct stat s;
	if (statPath(path, &s)) {
		return S_ISDIR(s.st_mode);
	} else {
		return false;
	}
}

bool fileExists(const char* path) {
	struct stat s;
	if (statPath(path, &s)) {
		return S_ISREG(s.st_mode);
	} else {
		return false;
	}
}

bool fileExists(const std::string& path) {
	struct stat s;
	if (statPath(path, &s)) {
		return S_ISREG(s.st_mode);
	} else {
		return false;
	}
}

bool fileExists(const UnicodeString& path) {
	struct stat s;
	if (statPath(path, &s)) {
		return S_ISREG(s.st_mode);
	} else {
		return false;
	}
}

// TODO structure creation
bool createDir(const char* path, bool structure) {
	return mkdir(path, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH)==0;
}

bool createDir(const std::string& path, bool structure) {
	return createDir(path.c_str(), false);
}

bool createDir(const UnicodeString& path, bool structure) {
	std::string str;
	path.toUTF8String(str);
	return createDir(str.c_str(), false);
}

bool createFile(const char* path, bool createpath) {
	// TODO: dir/path extraction from file path
	/*if (createpath && !dirExists()) {
		createDir();
	}*/
	int fd=creat(path, 0);
	if (fd!=-1) {
		close(fd);
		return true;
	}
	return false;
}

bool createFile(const std::string& path, bool createpath) {
	return createFile(path.c_str(), createpath);
}

bool createFile(const UnicodeString& path, bool createpath) {
	std::string str;
	path.toUTF8String(str);
	return createFile(str.c_str(), createpath);
}

bool deleteFile(const char* path) {
	return remove(path)==0;
}

bool deleteFile(const std::string& path) {
	return remove(path.c_str())==0;
}

bool deleteFile(const UnicodeString& path) {
	std::string str;
	path.toUTF8String(str);
	return remove(str.c_str())==0;
}

bool deleteDir(const char* path) {
	return rmdir(path)==0;
}

bool deleteDir(const std::string& path) {
	return rmdir(path.c_str())==0;
}

bool deleteDir(const UnicodeString& path) {
	std::string str;
	path.toUTF8String(str);
	return rmdir(str.c_str())==0;
}

} // namespace FileSystem

} // namespace duct

