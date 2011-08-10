/**
@file stream.hpp
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

duct++ Stream class.
*/

#ifndef _DUCT_STREAM_HPP
#define _DUCT_STREAM_HPP

#include <unicode/utf.h>
#include <unicode/unistr.h>
#include <unicode/ucnv.h>
#include <duct/config.hpp>

namespace duct {

/**
	Stream flags.
	The values reserved are 0x01 through 0x10, any value further than that can be used for deriving classes' custom flags.
*/
enum StreamFlags {
	/** Stream is readable. */
	STREAM_READABLE=0x01,
	/** Stream is writeable. */
	STREAM_WRITEABLE=0x02,
	/** Reserved flag 0x04. */
	__STREAM_RESERVED04=0x04,
	/** Reserved flag 0x08. */
	__STREAM_RESERVED08=0x08,
	/** Reserved flag 0x10. */
	__STREAM_RESERVED10=0x10
};

/** Abstract class for every stream. */
class DUCT_API Stream {
public:
	/**
		Constructor.
		This will set the _conv field to NULL.
	*/
	Stream();
	/**
		Destructor.
	*/
	virtual ~Stream();
	
	/**
		Read the given number of bytes into a pointer.
		If the return value is not the given size, then an error occurred (likely end-of-stream).
		@returns The number of bytes read, which mightn't be the given size.
		@param data Data pointer.
		@param size Number of bytes to read.
	*/
	virtual size_t read(void* data, size_t size)=0;
	/**
		Write the given number of bytes from a pointer.
		If the return value is not the given size, then an error occurred (likely end-of-stream).
		@returns The number of bytes written, which mightn't be the given size.
		@param data Data pointer.
		@param size Number of bytes to write.
	*/
	virtual size_t write(const void* data, size_t size)=0;
	
	/**
		Read an 8-bit integer from the stream.
		@returns The value read from the stream.
	*/
	virtual int8_t readInt8();
	/**
		Read an unsigned 8-bit integer from the stream.
		@returns The value read from the stream.
	*/
	virtual uint8_t readUInt8();
	/**
		Read a 16-bit integer from the stream.
		@returns The value read from the stream.
	*/
	virtual int16_t readInt16();
	/**
		Read an unsigned 16-bit integer from the stream.
		@returns The value read from the stream.
	*/
	virtual uint16_t readUInt16();
	/**
		Read a 32-bit integer from the stream.
		@returns The value read from the stream.
	*/
	virtual int32_t readInt32();
	/**
		Read an unsigned 32-bit integer from the stream.
		@returns The value read from the stream.
	*/
	virtual uint32_t readUInt32();
	/**
		Read a 64-bit integer from the stream.
		@returns The value read from the stream.
	*/
	virtual int64_t readInt64();
	/**
		Read an unsigned 64-bit integer from the stream.
		@returns The value read from the stream.
	*/
	virtual uint64_t readUInt64();
	/**
		Read a float from the stream.
		The size of a float depends on the compiler/architecture.
		@returns The value read from the stream.
	*/
	virtual float readFloat();
	/**
		Read a double from the stream.
		The size of a double depends on the compiler/architecture.
		@returns The value read from the stream.
	*/
	virtual double readDouble();
	
	/**
		Read a code point from the stream.
		The number of bytes read will depend on the stream's encoding.
		@returns The code point read from the stream.
	*/
	virtual UChar32 readChar();
	/**
		Read a string from the stream.
		The number of bytes read will depend on the stream's encoding.
		@returns The number of bytes read.
		@param str The output string.
		@param length The length of the string to read.
	*/
	virtual size_t readString(UnicodeString& str, size_t length);
	/**
		Read a line from the stream (variable-length string ending in '\\n').
		The CR character is ignored, and the output will not contain any if they are encountered.
		The number of bytes read will depend on the stream's encoding.
		@returns The number of bytes read.
		@param str The output string.
	*/
	virtual size_t readLine(UnicodeString& str);
	/**
		Read a null-terminated string from the stream.
		The number of bytes read will depend on the stream's encoding.
		@returns The number of bytes read, which may be less than expected.
		@param str The output string.
		@param maxlength The maximum length of the string to read.
	*/
	virtual size_t readCString(UnicodeString& str, size_t maxlength);
	
