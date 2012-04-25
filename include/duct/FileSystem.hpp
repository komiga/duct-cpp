/**
@file FileSystem.hpp
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

@section DESCRIPTION

duct++ file system helpers.
*/

#ifndef DUCT_FILESYSTEM_HPP_
#define DUCT_FILESYSTEM_HPP_

#include <duct/config.hpp>

#include <sys/stat.h>
#include <string>
#include <unicode/unistr.h>
#ifdef DUCT_PLATFORM_WINDOWS
#include <duct/windows/dirent.h>
#include <direct.h>
#include <io.h>
#else
#include <dirent.h>
#endif

namespace duct {

// Forward declarations
//enum PathType;
class DirStream;

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
	DirStream(char const* path);
	/**
		Constructor with path.
		isOpen() should be called to check if the stream was opened successfully.
		@param path The path to open.
		@see isOpen()
	*/
	DirStream(std::string const& path);
	/**
		Constructor with path.
		isOpen() should be called to check if the stream was opened successfully.
		@param path The path to open.
		@see isOpen()
	*/
	DirStream(icu::UnicodeString const& path);
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
	bool nextEntry(icu::UnicodeString& result);
	/**
		Get the current entry's name from the given directory stream.
		@returns true if the result string was set, or false the current entry was invalid.
		@param result Output string. Will not be cleared if the current entry is invalid.
		@see entryType()
	*/
	bool entryName(icu::UnicodeString& result) const;
	/**
		Check if the current entry is either the directory's parent ("..") or relative to the directory (".").
		@returns true if the current entry is the directory's parent or relative to the directory, false otherwise.
	*/
	bool isEntryParentOrRelative() const;
	/**
		Check if the stream currently has an entry.
		@returns true if the stream currently has an entry, or false if it does not.
		@param name description.
	*/
	bool hasEntry() const;
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
	void init();
	
private:
	std::string m_path;
	DIR* m_dir;
	dirent* m_entry;
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
//bool DUCT_API statPath(char const* path, struct stat* s);
/*
	Get stats on the given path.
	@returns true if the stat() function was successful, or false if it was not.
	@param path The path to get stats on.
	@param s The pointer to store the result.
*/
//bool DUCT_API statPath(std::string const& path, struct stat* s);
/*
	Get stats on the given path.
	@returns true if the stat() function was successful, or false if it was not.
	@param path The path to get stats on.
	@param s The pointer to store the result.
*/
//bool DUCT_API statPath(icu::UnicodeString const& path, struct stat* s);

/**
	Normalize the given path. This will replace all occurrences of <em>'\\'</em> with <em>'/'</em>.
	Any trailing slashes are kept as-is.
	@returns Nothing.
	@param path The path to normalize.
*/
void DUCT_API normalizePath(std::string& path);
/**
	Normalize the given path. This will replace all occurrences of <em>'\\'</em> with <em>'/'</em>.
	Any trailing slashes are kept as-is.
	@returns Nothing.
	@param path The path to normalize.
*/
void DUCT_API normalizePath(icu::UnicodeString& path);

/**
	Normalize the given path. This will replace all occurrences of <em>'\\'</em> with <em>'/'</em>.
	@returns Nothing.
	@param path The path to normalize.
	@param trailing_slash Whether to force a trailing slash or to remove the existing one.
*/
void DUCT_API normalizePath(std::string& path, bool trailing_slash);
/**
	Normalize the given path. This will replace all occurrences of <em>'\\'</em> with <em>'/'</em>.
	@returns Nothing.
	@param path The path to normalize.
	@param trailing_slash Whether to force a trailing slash or to remove the existing one.
*/
void DUCT_API normalizePath(icu::UnicodeString& path, bool trailing_slash);

/**
	Normalize the given path. This will replace all occurrences of <em>'\\'</em> with <em>'/'</em>.
	Any trailing slashes are kept as-is.
	@returns Nothing.
	@param path The path to normalize.
	@param result The resulting string.
*/
void DUCT_API normalizePath(std::string const& path, std::string& result);
/**
	Normalize the given path. This will replace all occurrences of <em>'\\'</em> with <em>'/'</em>.
	Any trailing slashes are kept as-is.
	@returns Nothing.
	@param path The path to normalize.
	@param result The resulting string.
*/
void DUCT_API normalizePath(icu::UnicodeString const& path, icu::UnicodeString& result);

/**
	Normalize the given path. This will replace all occurrences of <em>'\\'</em> with <em>'/'</em>.
	@returns Nothing.
	@param path The path to normalize.
	@param result The resulting string.
	@param trailing_slash Whether to force a trailing slash or to remove the existing one.
*/
void DUCT_API normalizePath(std::string const& path, std::string& result, bool trailing_slash);
/**
	Normalize the given path. This will replace all occurrences of <em>'\\'</em> with <em>'/'</em>.
	@returns Nothing.
	@param path The path to normalize.
	@param result The resulting string.
	@param trailing_slash Whether to force a trailing slash or to remove the existing one.
*/
void DUCT_API normalizePath(icu::UnicodeString const& path, icu::UnicodeString& result, bool trailing_slash);

/**
	Check if the given path has a trailing slash.
	@returns true if the given path has a trailing slash, or false if the path does not have a trailing slash.
	@param path The path to test.
*/
bool DUCT_API pathHasTrailingSlash(std::string const& path);
/**
	Check if the given path has a trailing slash.
	@returns true if the given path has a trailing slash, or false if the path does not have a trailing slash.
	@param path The path to test.
*/
bool DUCT_API pathHasTrailingSlash(icu::UnicodeString const& path);

/**
	Check if the given path has a file name.
	@returns true if the given path appears to have a filename, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API pathHasFilename(std::string const& path);
/**
	Check if the given path has a file name.
	@returns true if the given path appears to have a filename, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API pathHasFilename(icu::UnicodeString const& path);

/**
	Check if the given path has an extension.
	@returns true if the given path has an extension, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API pathHasExtension(std::string const& path);
/**
	Check if the given path has an extension.
	@returns true if the given path has an extension, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API pathHasExtension(icu::UnicodeString const& path);

/**
	Check if the given path has a left part.
	@returns true if the given path has a left part, or false if it does not.
	@param path The path to test.
	@param allow_leading Whether to allow a leading period in the path part, as in UNIX-style hidden files (true by default).
*/
bool DUCT_API pathHasLeftPart(std::string const& path, bool allow_leading=true);
/**
	Check if the given path has a left part.
	@returns true if the given path has a left part, or false if it does not.
	@param path The path to test.
	@param allow_leading Whether to allow a leading period in the path part, as in UNIX-style hidden files (true by default).
*/
bool DUCT_API pathHasLeftPart(icu::UnicodeString const& path, bool allow_leading=true);

/**
	Check if the given path has a right part.
	@returns true if the given path has a right part, or false if it does not.
	@param path The path to test.
	@param allow_leading Whether to allow a leading period in the path part, as in UNIX-style hidden files (true by default).
*/
bool DUCT_API pathHasRightPart(std::string const& path, bool allow_leading=true);
/**
	Check if the given path has a right part.
	@returns true if the given path has a right part, or false if it does not.
	@param path The path to test.
	@param allow_leading Whether to allow a leading period in the path part, as in UNIX-style hidden files (true by default).
*/
bool DUCT_API pathHasRightPart(icu::UnicodeString const& path, bool allow_leading=true);

/**
	Check if the given path has a directory path.
	@returns true if the given path has a directory path, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API pathHasDirectory(std::string const& path);
/**
	Check if the given path has a directory path.
	@returns true if the given path has a directory path, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API pathHasDirectory(icu::UnicodeString const& path);

/**
	Extract the extension from the given file path.
	@returns true if the extension was extracted, or false if the path doesn't have an extension.
	@param path The path to extract from.
	@param result This will be assigned to the extension if the path contains one.
	@param include_period Whether to include the period in the extension (false by default).
*/
bool DUCT_API extractFileExtension(std::string const& path, std::string& result, bool include_period=false);
/**
	Extract the extension from the given file path.
	@returns true if the extension was extracted, or false if the path doesn't have an extension.
	@param path The path to extract from.
	@param result This will be assigned to the extension if the path contains one.
	@param include_period Whether to include the period in the extension (false by default).
*/
bool DUCT_API extractFileExtension(icu::UnicodeString const& path, icu::UnicodeString& result, bool include_period=false);

/**
	Extract the directory path from the given file path.
	@returns true if the given file path has a directory path, or false if it does not.
	@param path The file path to extract from.
	@param result This will be assigned to the directory if the path contains one.
	@param trailing_slash Whether to append a trailing slash to the directory path.
*/
bool DUCT_API extractFileDirectory(std::string const& path, std::string& result, bool trailing_slash=true);
/**
	Extract the directory path from the given file path.
	@returns true if the given file path has a directory path, or false if it does not.
	@param path The file path to extract from.
	@param result This will be assigned to the directory if the path contains one.
	@param trailing_slash Whether to append a trailing slash to the directory path.
*/
bool DUCT_API extractFileDirectory(icu::UnicodeString const& path, icu::UnicodeString& result, bool trailing_slash=true);

/**
	Extract the file name from the given file path.
	@returns true if the given path contains a file name, or false if it does not (for example, "/some/directory/path/").
	@param path The file path to extract from.
	@param result This will be assigned to the filename.
	@param with_extension Whether to include the extension.
*/
bool DUCT_API extractFilename(std::string const& path, std::string& result, bool with_extension);
/**
	Extract the file name from the given file path.
	@returns true if the given path contains a file name, or false if it does not (for example, "/some/directory/path/").
	@param path The file path to extract from.
	@param result This will be assigned to the filename.
	@param with_extension Whether to include the extension.
*/
bool DUCT_API extractFilename(icu::UnicodeString const& path, icu::UnicodeString& result, bool with_extension);

/**
	Extract the file path part before the first period.
	@returns true if the part was extracted, or false otherwise.
	@param path The file path to extract from.
	@param result This will be assigned to the right half.
	@param allow_leading Whether to allow a leading period in the path part, as in UNIX-style hidden files (true by default).
*/
bool DUCT_API extractFileLeftPart(std::string const& path, std::string& result, bool allow_leading=true);
/**
	Extract the file path part before the first period.
	@returns true if the part was extracted, or false otherwise.
	@param path The file path to extract from.
	@param result This will be assigned to the right half.
	@param allow_leading Whether to allow a leading period in the path part, as in UNIX-style hidden files (true by default).
*/
bool DUCT_API extractFileLeftPart(icu::UnicodeString const& path, icu::UnicodeString& result, bool allow_leading=true);

/**
	Extract the file path part after the first period.
	@returns true if the part was extracted, or false otherwise.
	@param path The file path to extract from.
	@param result This will be assigned to the right half.
	@param include_period Whether to include the first period (true by default).
	@param allow_leading Whether to allow a leading period in the path part, as in UNIX-style hidden files (true by default).
*/
bool DUCT_API extractFileRightPart(std::string const& path, std::string& result, bool include_period=true, bool allow_leading=true);
/**
	Extract the file path part after the first period.
	@returns true if the part was extracted, or false otherwise.
	@param path The file path to extract from.
	@param result This will be assigned to the right half.
	@param include_period Whether to include the first period (true by default).
	@param allow_leading Whether to allow a leading period in the path part, as in UNIX-style hidden files (true by default).
*/
bool DUCT_API extractFileRightPart(icu::UnicodeString const& path, icu::UnicodeString& result, bool include_period=true, bool allow_leading=true);

/**
	Get the given path's type.
	@returns The given path's type. PATHTYPE_NONE, PATHTYPE_FILE, or PATHTYPE_DIR.
	@param path The path to test.
*/
PathType DUCT_API pathType(char const* path);
/**
	Get the given path's type.
	@returns The given path's type. PATHTYPE_NONE, PATHTYPE_FILE, or PATHTYPE_DIR.
	@param path The path to test.
*/
PathType DUCT_API pathType(std::string const& path);
/**
	Get the given path's type.
	@returns The given path's type. PATHTYPE_NONE, PATHTYPE_FILE, or PATHTYPE_DIR.
	@param path The path to test.
*/
PathType DUCT_API pathType(icu::UnicodeString const& path);

/**
	Change the current working directory.
	@returns true if the working directory was changed, or false if the working directory could not be changed to the given path.
	@param path The path to change to.
*/
bool DUCT_API changeDir(char const* path);
/**
	Change the current working directory.
	@returns true if the working directory was changed, or false if the working directory could not be changed to the given path.
	@param path The path to change to.
*/
bool DUCT_API changeDir(std::string const& path);
/**
	Change the current working directory.
	@returns true if the working directory was changed, or false if the working directory could not be changed to the given path.
	@param path The path to change to.
*/
bool DUCT_API changeDir(icu::UnicodeString const& path);

/**
	Get the current working directory.
	The result string is unmodified if an error occurs (which is very unlikely).
	@returns true on success, or false if there was an error.
	@param result The resulting path. This will be set to the working directory.
*/
bool DUCT_API getWorkingDir(std::string& result);
/**
	Get the current working directory.
	The result string is unmodified if an error occurs (which is very unlikely).
	@returns true on success, or false if there was an error.
	@param result The resulting path. This will be set to the working directory.
*/
bool DUCT_API getWorkingDir(icu::UnicodeString& result);

/**
	Get the current working directory and normalize the path.
	The result string is unmodified if an error occurs (which is very unlikely).
	@returns true on success, or false if there was an error.
	@param result The resulting path. This will be set to the working directory.
	@param trailing_slash Whether to force a trailing slash or to remove the existing one.
*/
bool DUCT_API getWorkingDir(std::string& result, bool trailing_slash);
/**
	Get the current working directory and normalize the path.
	The result string is unmodified if an error occurs (which is very unlikely).
	@returns true on success, or false if there was an error.
	@param result The resulting path. This will be set to the working directory.
	@param trailing_slash Whether to force a trailing slash or to remove the existing one.
*/
bool DUCT_API getWorkingDir(icu::UnicodeString& result, bool trailing_slash);

/**
	Get the absolute path for the given path.
	This does not guarantee that the resulting path actually exists.
	The result string is unmodified if the given path is 0-length or if the working directory could not be retrieved.
	@returns Nothing.
	@param path The path to absoluteify.
	@param result The resulting absolute path.
*/
void DUCT_API getAbsolutePath(std::string const& path, std::string& result);
/**
	Get the absolute path for the given path.
	This does not guarantee that the resulting path actually exists.
	The result string is unmodified if the given path is 0-length or if the working directory could not be retrieved.
	@returns Nothing.
	@param path The path to absoluteify.
	@param result The resulting absolute path.
*/
void DUCT_API getAbsolutePath(icu::UnicodeString const& path, icu::UnicodeString& result);

/**
	Get the absolute path for the given path, and store the result in the given string.
	This does not guarantee that the resulting path actually exists.
	The result string is unmodified if the given path is 0-length or if the working directory could not be retrieved.
	@returns Nothing.
	@param path The path to absoluteify and to store the result in.
*/
void DUCT_API getAbsolutePath(std::string& path);
/**
	Get the absolute path for the given path, and store the result in the given string.
	This does not guarantee that the resulting path actually exists.
	The result string is unmodified if the given path is 0-length or if the working directory could not be retrieved.
	@returns Nothing.
	@param path The path to absoluteify and to store the result in.
*/
void DUCT_API getAbsolutePath(icu::UnicodeString& path);

/**
	Get the absolute normalized path for the given path.
	This does not guarantee that the resulting path actually exists.
	The result string is unmodified if the given path is 0-length or if the working directory could not be retrieved.
	@returns Nothing.
	@param path The path to absoluteify.
	@param result The resulting absolute path.
*/
void DUCT_API getAbsolutePathNormalized(std::string const& path, std::string& result);
/**
	Get the absolute normalized path for the given path.
	This does not guarantee that the resulting path actually exists.
	The result string is unmodified if the given path is 0-length or if the working directory could not be retrieved.
	@returns Nothing.
	@param path The path to absoluteify.
	@param result The resulting absolute path.
*/
void DUCT_API getAbsolutePathNormalized(icu::UnicodeString const& path, icu::UnicodeString& result);

/**
	Get the absolute normalized path for the given path.
	This does not guarantee that the resulting path actually exists.
	The result string is unmodified if the given path is 0-length or if the working directory could not be retrieved.
	@returns Nothing.
	@param path The path to absoluteify.
	@param result The resulting absolute path.
	@param trailing_slash Whether to force a trailing slash or to remove the existing one.
*/
void DUCT_API getAbsolutePathNormalized(std::string const& path, std::string& result, bool trailing_slash);
/**
	Get the absolute normalized path for the given path.
	This does not guarantee that the resulting path actually exists.
	The result string is unmodified if the given path is 0-length or if the working directory could not be retrieved.
	@returns Nothing.
	@param path The path to absoluteify.
	@param result The resulting absolute path.
	@param trailing_slash Whether to force a trailing slash or to remove the existing one.
*/
void DUCT_API getAbsolutePathNormalized(icu::UnicodeString const& path, icu::UnicodeString& result, bool trailing_slash);

/**
	Get the absolute normalized path for the given path, and store the result in the given string.
	This does not guarantee that the resulting path actually exists.
	The result string is unmodified if the given path is 0-length or if the working directory could not be retrieved.
	@returns Nothing.
	@param path The path to absoluteify and to store the result in.
*/
void DUCT_API getAbsolutePathNormalized(std::string& path);
/**
	Get the absolute normalized path for the given path, and store the result in the given string.
	This does not guarantee that the resulting path actually exists.
	The result string is unmodified if the given path is 0-length or if the working directory could not be retrieved.
	@returns Nothing.
	@param path The path to absoluteify and to store the result in.
*/
void DUCT_API getAbsolutePathNormalized(icu::UnicodeString& path);

/**
	Get the absolute normalized path for the given path, and store the result in the given string.
	This does not guarantee that the resulting path actually exists.
	The result string is unmodified if the given path is 0-length or if the working directory could not be retrieved.
	@returns Nothing.
	@param path The path to absoluteify and to store the result in.
	@param trailing_slash Whether to force a trailing slash or to remove the existing one.
*/
void DUCT_API getAbsolutePathNormalized(std::string& path, bool trailing_slash);
/**
	Get the absolute normalized path for the given path, and store the result in the given string.
	This does not guarantee that the resulting path actually exists.
	The result string is unmodified if the given path is 0-length or if the working directory could not be retrieved.
	@returns Nothing.
	@param path The path to absoluteify and to store the result in.
	@param trailing_slash Whether to force a trailing slash or to remove the existing one.
*/
void DUCT_API getAbsolutePathNormalized(icu::UnicodeString& path, bool trailing_slash);

/**
	Resolve the given path to an absolute path, following symbolic links and such.
	Under a POSIX-compliant system, this will only resolve the path if it exists.
	The result string is unmodified if an error occurs (which is very unlikely).
	@returns true on success, or false if there was an error.
	@param path The path to resolve.
	@param result The resulting absolute path.
*/
bool DUCT_API resolvePath(std::string const& path, std::string& result);
/**
	Resolve the given path to an absolute path, following symbolic links and such.
	Under a POSIX-compliant system, this will only resolve the path if it exists.
	The result string is unmodified if an error occurs (which is very unlikely).
	@returns true on success, or false if there was an error.
	@param path The path to resolve.
	@param result The resulting absolute path.
*/
bool DUCT_API resolvePath(icu::UnicodeString const& path, icu::UnicodeString& result);

/**
	Resolve the given path to an absolute path, following symbolic links and such, and store the result in the given string.
	Under a POSIX-compliant system, this will only resolve the path if it exists.
	The given string is unmodified if an error occurs (which is very unlikely).
	@returns true on success, or false if there was an error.
	@param path The path to resolve and to store the result in.
*/
bool DUCT_API resolvePath(std::string& path);
/**
	Resolve the given path to an absolute path, following symbolic links and such, and store the result in the given string.
	Under a POSIX-compliant system, this will only resolve the path if it exists.
	The given string is unmodified if an error occurs (which is very unlikely).
	@returns true on success, or false if there was an error.
	@param path The path to resolve and to store the result in.
*/
bool DUCT_API resolvePath(icu::UnicodeString& path);

/**
	Resolve the given path to a normalized absolute path, following symbolic links and such.
	Under a POSIX-compliant system, this will only resolve the path if it exists.
	The result string is unmodified if an error occurs (which is very unlikely).
	@returns true on success, or false if there was an error.
	@param path The path to resolve.
	@param result The resulting absolute path.
*/
bool DUCT_API resolvePathNormalized(std::string const& path, std::string& result);
/**
	Resolve the given path to a normalized absolute path, following symbolic links and such.
	Under a POSIX-compliant system, this will only resolve the path if it exists.
	The result string is unmodified if an error occurs (which is very unlikely).
	@returns true on success, or false if there was an error.
	@param path The path to resolve.
	@param result The resulting absolute path.
*/
bool DUCT_API resolvePathNormalized(icu::UnicodeString const& path, icu::UnicodeString& result);

/**
	Resolve the given path to a normalized absolute path, following symbolic links and such.
	Under a POSIX-compliant system, this will only resolve the path if it exists.
	The result string is unmodified if an error occurs (which is very unlikely).
	@returns true on success, or false if there was an error.
	@param path The path to resolve.
	@param result The resulting absolute path.
	@param trailing_slash Whether to force a trailing slash or to remove the existing one.
*/
bool DUCT_API resolvePathNormalized(std::string const& path, std::string& result, bool trailing_slash);
/**
	Resolve the given path to a normalized absolute path, following symbolic links and such.
	Under a POSIX-compliant system, this will only resolve the path if it exists.
	The result string is unmodified if an error occurs (which is very unlikely).
	@returns true on success, or false if there was an error.
	@param path The path to resolve.
	@param result The resulting absolute path.
	@param trailing_slash Whether to force a trailing slash or to remove the existing one.
*/
bool DUCT_API resolvePathNormalized(icu::UnicodeString const& path, icu::UnicodeString& result, bool trailing_slash);

/**
	Resolve the given path to a normalized absolute path, following symbolic links and such, and store the result in the given string.
	Under a POSIX-compliant system, this will only resolve the path if it exists.
	The given string is unmodified if an error occurs (which is very unlikely).
	@returns true on success, or false if there was an error.
	@param path The path to resolve and to store the result in.
*/
bool DUCT_API resolvePathNormalized(std::string& path);
/**
	Resolve the given path to a normalized absolute path, following symbolic links and such, and store the result in the given string.
	Under a POSIX-compliant system, this will only resolve the path if it exists.
	The given string is unmodified if an error occurs (which is very unlikely).
	@returns true on success, or false if there was an error.
	@param path The path to resolve and to store the result in.
*/
bool DUCT_API resolvePathNormalized(icu::UnicodeString& path);

/**
	Resolve the given path to a normalized absolute path, following symbolic links and such, and store the result in the given string.
	Under a POSIX-compliant system, this will only resolve the path if it exists.
	The given string is unmodified if an error occurs (which is very unlikely).
	@returns true on success, or false if there was an error.
	@param path The path to resolve and to store the result in.
	@param trailing_slash Whether to force a trailing slash or to remove the existing one.
*/
bool DUCT_API resolvePathNormalized(std::string& path, bool trailing_slash);
/**
	Resolve the given path to a normalized absolute path, following symbolic links and such, and store the result in the given string.
	Under a POSIX-compliant system, this will only resolve the path if it exists.
	The given string is unmodified if an error occurs (which is very unlikely).
	@returns true on success, or false if there was an error.
	@param path The path to resolve and to store the result in.
	@param trailing_slash Whether to force a trailing slash or to remove the existing one.
*/
bool DUCT_API resolvePathNormalized(icu::UnicodeString& path, bool trailing_slash);

/**
	Get the filesize of the given file.
	@returns The size of the given file.
	@param path The file path.
*/
uint64_t DUCT_API getFileSize(char const* path);
/**
	Get the filesize of the given file.
	@returns The size of the given file.
	@param path The file path.
*/
uint64_t DUCT_API getFileSize(std::string const& path);
/**
	Get the filesize of the given file.
	@returns The size of the given file.
	@param path The file path.
*/
uint64_t DUCT_API getFileSize(icu::UnicodeString const& path);

/**
	Check if the given directory exists.
	@returns true if the directory exists, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API dirExists(char const* path);
/**
	Check if the given directory exists.
	@returns true if the directory exists, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API dirExists(std::string const& path);
/**
	Check if the given directory exists.
	@returns true if the directory exists, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API dirExists(icu::UnicodeString const& path);

/**
	Check if the given file exists.
	@returns true if the file exists, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API fileExists(char const* path);
/**
	Check if the given file exists.
	@returns true if the file exists, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API fileExists(std::string const& path);
/**
	Check if the given file exists.
	@returns true if the file exists, or false if it does not.
	@param path The path to test.
*/
bool DUCT_API fileExists(icu::UnicodeString const& path);

/**
	Create the given directory path.
	@returns true if the directory path was created, or false if the directory path could not be created.
	@param path The directory path to create.
	@param structure Whether to create the entire path structure (e.g. if the directory 'foo' does not exist in the path 'foo/bar', it will be created if structure is true).
*/
bool DUCT_API createDir(char const* path, bool structure=false);
/**
	Create the given directory path.
	@returns true if the directory path was created, or false if the directory path could not be created.
	@param path The directory path to create.
	@param structure Whether to create the entire path structure (e.g. if the directory 'foo' does not exist in the path 'foo/bar', it will be created if structure is true).
*/
bool DUCT_API createDir(std::string const& path, bool structure=false);
/**
	Create the given directory path.
	@returns true if the directory path was created, or false if the directory path could not be created.
	@param path The directory path to create.
	@param structure Whether to create the entire path structure (e.g. if the directory 'foo' does not exist in the path 'foo/bar', it will be created if structure is true).
*/
bool DUCT_API createDir(icu::UnicodeString const& path, bool structure=false);

/**
	Create the given file.
	@returns true if the file was created, or false if the file could not be created.
	@param path The file path to create.
	@param createpath Whether to write the entire directory structure for the file path (e.g. if the directory 'foo' does not exist in the path 'foo/bar.bork', it will be created if structure is true).
*/
bool DUCT_API createFile(char const* path, bool createpath=false);
/**
	Create the given file.
	@returns true if the file was created, or false if the file could not be created.
	@param path The file path to create.
	@param createpath Whether to write the entire directory structure for the file path (e.g. if the directory 'foo' does not exist in the path 'foo/bar.bork', it will be created if structure is true).
*/
bool DUCT_API createFile(std::string const& path, bool createpath=false);
/**
	Create the given file.
	@returns true if the file was created, or false if the file could not be created.
	@param path The file path to create.
	@param createpath Whether to write the entire directory structure for the file path (e.g. if the directory 'foo' does not exist in the path 'foo/bar.bork', it will be created if structure is true).
*/
bool DUCT_API createFile(icu::UnicodeString const& path, bool createpath=false);

/**
	Delete the given file.
	@returns true if the file was deleted, or false if it was not.
	@param path The path of the file to remove.
*/
bool DUCT_API deleteFile(char const* path);
/**
	Delete the given file.
	@returns true if the file was deleted, or false if it was not.
	@param path The path of the file to remove.
*/
bool DUCT_API deleteFile(std::string const& path);
/**
	Delete the given file.
	@returns true if the file was deleted, or false if it was not.
	@param path The path of the file to remove.
*/
bool DUCT_API deleteFile(icu::UnicodeString const& path);

/**
	Delete the given directory.
	@returns true if the directory was deleted, or false if it was not.
	@param path The directory to remove.
*/
bool DUCT_API deleteDir(char const* path);
/**
	Delete the given directory.
	@returns true if the directory was deleted, or false if it was not.
	@param path The directory to remove.
*/
bool DUCT_API deleteDir(std::string const& path);
/**
	Delete the given directory.
	@returns true if the directory was deleted, or false if it was not.
	@param path The directory to remove.
*/
bool DUCT_API deleteDir(icu::UnicodeString const& path);

} // namespace FileSystem
} // namespace duct

#endif // DUCT_FILESYSTEM_HPP_
