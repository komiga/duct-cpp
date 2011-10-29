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

#include <sys/stat.h>
#include <string>
#include <unicode/unistr.h>
#include <duct/config.hpp>
#ifdef DUCT_PLATFORM_WINDOWS
#include <duct/windows/dirent.h>
#include <direct.h>
#include <io.h>
#else
#include <dirent.h>
#endif

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
	
private:
	std::string _path;
	DIR* _dir;
	dirent* _entry;
	void init();
};

/**
	File system functions.
	No functions here will automatically normalize paths. They are always expected to be normalized (that is, with <em>'/'</em> as the separator).
	See FileSystem::normalizePath().
*/
namespace FileSystem {

/*
	Get stats on the given path.
	@returns true if the stat() function was successful, or false if it was not.
	@param path The path to get stats on.
	@param s The pointer to store the result.
*/
//bool DUCT_API statPath(const char* path, struct stat* s);
/*
	Get stats on the given path.
	@returns true if the stat() function was successful, or false if it was not.
	@param path The path to get stats on.
	@param s The pointer to store the result.
*/
//bool DUCT_API statPath(const std::string& path, struct stat* s);
/*
	Get stats on the given path.
	@returns true if the stat() function was successful, or false if it was not.
	@param path The path to get stats on.
	@param s The pointer to store the result.
*/
//bool DUCT_API statPath(const UnicodeString& path, struct stat* s);

/**
	Normalize the given path. This will replace all occurrances of <em>'\\'</em> with <em>'/'</em>.
	Any trailing slashes are kept as-is.
	@returns Nothing.
	@param path The path to normalize.
	@param result The resulting string.
*/
void DUCT_API normalizePath(const std::string& path, std::string& result);
/**
	Normalize the given path. This will replace all occurrances of <em>'\\'</em> with <em>'/'</em>.
	Any trailing slashes are kept as-is.
	@returns Nothing.
	@param path The path to normalize.
	@param result The resulting string.
*/
void DUCT_API normalizePath(const UnicodeString& path, UnicodeString& result);

/**
	Normalize the given path. This will replace all occurrances of <em>'\\'</em> with <em>'/'</em>.
	@returns Nothing.
	@param path The path to normalize.
	@param result The resulting string.
	@param trailing_slash Whether to force a trailing slash or to remove the existing one.
*/
void DUCT_API normalizePath(const std::string& path, std::string& result, bool trailing_slash);
/**
	Normalize the given path. This will replace all occurrances of <em>'\\'</em> with <em>'/'</em>.
	@returns Nothing.
	@param path The path to normalize.
	@param result The resulting string.
	@param trailing_slash Whether to force a trailing slash or to remove the existing one.
*/
void DUCT_API normalizePath(const UnicodeString& path, UnicodeString& result, bool trailing_slash);

/**
	Check if the given path has a trailing slash.
	@returns true if the given path has a trailing slash, or false if the path does not have a trailing slash.
	@param path The path to test.
*/
bool DUCT_API pathHasTrailingSlash(const std::string& path);
/**
	Check if the given path has a trailing slash.
	@returns true if the given path has a trailing slash, or false if the path does not have a trailing slash.
	@param path The path to test.
*/
bool DUCT_API pathHasTrailingSlash(const UnicodeString& path);

/**
	Check if the given path has a file name.
	@returns true if the given path appears to have a filename, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API pathHasFilename(const std::string& path);
/**
	Check if the given path has a file name.
	@returns true if the given path appears to have a filename, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API pathHasFilename(const UnicodeString& path);

/**
	Check if the given path has an extension.
	@returns true if the given path has an extension, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API pathHasExtension(const std::string& path);
/**
	Check if the given path has an extension.
	@returns true if the given path has an extension, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API pathHasExtension(const UnicodeString& path);

/**
	Check if the given path has a left part.
	@returns true if the given path has a left part, or false if it does not.
	@param path The path to test.
	@param allow_leading Whether to allow a leading period in the path part, as in UNIX-style hidden files (true by default).
*/
bool DUCT_API pathHasLeftPart(const std::string& path, bool allow_leading=true);
/**
	Check if the given path has a left part.
	@returns true if the given path has a left part, or false if it does not.
	@param path The path to test.
	@param allow_leading Whether to allow a leading period in the path part, as in UNIX-style hidden files (true by default).
*/
bool DUCT_API pathHasLeftPart(const UnicodeString& path, bool allow_leading=true);

/**
	Check if the given path has a right part.
	@returns true if the given path has a right part, or false if it does not.
	@param path The path to test.
	@param allow_leading Whether to allow a leading period in the path part, as in UNIX-style hidden files (true by default).
*/
bool DUCT_API pathHasRightPart(const std::string& path, bool allow_leading=true);
/**
	Check if the given path has a right part.
	@returns true if the given path has a right part, or false if it does not.
	@param path The path to test.
	@param allow_leading Whether to allow a leading period in the path part, as in UNIX-style hidden files (true by default).
*/
bool DUCT_API pathHasRightPart(const UnicodeString& path, bool allow_leading=true);

/**
	Check if the given path has a directory path.
	@returns true if the given path has a directory path, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API pathHasDirectory(const std::string& path);
/**
	Check if the given path has a directory path.
	@returns true if the given path has a directory path, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API pathHasDirectory(const UnicodeString& path);

/**
	Extract the extension from the given file path.
	@returns true if the extension was extracted, or false if the path doesn't have an extension.
	@param path The path to extract from.
	@param result This will be assigned to the extension if the path contains one.
	@param include_period Whether to include the period in the extension (false by default).
*/
bool DUCT_API extractFileExtension(const std::string& path, std::string& result, bool include_period=false);
/**
	Extract the extension from the given file path.
	@returns true if the extension was extracted, or false if the path doesn't have an extension.
	@param path The path to extract from.
	@param result This will be assigned to the extension if the path contains one.
	@param include_period Whether to include the period in the extension (false by default).
*/
bool DUCT_API extractFileExtension(const UnicodeString& path, UnicodeString& result, bool include_period=false);

/**
	Extract the directory path from the given file path.
	@returns true if the given file path has a directory path, or false if it does not.
	@param path The file path to extract from.
	@param result This will be assigned to the directory if the path contains one.
	@param trailing_slash Whether to append a trailing slash to the directory path.
*/
bool DUCT_API extractFileDirectory(const std::string& path, std::string& result, bool trailing_slash=true);
/**
	Extract the directory path from the given file path.
	@returns true if the given file path has a directory path, or false if it does not.
	@param path The file path to extract from.
	@param result This will be assigned to the directory if the path contains one.
	@param trailing_slash Whether to append a trailing slash to the directory path.
*/
bool DUCT_API extractFileDirectory(const UnicodeString& path, UnicodeString& result, bool trailing_slash=true);

/**
	Extract the file name from the given file path.
	@returns true if the given path contains a file name, or false if it does not (for example, "/some/directory/path/").
	@param path The file path to extract from.
	@param result This will be assigned to the filename.
	@param with_extension Whether to include the extension.
*/
bool DUCT_API extractFilename(const std::string& path, std::string& result, bool with_extension);
/**
	Extract the file name from the given file path.
	@returns true if the given path contains a file name, or false if it does not (for example, "/some/directory/path/").
	@param path The file path to extract from.
	@param result This will be assigned to the filename.
	@param with_extension Whether to include the extension.
*/
bool DUCT_API extractFilename(const UnicodeString& path, UnicodeString& result, bool with_extension);

/**
	Extract the file path part before the first period.
	@returns true if the part was extracted, or false otherwise.
	@param path The file path to extract from.
	@param result This will be assigned to the right half.
	@param allow_leading Whether to allow a leading period in the path part, as in UNIX-style hidden files (true by default).
*/
bool DUCT_API extractFileLeftPart(const std::string& path, std::string& result, bool allow_leading=true);
/**
	Extract the file path part before the first period.
	@returns true if the part was extracted, or false otherwise.
	@param path The file path to extract from.
	@param result This will be assigned to the right half.
	@param allow_leading Whether to allow a leading period in the path part, as in UNIX-style hidden files (true by default).
*/
bool DUCT_API extractFileLeftPart(const UnicodeString& path, UnicodeString& result, bool allow_leading=true);

/**
	Extract the file path part after the first period.
	@returns true if the part was extracted, or false otherwise.
	@param path The file path to extract from.
	@param result This will be assigned to the right half.
	@param include_period Whether to include the first period (true by default).
	@param allow_leading Whether to allow a leading period in the path part, as in UNIX-style hidden files (true by default).
*/
bool DUCT_API extractFileRightPart(const std::string& path, std::string& result, bool include_period=true, bool allow_leading=true);
/**
	Extract the file path part after the first period.
	@returns true if the part was extracted, or false otherwise.
	@param path The file path to extract from.
	@param result This will be assigned to the right half.
	@param include_period Whether to include the first period (true by default).
	@param allow_leading Whether to allow a leading period in the path part, as in UNIX-style hidden files (true by default).
*/
bool DUCT_API extractFileRightPart(const UnicodeString& path, UnicodeString& result, bool include_period=true, bool allow_leading=true);

/**
	Get the given path's type.
	@returns The given path's type. PATHTYPE_NONE, PATHTYPE_FILE, or PATHTYPE_DIR.
	@param path The path to test.
*/
PathType DUCT_API pathType(const char* path);
/**
	Get the given path's type.
	@returns The given path's type. PATHTYPE_NONE, PATHTYPE_FILE, or PATHTYPE_DIR.
	@param path The path to test.
*/
PathType DUCT_API pathType(const std::string& path);
/**
	Get the given path's type.
	@returns The given path's type. PATHTYPE_NONE, PATHTYPE_FILE, or PATHTYPE_DIR.
	@param path The path to test.
*/
PathType DUCT_API pathType(const UnicodeString& path);

/**
	Change the current working directory.
	@returns true if the working directory was changed, or false if the working directory could not be changed to the given path.
	@param path The path to change to.
*/
bool DUCT_API changeDir(const char* path);
/**
	Change the current working directory.
	@returns true if the working directory was changed, or false if the working directory could not be changed to the given path.
	@param path The path to change to.
*/
bool DUCT_API changeDir(const std::string& path);
/**
	Change the current working directory.
	@returns true if the working directory was changed, or false if the working directory could not be changed to the given path.
	@param path The path to change to.
*/
bool DUCT_API changeDir(const UnicodeString& path);

/**
	Get the current working directory.
	The given string is unmodified if an error occurs (which is very unlikely).
	@returns true on success, or false if there was not enough free memory to allocate a string buffer.
	@param result The string to place the result in.
*/
bool DUCT_API getWorkingDir(UnicodeString& result);

/**
	Get the filesize of the given file.
	@returns The size of the given file.
	@param path The file path.
*/
uint64_t DUCT_API getFileSize(const char* path);
/**
	Get the filesize of the given file.
	@returns The size of the given file.
	@param path The file path.
*/
uint64_t DUCT_API getFileSize(const std::string& path);
/**
	Get the filesize of the given file.
	@returns The size of the given file.
	@param path The file path.
*/
uint64_t DUCT_API getFileSize(const UnicodeString& path);

/**
	Check if the given directory exists.
	@returns true if the directory exists, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API dirExists(const char* path);
/**
	Check if the given directory exists.
	@returns true if the directory exists, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API dirExists(const std::string& path);
/**
	Check if the given directory exists.
	@returns true if the directory exists, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API dirExists(const UnicodeString& path);

/**
	Check if the given file exists.
	@returns true if the file exists, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API fileExists(const char* path);
/**
	Check if the given file exists.
	@returns true if the file exists, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API fileExists(const std::string& path);
/**
	Check if the given file exists.
	@returns true if the file exists, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API fileExists(const UnicodeString& path);

/**
	Create the given directory path.
	@returns true if the directory path was created, or false if the directory path could not be created.
	@param path The directory path to create.
	@param structure Whether to create the entire path structure (e.g. if the directory 'foo' does not exist in the path 'foo/bar', it will be created if structure is true).
*/
bool DUCT_API createDir(const char* path, bool structure=false);
/**
	Create the given directory path.
	@returns true if the directory path was created, or false if the directory path could not be created.
	@param path The directory path to create.
	@param structure Whether to create the entire path structure (e.g. if the directory 'foo' does not exist in the path 'foo/bar', it will be created if structure is true).
*/
bool DUCT_API createDir(const std::string& path, bool structure=false);
/**
	Create the given directory path.
	@returns true if the directory path was created, or false if the directory path could not be created.
	@param path The directory path to create.
	@param structure Whether to create the entire path structure (e.g. if the directory 'foo' does not exist in the path 'foo/bar', it will be created if structure is true).
*/
bool DUCT_API createDir(const UnicodeString& path, bool structure=false);

/**
	Create the given file.
	@returns true if the file was created, or false if the file could not be created.
	@param path The file path to create.
	@param createpath Whether to write the entire directory structure for the file path (e.g. if the directory 'foo' does not exist in the path 'foo/bar.bork', it will be created if structure is true).
*/
bool DUCT_API createFile(const char* path, bool createpath=false);
/**
	Create the given file.
	@returns true if the file was created, or false if the file could not be created.
	@param path The file path to create.
	@param createpath Whether to write the entire directory structure for the file path (e.g. if the directory 'foo' does not exist in the path 'foo/bar.bork', it will be created if structure is true).
*/
bool DUCT_API createFile(const std::string& path, bool createpath=false);
/**
	Create the given file.
	@returns true if the file was created, or false if the file could not be created.
	@param path The file path to create.
	@param createpath Whether to write the entire directory structure for the file path (e.g. if the directory 'foo' does not exist in the path 'foo/bar.bork', it will be created if structure is true).
*/
bool DUCT_API createFile(const UnicodeString& path, bool createpath=false);

/**
	Delete the given file.
	@returns true if the file was deleted, or false if it was not.
	@param path The path of the file to remove.
*/
bool DUCT_API deleteFile(const char* path);
/**
	Delete the given file.
	@returns true if the file was deleted, or false if it was not.
	@param path The path of the file to remove.
*/
bool DUCT_API deleteFile(const std::string& path);
/**
	Delete the given file.
	@returns true if the file was deleted, or false if it was not.
	@param path The path of the file to remove.
*/
bool DUCT_API deleteFile(const UnicodeString& path);

/**
	Delete the given directory.
	@returns true if the directory was deleted, or false if it was not.
	@param path The directory to remove.
*/
bool DUCT_API deleteDir(const char* path);
/**
	Delete the given directory.
	@returns true if the directory was deleted, or false if it was not.
	@param path The directory to remove.
*/
bool DUCT_API deleteDir(const std::string& path);
/**
	Delete the given directory.
	@returns true if the directory was deleted, or false if it was not.
	@param path The directory to remove.
*/
bool DUCT_API deleteDir(const UnicodeString& path);

} // namespace FileSystem

} // namespace duct

#endif // _DUCT_FILESYSTEM_HPP

