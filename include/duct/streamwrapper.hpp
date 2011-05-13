/**
@file streamwrapper.hpp
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

duct++ StreamWrapper class.
*/

#ifndef _DUCT_STREAMWRAPPER_HPP
#define _DUCT_STREAMWRAPPER_HPP

#include <duct/config.hpp>
#include <duct/stream.hpp>

namespace duct {

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
	virtual size_t write(const void* data, size_t size);
	
	virtual char readByte();
	virtual short readShort();
	virtual int readInt();
	virtual float readFloat();
	
	virtual UChar32 readChar();
	virtual size_t readString(UnicodeString& str, size_t length);
	virtual size_t readLine(UnicodeString& str);
	virtual size_t readCString(UnicodeString& str, size_t maxlength);
	
	virtual void writeByte(char value);
	virtual void writeShort(short value);
	virtual void writeInt(int value);
	virtual void writeFloat(float value);
	
	virtual size_t writeChar16(UChar value);
	virtual size_t writeChar32(UChar32 value);
	virtual size_t writeString(const UnicodeString& str);
	virtual size_t writeLine(const UnicodeString& str);
	virtual size_t writeCString(const UnicodeString& str);
	
	virtual void flush();
	virtual bool eof() const;
	virtual size_t size() const;
	virtual unsigned long pos() const;
	virtual unsigned long seek(unsigned long pos);
	virtual unsigned long skip(long change);
	virtual void close();
	
	virtual void setFlags(unsigned int flags);
	virtual unsigned int getFlags() const;
	
	virtual bool setEncoding(const char* encoding);
	virtual bool setEncoding(const UnicodeString& encoding);
	virtual const char* getEncoding() const;
	
	virtual UConverter* getConv();
	virtual void closeConv();
	
protected:
	/** The wrapped stream. */
	Stream* _stream;
	/** Used to tell the destructor to close the wrapped stream. */
	bool _autoclose;
	
};

} // namespace duct

#endif // _DUCT_STREAMWRAPPER_HPP

