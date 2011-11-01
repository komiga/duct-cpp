/**
@file windows/filesystem.cpp
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

#include <string.h>
#include <windows.h>
#include <direct.h>
#include <io.h>
#include <duct/debug.hpp>
#include <duct/filesystem.hpp>

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

#define _S_ISDIR(mode) ((mode & _S_IFMT)==_S_IFDIR)
#define _S_ISREG(mode) ((mode & _S_IFMT)==_S_IFREG)

bool statPath(const char* path, struct _stat* s) {
	return _stat(path, s)==0;
}

bool statPath(const std::string& path, struct _stat* s) {
	return _stat(path.c_str(), s)==0;
}

bool statPath(const UnicodeString& path, struct _stat* s) {
	std::string str;
	path.toUTF8String(str);
	return _stat(str.c_str(), s)==0;
}

PathType pathType(const char* path) {
	struct _stat s;
	if (statPath(path, &s)) {
		return _S_ISREG(s.st_mode) ? PATHTYPE_FILE : (_S_ISDIR(s.st_mode) ? PATHTYPE_DIR : PATHTYPE_NONE);
	} else {
		return PATHTYPE_NONE;
	}
}

PathType pathType(const std::string& path) {
	struct _stat s;
	if (statPath(path, &s)) {
		return _S_ISREG(s.st_mode) ? PATHTYPE_FILE : (_S_ISDIR(s.st_mode) ? PATHTYPE_DIR : PATHTYPE_NONE);
	} else {
		return PATHTYPE_NONE;
	}
}

PathType pathType(const UnicodeString& path) {
	struct _stat s;
	if (statPath(path, &s)) {
		return _S_ISREG(s.st_mode) ? PATHTYPE_FILE : (_S_ISDIR(s.st_mode) ? PATHTYPE_DIR : PATHTYPE_NONE);
	} else {
		return PATHTYPE_NONE;
	}
}

bool changeDir(const char* path) {
	if (_chdir(path)==0) {
		return true;
	}
	return false;
}

bool changeDir(const std::string& path) {
	return changeDir(path.c_str());
}

bool changeDir(const UnicodeString& path) {
	std::string str;
	path.toUTF8String(str);
	return changeDir(str.c_str());
}

bool getWorkingDir(std::string& result) {
	char* buffer=_getcwd(NULL, 0);
	if (buffer!=NULL) {
		result.assign(buffer);
		free(buffer);
		return true;
	}
	return false;
}

bool getWorkingDir(UnicodeString& result) {
	char* buffer=_getcwd(NULL, 0);
	if (buffer!=NULL) {
		UnicodeString temp(buffer);
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

void getAbsolutePath(const UnicodeString& path, UnicodeString& result) {
	if (0!=path.length()) {
		if ('/'!=path[0]) {
			UnicodeString build;
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
	#define BUFSIZE 1024
	char buffer[BUFSIZE];
	int len=GetFullPathNameA(path.c_str(), BUFSIZE, buffer, NULL);
	if (0!=len) {
		result.assign(buffer, len);
		return true;
	}
	return false;
}

bool resolvePath(const UnicodeString& path, UnicodeString& result) {
	#define BUFSIZE 1024
	wchar_t buffer[BUFSIZE];
	UnicodeString path_copy;
	path_copy.fastCopyFrom(path);
	int len=GetFullPathNameW(path_copy.getTerminatedBuffer(), BUFSIZE, buffer, NULL);
	if (0!=len) {
		result.setTo(buffer, len);
		return true;
	}
	return false;
}

uint64_t getFileSize(const char* path) {
	struct _stat s;
	if (statPath(path, &s)) {
		return s.st_size;
	}
	return 0;
}

uint64_t getFileSize(const std::string& path) {
	struct _stat s;
	if (statPath(path, &s)) {
		return s.st_size;
	}
	return 0;
}

uint64_t getFileSize(const UnicodeString& path) {
	struct _stat s;
	if (statPath(path, &s)) {
		return s.st_size;
	}
	return 0;
}

bool dirExists(const char* path) {
	struct _stat s;
	if (statPath(path, &s)) {
		return _S_ISDIR(s.st_mode);
	} else {
		return false;
	}
}

bool dirExists(const std::string& path) {
	struct _stat s;
	if (statPath(path, &s)) {
		return _S_ISDIR(s.st_mode);
	} else {
		return false;
	}
}

bool dirExists(const UnicodeString& path) {
	struct _stat s;
	if (statPath(path, &s)) {
		return _S_ISDIR(s.st_mode);
	} else {
		return false;
	}
}

bool fileExists(const char* path) {
	struct _stat s;
	if (statPath(path, &s)) {
		return _S_ISREG(s.st_mode);
	} else {
		return false;
	}
}

bool fileExists(const std::string& path) {
	struct _stat s;
	if (statPath(path, &s)) {
		return _S_ISREG(s.st_mode);
	} else {
		return false;
	}
}

bool fileExists(const UnicodeString& path) {
	struct _stat s;
	if (statPath(path, &s)) {
		return _S_ISREG(s.st_mode);
	} else {
		return false;
	}
}

// TODO structure creation
bool createDir(const char* path, bool structure) {
	(void)structure;
	return CreateDirectory(path, NULL)!=0;
}

bool createDir(const std::string& path, bool structure) {
	return createDir(path.c_str(), structure);
}

bool createDir(const UnicodeString& path, bool structure) {
	std::string str;
	path.toUTF8String(str);
	return createDir(str.c_str(), structure);
}

bool createFile(const char* path, bool createpath) {
	(void)createpath;
	// TODO: dir/path extraction from file path
	/*if (createpath && !dirExists()) {
		createDir();
	}*/
	int fd=_creat(path, _S_IREAD|_S_IWRITE);
	if (fd!=-1) {
		_close(fd);
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
	return DeleteFile(path)!=0;
}

bool deleteFile(const std::string& path) {
	return DeleteFile(path.c_str())!=0;
}

bool deleteFile(const UnicodeString& path) {
	std::string str;
	path.toUTF8String(str);
	return DeleteFile(str.c_str())!=0;
}

bool deleteDir(const char* path) {
	return RemoveDirectory(path)!=0;
}

bool deleteDir(const std::string& path) {
	return RemoveDirectory(path.c_str())!=0;
}

bool deleteDir(const UnicodeString& path) {
	std::string str;
	path.toUTF8String(str);
	return RemoveDirectory(str.c_str())!=0;
}

} // namespace FileSystem

} // namespace duct

