
#include "duct/filestream.hpp"
#include <unicode/ustream.h>
#include <stdio.h>
#include <iostream>

using namespace duct;

int main() {
	const char* file="data/foobar";
	UnicodeString str("abcd");
	
	{
		FileStream out(file, false, true);
		out.writeString(str);
		out.writeInt8(0xa0);
		out.writeInt16(0xa1b1);
		out.writeInt32(0xa2b2c2d2);
		// NOTE: FileStream::eof returns the equality of pos() and size().
		// Thus, in this case, it simply means that the stream is at
		// the end of the stream, not that the stream was closed.
		printf("out.eof()==%s  out.pos()==%lu  out.size()==%lu\n", out.eof() ? "true" : "false", out.pos(), out.size());
		out.close();
	}
	
	{
		FileStream in(file, true, false);
		in.readString(str, str.length());
		printf("in.eof()==%s  in.pos()==%lu  in.size()==%lu\n", in.eof() ? "true" : "false", in.pos(), in.size());
		int8_t a=in.readInt8();
		int16_t b=in.readInt16();
		int32_t c=in.readInt32();
		std::cout<<str<<std::endl;
		printf("%d (%d), %d (%d), %d (%u)\n", a, (uint8_t)a, b, (uint16_t)b, c, (uint32_t)c);
		printf("in.eof()==%s\n", in.eof() ? "true" : "false");
		// in.read will return 0, because no bytes were read.
		// Also, in debug mode an assertion will fail.
		//char x;
		//printf("in.read(&x, 1)=%d\n", in.read(&x, 1));
		in.close();
	}
	
	return 0;
}

