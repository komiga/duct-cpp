/**
@file filestream.hpp
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

duct++ FileStream class.
*/

#ifndef _DUCT_FILESTREAM_HPP
#define _DUCT_FILESTREAM_HPP

#include <duct/config.hpp>
#include <duct/stream.hpp>

namespace duct {

/**
	FileStream flags.
*/
enum FileStreamFlags {
	/** Stream is opened with the append attribute. This flag implies STREAM_WRITEABLE. */
	FILESTREAM_APPEND=__STREAM_RESERVED04
};

/**
	File stream.
	C stream for reading and writing files.
*/
class DUCT_API FileStream : public Stream {
public:
	/**
		Constructor with path.
		The file will be opened with both reading and writing modes.
		isOpen() should be called to check if the file was opened successfully.
		@param path The path to the file.
		@param encoding The character encoding to use. Default is UTF-8.
	*/
	FileStream(const char* path, const char* encoding="utf8");
	FileStream(const std::string& path, const char* encoding="utf8");
	FileStream(const icu::UnicodeString& path, const char* encoding="utf8");
	/**
		Constructor with path and modes.
		isOpen() should be called to check if the file was opened successfully.
		@param path The path to the file.
		@param writeable Whether the stream can be written to.
		@param readable Whether the stream can be read from.
		@param encoding The character encoding to use. Default is UTF-8.
	*/
	FileStream(const char* path, bool readable, bool writeable, const char* encoding="utf8");
	FileStream(const std::string& path, bool readable, bool writeable, const char* encoding="utf8");
	FileStream(const icu::UnicodeString& path, bool readable, bool writeable, const char* encoding="utf8");
	/**
		Constructor with path and flags.
		isOpen() should be called to check if the file was opened successfully.
		@param path The path to the file.
		@param flags A combination of #Flags values.
		@param encoding The character encoding to use. Default is UTF-8.
	*/
	FileStream(const char* path, unsigned int flags, const char* encoding="utf8");
	FileStream(const std::string& path, unsigned int flags, const char* encoding="utf8");
	FileStream(const icu::UnicodeString& path, unsigned int flags, const char* encoding="utf8");
	/**
		Destructor.
		The stream will be closed at deconstruction, though this should not be relied upon.
	*/
	~FileStream();
	/**
		Check if the stream is open.
		This is different from eof() -- which will check if the stream is closed <em>or</em> if it is at the end of the file.
		@returns true if the stream is open, or false if it is not.
	*/
	bool isOpen() const;
	
	virtual size_t read(void* data, size_t size);
	virtual size_t write(const void* data, size_t size);
	virtual void flush();
	virtual bool eof() const;
	virtual size_t size() const;
	virtual unsigned long pos() const;
	virtual unsigned long seek(unsigned long pos);
	virtual void close();
	
	/*
		Scan and read the items in the given format.
		This is a cover for fscanf().
		@returns The number of items successfully read, or EOF if there is not enough data left in the stream.
		@param format The format string.
		@param ... Variadic arguments.
	*/
	//int scanf(const char* format, ...);
	/**
		Set the stream's flags.
		<em>NOTE: This function does nothing for FileStream. The flags cannot be changed after the stream is opened.</em>
		@returns Nothing.
		@param flags A combination of #Flags values (and possibly custom flags from a deriving class).
	*/
	virtual void setFlags(unsigned int flags);
	/**
		Open the given file with the given modes.
		@returns A new FileStream, or NULL if an error occurred.
		@param path The file path to open.
		@param readable Open as readable.
		@param writeable Open as writeable.
		@param encoding The character encoding to use. Default is UTF-8.
	*/
	static FileStream* openFile(const char* path, bool readable, bool writeable, const char* encoding="utf8");
	static FileStream* openFile(const std::string& path, bool readable, bool writeable, const char* encoding="utf8");
	static FileStream* openFile(const icu::UnicodeString& path, bool readable, bool writeable, const char* encoding="utf8");
	/**
		Open the given file with the flags.
		@returns A new FileStream, or NULL if an error occurred.
		@param path The file path to open.
		@param flags The flags to open the stream with.
		@param encoding The character encoding to use. Default is UTF-8.
	*/
	static FileStream* openFile(const char* path, unsigned int flags, const char* encoding="utf8");
	static FileStream* openFile(const std::string& path, unsigned int flags, const char* encoding="utf8");
	static FileStream* openFile(const icu::UnicodeString& path, unsigned int flags, const char* encoding="utf8");
	/**
		Open the given file as readable.
		@returns A new FileStream, or NULL if an error occurred.
		@param path The file path to open.
		@param encoding The character encoding to use. Default is UTF-8.
	*/
	static FileStream* readFile(const char* path, const char* encoding="utf8");
	static FileStream* readFile(const std::string& path, const char* encoding="utf8");
	static FileStream* readFile(const icu::UnicodeString& path, const char* encoding="utf8");
	/**
		Open the given file as writeable.
		@returns A new FileStream, or NULL if an error occurred.
		@param path The file path to open.
		@param encoding The character encoding to use. Default is UTF-8.
	*/
	static FileStream* writeFile(const char* path, const char* encoding="utf8");
	static FileStream* writeFile(const std::string& path, const char* encoding="utf8");
	static FileStream* writeFile(const icu::UnicodeString& path, const char* encoding="utf8");
	
protected:
	FILE* _file;			// C FILE pointer
	unsigned long _pos;		// Stream position (cached and handled internally)
	size_t _size;			// Stream size (cached and handled internally)
	
	/** Initialize the stream with a file path and flags. */
	void init(const char* path, unsigned int flags);
	/** Initialize the stream with a FILE pointer and flags. */
	virtual void init(FILE* file, unsigned int flags);
	
private:
	FileStream();
};

} // namespace duct

#endif // _DUCT_FILESTREAM_HPP
