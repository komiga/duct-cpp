/**
@file coverstream.hpp
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

duct++ CoverStream class.
*/

#ifndef _DUCT_COVERSTREAM_HPP
#define _DUCT_COVERSTREAM_HPP

#include <duct/config.hpp>
#include <duct/stream.hpp>

namespace duct {

/**
	Cover stream.
	Covers another stream for layered processing. A CoverStream is different from StreamWrapper, where all read/write functions drop down to the wrapped stream by default.
	The class leaves both read() and write() open for derived classes, with Stream's default read/write-specific implementations (e.g. readString, writeString, readInt, writeInt), and drops down all stream-specific functions to the covered stream (e.g. eof(), size(), pos()).
	Derived classes should drop down at the read() and write() functions, by using the covered stream as a 'destination' for processed data.
*/
class DUCT_API CoverStream : public Stream {
public:
	/**
		Constructor.
		This will set the stream to NULL.
	*/
	CoverStream();
	/**
		Destructor.
		If auto close is set to true, the covered stream will be closed.
		@see setAutoClose()
	*/
	virtual ~CoverStream();
	
	/**
		Set the covered stream.
		If autoclose is on, the current stream will be closed.
		@returns Nothing.
		@param stream The stream to cover.
	*/
	virtual void setStream(Stream* stream);
	/**
		Get the covered stream.
		@returns The covered stream.
	*/
	virtual Stream* getStream() const;
	
	/**
		Turn auto closing on or off.
		If auto closing is on, the covered stream will be closed when the cover stream is destroyed.
		@returns Nothing.
		@param autoclose On: true, off: false.
	*/
	void setAutoClose(bool autoclose);
	/**
		Get the auto closing state.
		@returns true or false for on or off, respectively.
	*/
	bool getAutoClose() const;
	
	virtual size_t read(void* data, size_t size)=0;
	virtual size_t write(void const* data, size_t size)=0;
	
	// Base functions passing down to the covered stream
	virtual void flush();
	virtual bool eof() const;
	virtual size_t size() const;
	virtual unsigned long pos() const;
	virtual unsigned long seek(unsigned long pos);
	virtual unsigned long skip(long change);
	virtual void close();
	
protected:
	/** The covered stream. */
	Stream* m_stream;
	/** Used to tell the destructor to close the covered stream. */
	bool m_autoclose;
	
};

} // namespace duct

#endif // _DUCT_COVERSTREAM_HPP

