/**
@file endianstream.hpp
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

duct++ EndianStream class.
*/

#ifndef _DUCT_ENDIANSTREAM_HPP
#define _DUCT_ENDIANSTREAM_HPP

#include <duct/config.hpp>
#include <duct/streamwrapper.hpp>

namespace duct {

/**
	Endian stream.
	Note that only fixed-size read/write methods will use endian switching.
*/
class DUCT_API EndianStream : public StreamWrapper {
public:
	/**
		Constructor with stream.
		Automatically closing the wrapped stream is on by default.
		@param stream The stream to wrap.
		@param autoclose See setAutoClose().
		@param order Byte order of the stream.
	*/
	EndianStream(Stream* stream, bool autoclose=true, int order=DUCT_LITTLE_ENDIAN);
	
	virtual int16_t readInt16();
	virtual uint16_t readUInt16();
	virtual int32_t readInt32();
	virtual uint32_t readUInt32();
	virtual int64_t readInt64();
	virtual uint64_t readUInt64();
	virtual float readFloat();
	virtual double readDouble();
	
	virtual size_t writeInt16(int16_t value);
	virtual size_t writeUInt16(uint16_t value);
	virtual size_t writeInt32(int32_t value);
	virtual size_t writeUInt32(uint32_t value);
	virtual size_t writeInt64(int64_t value);
	virtual size_t writeUInt64(uint64_t value);
	virtual size_t writeFloat(float value);
	virtual size_t writeDouble(double value);
	
protected:
	/** Byte order. */
	int _order;
};

} // namespace duct

#endif // _DUCT_ENDIANSTREAM_HPP

