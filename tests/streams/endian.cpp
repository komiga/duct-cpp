
//#define DUCT_DYNAMIC

#include <duct/debug.hpp>
#include <duct/filestream.hpp>
#include <duct/endianstream.hpp>

using namespace std;
using namespace duct;

int main() {
	{
		FileStream out("data/littleendian", false, true);
		out.writeInt16(0xa0b0);
		{
			EndianStream stream(&out, false, DUCT_LITTLE_ENDIAN);
			stream.writeInt16(0xa1b1);
			stream.writeInt32(0xa2b2c2d2);
			stream.writeFloat(12.34f);
		}
	}
	{
		FileStream out("data/bigendian", false, true);
		out.writeInt16(0xa0b0);
		{
			EndianStream stream(&out, false, DUCT_BIG_ENDIAN);
			stream.writeInt16(0xa1b1);
			stream.writeInt32(0xa2b2c2d2);
			stream.writeFloat(12.34f);
		}
	}
	return 0;
}