	/**
		Write an 8-bit integer to the stream.
		If the return value is not the size of the value, then an error occurred (likely end-of-stream).
		@returns The number of bytes written.
		@param value The value to write.
	*/
	virtual size_t writeInt8(int8_t value);
	/**
		Write an unsigned 8-bit integer to the stream.
		If the return value is not the size of the value, then an error occurred (likely end-of-stream).
		@returns The number of bytes written.
		@param value The value to write.
	*/
	virtual size_t writeUInt8(uint8_t value);
	/**
		Write a 16-bit integer to the stream.
		If the return value is not the size of the value, then an error occurred (likely end-of-stream).
		@returns The number of bytes written.
		@param value The value to write.
	*/
	virtual size_t writeInt16(int16_t value);
	/**
		Write an unsigned 16-bit integer to the stream.
		If the return value is not the size of the value, then an error occurred (likely end-of-stream).
		@returns The number of bytes written.
		@param value The value to write.
	*/
	virtual size_t writeUInt16(uint16_t value);
	/**
		Write a 32-bit integer to the stream.
		If the return value is not the size of the value, then an error occurred (likely end-of-stream).
		@returns The number of bytes written.
		@param value The value to write.
	*/
	virtual size_t writeInt32(int32_t value);
	/**
		Write an unsigned 32-bit integer to the stream.
		If the return value is not the size of the value, then an error occurred (likely end-of-stream).
		@returns The number of bytes written.
		@param value The value to write.
	*/
	virtual size_t writeUInt32(uint32_t value);
	/**
		Write an 64-bit integer to the stream.
		If the return value is not the size of the value, then an error occurred (likely end-of-stream).
		@returns The number of bytes written.
		@param value The value to write.
	*/
	virtual size_t writeInt64(int64_t value);
	/**
		Write an unsigned 64-bit integer to the stream.
		If the return value is not the size of the value, then an error occurred (likely end-of-stream).
		@returns The number of bytes written.
		@param value The value to write.
	*/
	virtual size_t writeUInt64(uint64_t value);
	/**
		Write a float to the stream.
		The size of a float depends on the compiler/architecture.
		If the return value is not the size of the value, then an error occurred (likely end-of-stream).
		@returns The number of bytes written.
		@param value The value to write to the stream.
	*/
	virtual size_t writeFloat(float value);
	/**
		Write a double to the stream.
		The size of a double depends on the compiler/architecture.
		If the return value is not the size of the value, then an error occurred (likely end-of-stream).
		@returns The number of bytes written.
		@param value The value to write to the stream.
	*/
	virtual size_t writeDouble(double value);
	
	/**
		Write a code unit to the stream.
		The number of bytes written will depend on the stream's encoding.
		@returns The number of bytes written.
		@param c The code unit to write.
	*/
	virtual size_t writeChar16(UChar value);
	/**
		Write a code point to the stream.
		The number of bytes written will depend on the stream's encoding.
		@returns The number of bytes written.
		@param c The code point to write.
	*/
	virtual size_t writeChar32(UChar32 value);
	/**
		Write the given string to the stream.
		The number of bytes written will depend on the stream's encoding.
		@returns The number of bytes written.
		@param str The string to write to the stream.
	*/
	virtual size_t writeString(const UnicodeString& str);
	/**
		Write the the given string as a line (appends '\\n').
		The number of bytes written will depend on the stream's encoding.
		@returns The number of bytes written.
		@param str The string to write to the stream.
	*/
	virtual size_t writeLine(const UnicodeString& str);
	/**
		Write the given string to the stream as a null-terminated string.
		This will basically write str+'\\0' to the stream.
		The number of bytes written will depend on the stream's encoding.
		@returns The number of bytes written.
		@param str The string to write to the stream.
	*/
	virtual size_t writeCString(const UnicodeString& str);
	
