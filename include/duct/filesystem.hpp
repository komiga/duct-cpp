/**
@file filesystem.hpp
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

@section DESCRIPTION

duct++ file system helpers.
*/

#ifndef _DUCT_FILESYSTEM_HPP
#define _DUCT_FILESYSTEM_HPP

#include <duct/config.hpp>
#ifdef DUCT_PLATFORM_WINDOWS
#include <duct/win32/dirent.h>
#else
#include <dirent.h>
#endif
#include <sys/stat.h>
#include <string>
#include <unicode/unistr.h>

namespace duct {

/**
	File types.
*/
enum PathType {
	/**
		Non-existant file.
	*/
	PATHTYPE_NONE=0,
	/**
		File type.
		Non-directories are always treated as files (e.g. links on UNIX).
	*/
	PATHTYPE_FILE,
	/**
		Directory type.
	*/
	PATHTYPE_DIR
};

/**
	Directory stream.
*/
class DUCT_API DirStream {
public:
	/**
		Constructor with path.
		isOpen() should be called to check if the stream was opened successfully.
		@param path The path to open.
		@see isOpen()
	*/
	DirStream(const char* path);
	/**
		Constructor with path.
		isOpen() should be called to check if the stream was opened successfully.
		@param path The path to open.
		@see isOpen()
	*/
	DirStream(const std::string& path);
	/**
		Constructor with path.
		isOpen() should be called to check if the stream was opened successfully.
		@param path The path to open.
		@see isOpen()
	*/
	DirStream(const UnicodeString& path);
	/**
		Destructor.
		Stream will be closed upon destruction.
	*/
	~DirStream();
	
	/**
		Get the next entry in the directory stream.
		If the next entry could not be retrieved, the current entry will be NULL'd and the stream should be closed.
		@returns true if the next entry was retrieved, or false if the next entry could not be retrieved (already at the last entry, or the stream was closed).
	*/
	bool nextEntry();
	/**
		Get the next entry in the directory stream and fill the result string with its name.
		If the next entry could not be retrieved, the current entry will be NULL'd and the stream should be closed.
		@returns true if the next entry was retrieved, or false if the next entry could not be retrieved (already at the last entry, or the stream was closed).
		@param result Output string. Will not be cleared if the next entry was not retrieved.
	*/
	bool nextEntry(UnicodeString& result);
	
	/**
		Get the current entry's name from the given directory stream.
		@returns true if the result string was set, or false the current entry was invalid.
		@param result Output string. Will not be cleared if the current entry is invalid.
		@see entryType()
	*/
	bool entryName(UnicodeString& result) const;
	/**
		Get the current entry's type.
		@returns The current entry's type. Either PATHTYPE_FILE or PATHTYPE_DIR.
		@see entryName()
		@see PathType
	*/
	PathType entryType() const;
	
