
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
		out.writeByte((char)0xa0);
		out.writeShort((short)0xa1b1);
		out.writeInt(0xa2b2c2d2);
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
		char a=in.readByte();
		short b=in.readShort();
		int c=in.readInt();
		std::cout<<str<<std::endl;
		printf("%d (%d), %d (%d), %d (%u)\n", a, (unsigned char)a, b, (unsigned short)b, c, (unsigned int)c);
		printf("in.eof()==%s\n", in.eof() ? "true" : "false");
		// in.read will return 0, because no bytes were read.
		// Also, in debug mode an assertion will fail.
		//char x;
		//printf("in.read(&x, 1)=%d\n", in.read(&x, 1));
		in.close();
	}
	
	return 0;
}

