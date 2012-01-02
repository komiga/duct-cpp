/**
@file bufferstream.hpp
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

duct++ BufferStream class.
*/

#ifndef _DUCT_BUFFERSTREAM_HPP
#define _DUCT_BUFFERSTREAM_HPP

#include <duct/config.hpp>
#include <duct/stream.hpp>

namespace duct {

/**
	Buffer stream.
	The stream is not responsible for freeing the buffer.
	The stream will not reallocate the buffer when writing data. An assertion will fail if the stream is told to write past the size of the buffer.
*/
class DUCT_API BufferStream : public Stream {
public:
	/**
		Constructor with buffer, buffer size, flags and character encoding.
		@param buffer The buffer to read/write from/to.
		@param size The size of the given buffer.
		@param flags The stream flags. Default is STREAM_READABLE|STREAM_WRITABLE.
		@param encoding The character encoding to use. Default is UTF8.
	*/
	BufferStream(void* buffer, size_t size, unsigned int flags=STREAM_READABLE|STREAM_WRITEABLE, char const* encoding="utf8");
	
	/**
		Set the stream's buffer and size.
		@returns Nothing.
		@param buffer The stream's new buffer.
		@param size The new size of the stream.
	*/
	void setBuffer(void* buffer, size_t size);
	/**
		Get the stream's buffer.
		@returns The stream's buffer.
	*/
	void* getBuffer();
	
	virtual size_t read(void* data, size_t size);
	virtual size_t write(void const* data, size_t size);
	virtual void flush();
	virtual bool eof() const;
	virtual size_t size() const;
	virtual unsigned long pos() const;
	virtual unsigned long seek(unsigned long pos);
	virtual void close();
	
protected:
	void* _buffer;			// Buffer
	unsigned long _pos;		// Stream position
	size_t _size;			// Stream size
	
private:
	BufferStream();
};

/**
	Read-only buffer stream.
	The stream is not responsible for freeing the buffer.
*/
class DUCT_API ReadOnlyBufferStream : public Stream {
public:
	/**
		Constructor with buffer, buffer size, flags and character encoding.
		@param buffer The buffer to read from.
		@param size The size of the given buffer.
		@param flags The stream flags. Default is STREAM_READABLE.
		@param encoding The character encoding to use. Default is UTF8.
	*/
	ReadOnlyBufferStream(void const* buffer, size_t size, unsigned int flags=STREAM_READABLE, char const* encoding="utf8");
	
	/**
		Set the stream's buffer and size.
		@returns Nothing.
		@param buffer The stream's new buffer.
		@param size The new size of the stream.
	*/
	void setBuffer(void const* buffer, size_t size);
	/**
		Get the stream's buffer.
		@returns The stream's buffer.
	*/
	void const* getBuffer();
	
	virtual size_t read(void* data, size_t size);
	virtual size_t write(void const* data, size_t size);
	virtual void flush();
	virtual bool eof() const;
	virtual size_t size() const;
	virtual unsigned long pos() const;
	virtual unsigned long seek(unsigned long pos);
	virtual void close();
	virtual void setFlags(unsigned int flags);
	
protected:
	void const* _buffer;	// Buffer
	unsigned long _pos;		// Stream position
	size_t _size;			// Stream size
	
private:
	ReadOnlyBufferStream();
};

} // namespace duct

#endif // _DUCT_BUFFERSTREAM_HPP