	/**
		Check if the stream is opened.
		@returns true if the stream is opened, or false if it is not.
	*/
	bool isOpen() const;
	/**
		Close the directory stream.
		@returns true if the stream was closed, or false if the stream was already closed or not opened.
	*/
	bool close();
	
protected:
	std::string _path;
	DIR* _dir;
	dirent* _entry;
	
private:
	void init();
	
};

/**
	File system functions.
*/
namespace FileSystem {

/**
	Get stats on the given path.
	@returns true if the stat() function was successful, or false if it was not.
	@param path The path to get stats on.
	@param s The pointer to store the result.
*/
bool statPath(const char* path, struct stat* s);
/**
	Get stats on the given path.
	@returns true if the stat() function was successful, or false if it was not.
	@param path The path to get stats on.
	@param s The pointer to store the result.
*/
bool statPath(const std::string& path, struct stat* s);
/**
	Get stats on the given path.
	@returns true if the stat() function was successful, or false if it was not.
	@param path The path to get stats on.
	@param s The pointer to store the result.
*/
bool statPath(const UnicodeString& path, struct stat* s);

/**
	Get the given path's type.
	@returns The given path's type. PATHTYPE_NONE, PATHTYPE_FILE, or PATHTYPE_DIR.
	@param path The path to test.
*/
PathType pathType(const char* path);
/**
	Get the given path's type.
	@returns The given path's type. PATHTYPE_NONE, PATHTYPE_FILE, or PATHTYPE_DIR.
	@param path The path to test.
*/
PathType pathType(const std::string& path);
/**
	Get the given path's type.
	@returns The given path's type. PATHTYPE_NONE, PATHTYPE_FILE, or PATHTYPE_DIR.
	@param path The path to test.
*/
PathType pathType(const UnicodeString& path);

/**
	Check if the given directory exists.
	@returns true if the directory exists, or false if it does not.
	@param path The path to test.
*/
bool dirExists(const char* path);
/**
	Check if the given directory exists.
	@returns true if the directory exists, or false if it does not.
	@param path The path to test.
*/
bool dirExists(const std::string& path);
/**
	Check if the given directory exists.
	@returns true if the directory exists, or false if it does not.
	@param path The path to test.
*/
bool dirExists(const UnicodeString& path);

/**
	Check if the given file exists.
	@returns true if the file exists, or false if it does not.
	@param path The path to test.
*/
bool fileExists(const char* path);
/**
	Check if the given file exists.
	@returns true if the file exists, or false if it does not.
	@param path The path to test.
*/
bool fileExists(const std::string& path);
/**
	Check if the given file exists.
	@returns true if the file exists, or false if it does not.
	@param path The path to test.
*/
bool fileExists(const UnicodeString& path);

/**
	Create the given directory path.
	@returns true if the directory path was created, or false if the directory path could not be created.
	@param path The directory path to create.
	@param structure Whether to create the entire path structure (e.g. if the directory 'foo' does not exist in the path 'foo/bar', it will be created if structure is true).
*/
bool createDir(const char* path, bool structure=false);
/**
	Create the given directory path.
	@returns true if the directory path was created, or false if the directory path could not be created.
	@param path The directory path to create.
	@param structure Whether to create the entire path structure (e.g. if the directory 'foo' does not exist in the path 'foo/bar', it will be created if structure is true).
*/
bool createDir(const std::string& path, bool structure=false);
/**
	Create the given directory path.
	@returns true if the directory path was created, or false if the directory path could not be created.
	@param path The directory path to create.
	@param structure Whether to create the entire path structure (e.g. if the directory 'foo' does not exist in the path 'foo/bar', it will be created if structure is true).
*/
bool createDir(const UnicodeString& path, bool structure=false);

/**
	Create the given file.
	@returns true if the file was created, or false if the file could not be created.
	@param path The file path to create.
	@param createpath Whether to write the entire directory structure for the file path (e.g. if the directory 'foo' does not exist in the path 'foo/bar.bork', it will be created if structure is true).
*/
bool createFile(const char* path, bool createpath=false);
/**
	Create the given file.
	@returns true if the file was created, or false if the file could not be created.
	@param path The file path to create.
	@param createpath Whether to write the entire directory structure for the file path (e.g. if the directory 'foo' does not exist in the path 'foo/bar.bork', it will be created if structure is true).
*/
bool createFile(const std::string& path, bool createpath=false);
/**
	Create the given file.
	@returns true if the file was created, or false if the file could not be created.
	@param path The file path to create.
	@param createpath Whether to write the entire directory structure for the file path (e.g. if the directory 'foo' does not exist in the path 'foo/bar.bork', it will be created if structure is true).
*/
bool createFile(const UnicodeString& path, bool createpath=false);

/**
	Delete the given file.
	@returns true if the file was deleted, or false if it was not.
	@param path The path of the file to remove.
*/
bool deleteFile(const char* path);
/**
	Delete the given file.
	@returns true if the file was deleted, or false if it was not.
	@param path The path of the file to remove.
*/
bool deleteFile(const std::string& path);
/**
	Delete the given file.
	@returns true if the file was deleted, or false if it was not.
	@param path The path of the file to remove.
*/
bool deleteFile(const UnicodeString& path);

/**
	Delete the given directory.
	@returns true if the directory was deleted, or false if it was not.
	@param path The directory to remove.
*/
bool deleteDir(const char* path);
/**
	Delete the given directory.
	@returns true if the directory was deleted, or false if it was not.
	@param path The directory to remove.
*/
bool deleteDir(const std::string& path);
/**
	Delete the given directory.
	@returns true if the directory was deleted, or false if it was not.
	@param path The directory to remove.
*/
bool deleteDir(const UnicodeString& path);

} // namespace FileSystem

} // namespace duct

#endif // _DUCT_FILESYSTEM_HPP

