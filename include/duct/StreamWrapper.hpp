/**
@file StreamWrapper.hpp
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

duct++ StreamWrapper class.
*/

#ifndef DUCT_STREAMWRAPPER_HPP_
#define DUCT_STREAMWRAPPER_HPP_

#include <duct/config.hpp>
#include <duct/Stream.hpp>

namespace duct {

// Forward declarations
class StreamWrapper;

/**
	Stream wrapper.
	All functions by default drop down to the wrapped stream.
*/
class DUCT_API StreamWrapper : public Stream {
public:
	/**
		Constructor.
		This will set the stream to NULL.
	*/
	StreamWrapper();
	/**
		Destructor.
		If auto close is set to true, the wrapped stream will be closed.
		@see setAutoClose()
	*/
	virtual ~StreamWrapper();
	
	/**
		Set the wrapped stream.
		If autoclose is on, the current stream will be closed.
		@returns Nothing.
		@param stream The stream to wrap.
	*/
	virtual void setStream(Stream* stream);
	/**
		Get the wrapped stream.
		@returns The wrapped stream.
	*/
	virtual Stream* getStream() const;
	
	/**
		Turn auto closing on or off.
		If auto closing is on, the wrapped stream will be closed when the wrapper is destroyed.
		@returns Nothing.
		@param autoclose On: true, off: false.
	*/
	void setAutoClose(bool autoclose);
	/**
		Get the auto closing state.
		@returns true or false for on or off, respectively.
	*/
	bool getAutoClose() const;
	
	// Base functions passing down to the wrapped stream
	virtual size_t read(void* data, size_t size);
	virtual size_t write(void const* data, size_t size);
	
	virtual int8_t readInt8();
	virtual uint8_t readUInt8();
	virtual int16_t readInt16();
	virtual uint16_t readUInt16();
	virtual int32_t readInt32();
	virtual uint32_t readUInt32();
	virtual int64_t readInt64();
	virtual uint64_t readUInt64();
	virtual float readFloat();
	virtual double readDouble();
	
	virtual UChar32 readChar();
	virtual size_t readString(icu::UnicodeString& str, size_t length);
	virtual size_t readLine(icu::UnicodeString& str);
	virtual size_t readCString(icu::UnicodeString& str, size_t maxlength);
	
	virtual size_t writeInt8(int8_t value);
	virtual size_t writeUInt8(uint8_t value);
	virtual size_t writeInt16(int16_t value);
	virtual size_t writeUInt16(uint16_t value);
	virtual size_t writeInt32(int32_t value);
	virtual size_t writeUInt32(uint32_t value);
	virtual size_t writeInt64(int64_t value);
	virtual size_t writeUInt64(uint64_t value);
	virtual size_t writeFloat(float value);
	virtual size_t writeDouble(double value);
	
	virtual size_t writeChar16(UChar value);
	virtual size_t writeChar32(UChar32 value);
	virtual size_t writeString(icu::UnicodeString const& str);
	virtual size_t writeLine(icu::UnicodeString const& str);
	virtual size_t writeCString(icu::UnicodeString const& str);
	
	virtual void flush();
	virtual bool eof() const;
	virtual size_t size() const;
	virtual unsigned long pos() const;
	virtual unsigned long seek(unsigned long pos);
	virtual unsigned long skip(long change);
	virtual void close();
	
	virtual void setFlags(unsigned int flags);
	virtual unsigned int getFlags() const;
	
	virtual bool setEncoding(char const* encoding);
	virtual bool setEncoding(icu::UnicodeString const& encoding);
	virtual char const* getEncoding() const;
	
	virtual UConverter* getConv();
	virtual void closeConv();
	
protected:
	/** The wrapped stream. */
	Stream* m_stream;
	/** Used to tell the destructor to close the wrapped stream. */
	bool m_autoclose;
};

} // namespace duct

#endif // DUCT_STREAMWRAPPER_HPP_
