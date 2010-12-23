
#include "duct/debug.hpp"
#include "duct/bufferstream.hpp"

using namespace std;
using namespace duct;

int main() {
	unsigned char buffer[8];
	BufferStream stream(buffer, 8);
	stream.writeShort(0xa0b0);
	stream.writeShort(0xa1b1);
	stream.writeInt(0xa2b2c2d2);
	stream.seek(0);
	short a = stream.readShort();
	short b = stream.readShort();
	int c = stream.readInt();
	printf("%hx, %hx, %x\n", a, b, c);
	for (unsigned int i = 0; i < 8; ++i) {
		printf("%hx", buffer[i]);
	}
	printf("\n");
	return 0;
}

