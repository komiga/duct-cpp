
#include <duct/debug.hpp>
#include <duct/BufferStream.hpp>

using namespace std;
using namespace duct;

int main() {
	unsigned char buffer[8];
	BufferStream stream(buffer, 8);
	stream.writeInt16(0xa0b0);
	stream.writeInt16(0xa1b1);
	stream.writeInt32(0xa2b2c2d2);
	stream.seek(0);
	int16_t a=stream.readInt16();
	int16_t b=stream.readInt16();
	int32_t c=stream.readInt32();
	printf("%hx, %hx, %x\n", a, b, c);
	for (unsigned int i=0; i<8; ++i) {
		printf("%hx", buffer[i]);
	}
	printf("\n");
	return 0;
}

