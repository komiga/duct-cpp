/**
@file unix/filesystem.cpp
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
#include <duct/filesystem.hpp>

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

namespace duct {

// class DirStream implementation

const char* g_cstr_periods="..";

DirStream::DirStream(const char* path) {
	_path.append(path);
	init();
}

DirStream::DirStream(const std::string& path) {
	_path.append(path);
	init();
}

DirStream::DirStream(const icu::UnicodeString& path) {
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

bool DirStream::nextEntry(icu::UnicodeString& result) {
	_entry=readdir(_dir);
	if (_entry) {
		result.setTo(icu::UnicodeString(_entry->d_name));
		return true;
	}
	return false;
}

bool DirStream::entryName(icu::UnicodeString& result) const {
	if (_entry) {
		result.setTo(icu::UnicodeString(_entry->d_name));
		return true;
	}
	return false;
}

bool DirStream::isEntryParentOrRelative() const {
	if (_entry) {
		return (1==strlen(_entry->d_name) && 0==strncmp(g_cstr_periods, _entry->d_name, 1)) || 0==strcmp(g_cstr_periods, _entry->d_name);
	}
	return false;
}

bool DirStream::hasEntry() const {
	return NULL!=_entry;
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
	_entry=NULL;
}

// FileSystem implementation

namespace FileSystem {

bool statPath(const char* path, struct stat* s) {
	return stat(path, s)==0;
}

bool statPath(const std::string& path, struct stat* s) {
	return stat(path.c_str(), s)==0;
}

bool statPath(const icu::UnicodeString& path, struct stat* s) {
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

PathType pathType(const icu::UnicodeString& path) {
	struct stat s;
	if (statPath(path, &s)) {
		return S_ISREG(s.st_mode) ? PATHTYPE_FILE : (S_ISDIR(s.st_mode) ? PATHTYPE_DIR : PATHTYPE_NONE);
	} else {
		return PATHTYPE_NONE;
	}
}

bool changeDir(const char* path) {
	if (chdir(path)==0) {
		return true;
	}
	return false;
}

bool changeDir(const std::string& path) {
	return changeDir(path.c_str());
}

bool changeDir(const icu::UnicodeString& path) {
	std::string str;
	path.toUTF8String(str);
	return changeDir(str.c_str());
}

bool getWorkingDir(std::string& result) {
	char* buffer=getcwd(NULL, 0);
	if (buffer!=NULL) {
		result.assign(buffer);
		free(buffer);
		return true;
	}
	return false;
}

bool getWorkingDir(icu::UnicodeString& result) {
	char* buffer=getcwd(NULL, 0);
	if (buffer!=NULL) {
		icu::UnicodeString temp(buffer);
		result.setTo(temp);
		free(buffer);
		return true;
	}
	return false;
}

void getAbsolutePath(const std::string& path, std::string& result) {
	if (0!=path.size()) {
		if ('/'!=path[0]) {
			std::string build;
			if (!getWorkingDir(build, true)) {
				return;
			}
			build.append(path);
			result.assign(build);
		} else if (&result!=&path) {
			result.assign(path);
		}
	}
}

void getAbsolutePath(const icu::UnicodeString& path, icu::UnicodeString& result) {
	if (0!=path.length()) {
		if ('/'!=path[0]) {
			icu::UnicodeString build;
			if (!getWorkingDir(build, true)) {
				return;
			}
			build.append(path);
			result.setTo(build);
		} else if (&result!=&path) {
			result.setTo(path);
		}
	}
}

bool resolvePath(const std::string& path, std::string& result) {
	char* buffer=realpath(path.c_str(), NULL);
	if (buffer!=NULL) {
		result.assign(buffer);
		free(buffer);
		return true;
	}
	return false;
}

bool resolvePath(const icu::UnicodeString& path, icu::UnicodeString& result) {
	std::string str;
	path.toUTF8String(str);
	char* buffer=realpath(str.c_str(), NULL);
	if (buffer!=NULL) {
		icu::UnicodeString temp(buffer);
		result.setTo(temp);
		free(buffer);
		return true;
	}
	return false;
}

uint64_t getFileSize(const char* path) {
	struct stat s;
	if (statPath(path, &s)) {
		return s.st_size;
	}
	return 0;
}

uint64_t getFileSize(const std::string& path) {
	struct stat s;
	if (statPath(path, &s)) {
		return s.st_size;
	}
	return 0;
}

uint64_t getFileSize(const icu::UnicodeString& path) {
	struct stat s;
	if (statPath(path, &s)) {
		return s.st_size;
	}
	return 0;
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

bool dirExists(const icu::UnicodeString& path) {
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

bool fileExists(const icu::UnicodeString& path) {
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

bool createDir(const icu::UnicodeString& path, bool structure) {
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

bool createFile(const icu::UnicodeString& path, bool createpath) {
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

bool deleteFile(const icu::UnicodeString& path) {
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

bool deleteDir(const icu::UnicodeString& path) {
	std::string str;
	path.toUTF8String(str);
	return rmdir(str.c_str())==0;
}

} // namespace FileSystem

} // namespace duct