	/**
		Read a null-terminated string and compare the given string against it.
		@returns true if the given string matched the read string, or false if they did not match.
		@param checkstr The string to compare with.
		@param maxlength Optional max-length for reading the C string. If maxlength is greater than 0, it will be used as the maximum length for reading the C string, otherwise the given string's length will be used (plus one for the null character).
	*/
	bool readAndMatchCString(const UnicodeString& checkstr, size_t maxlength=0);
	/**
		Read a reserved-space null-terminated string.
		@returns Nothing.
		@param result The result string.
		@param size The size of the reserved space.
	*/
	void readReservedCString(UnicodeString& result, size_t size);
	/**
		Read a reserved-space null-terminated string (limited size string) and compare the given string against it.
		@returns true if the given string matched the read string, or false if they did not match.
		@param checkstr The string to match with.
		@param size The size of the reserved space.
	*/
	bool readAndMatchReservedCString(const UnicodeString& checkstr, size_t size);
	/**
		Write reserved-area data.
		@returns Nothing.
		@param size The size of the reserved data.
		@param padvalue The value with which to pad the space.
	*/
	void writeReservedData(size_t size, unsigned char padvalue);
	/**
		Write the given string as a null-terminated string with reserved-area padding.
		@returns Nothing.
		@param str The string to write.
		@param size The size of the reserved space.
		@param padvalue The value with which to pad the space.
	*/
	void writeReservedCString(const UnicodeString& str, size_t size, unsigned char padvalue);
	
	/**
		Flush the stream.
		@returns Nothing.
	*/
	virtual void flush()=0;
	/**
		Get the end-of state.
		NOTE: For streams that are <em>writeable</em>, a return value of true can mean that either the stream was closed,
		or the writing position is at the very end of the stream (which should mean that it can still write data).
		@returns true if the stream either has ended, or is at the end of the stream, or false if it has not.
	*/
	virtual bool eof() const=0;
	/**
		Get the size of the stream.
		@returns The size of the stream.
	*/
	virtual size_t size() const=0;
	/**
		Get the stream's position.
		@returns The position of the stream.
	*/
	virtual unsigned long pos() const=0;
	/**
		Seek the stream (change the reading/writing position).
		@returns The new position of the stream.
		@param pos The position to seek to.
	*/
	virtual unsigned long seek(unsigned long pos)=0;
	/**
		Seek the stream forwards or backwards by the given value.
		@returns The new position of the stream.
		@param change Positive or negative relative change in the stream's position.
	*/
	virtual unsigned long skip(long change);
	/**
		Close the stream.
		@returns Nothing.
	*/
	virtual void close()=0;
	
	/**
		Set the stream's flags.
		@returns Nothing.
		@param flags A combination of #Flags values (and possibly custom flags from a deriving class).
	*/
	virtual void setFlags(unsigned int flags);
	/**
		Get the stream's flags.
		@returns The stream's flags (a combination of #Flags values, and possibly custom flags from a deriving class).
	*/
	virtual unsigned int getFlags() const;
	
	/**
		Set the stream's character encoding.
		If the converter fails to open, the previous converter stays valid (which may be unopened as well).
		@returns true if the converter was opened, or false if the converter was unable to be opened.
		@param encoding The stream's new character encoding. See ICU documentation.
		@see getEncoding()
	*/
	virtual bool setEncoding(const char* encoding);
	virtual bool setEncoding(const UnicodeString& encoding);
	/**
		Get the stream's character encoding.
		@returns The stream's character encoding. This may be NULL if the character converter has not been opened.
	*/
	virtual const char* getEncoding() const;
	
	/**
		Get the stream's character converter.
		@returns The stream's character converter. This may be NULL if the character converter has not been opened.
	*/
	virtual UConverter* getConv();
	virtual const UConverter* getConv() const;
	/**
		Close the stream's character converter.
		@returns Nothing.
	*/
	virtual void closeConv();
	
private:
	/**
		Read an element by size.
		This is an alias for read(&element, sizeof(_elem_t)).
		If the return value is not the size of the element, then an error occurred (likely end-of-stream).
		@returns The number of bytes read, which mightn't be the size of the element.
		@param element Reference to the element to read into.
	*/
	template <typename _elem_t>
	inline size_t read(_elem_t& element);
	/**
		Write the given element by size.
		This is an alias for write(&element, sizeof(_elem_t)).
		If the return value is not the size of the element, then an error occurred (likely end-of-stream).
		@returns The number of bytes read, which mightn't be the size of the element.
		@param element Reference to the element to write.
	*/
	template <typename _elem_t>
	inline size_t write(const _elem_t& element);
	
protected:
	/**
		Stream flags (readable, writeable, etc.)
		Combination of Flags values (and possibly other enums for deriving classes).
	*/
	unsigned int _flags;
	/**
		Character converter for string reading/writing.
	*/
	UConverter* _conv;
};

#include <duct/inline/stream.inl>

} // namespace duct

#endif // _DUCT_STREAM_HPP

