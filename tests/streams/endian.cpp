
//#define DUCT_DYNAMIC

#include "duct/debug.hpp"
#include "duct/filestream.hpp"
#include "duct/endianstream.hpp"

using namespace std;
using namespace duct;

int main() {
	{
		FileStream out("data/littleendian", false, true);
		out.writeShort(0xa0b0);
		{
			EndianStream stream(&out, false, DUCT_LITTLE_ENDIAN);
			stream.writeShort(0xa1b1);
			stream.writeInt(0xa2b2c2d2);
		}
	}
	{
		FileStream out("data/bigendian", false, true);
		out.writeShort(0xa0b0);
		{
			EndianStream stream(&out, false, DUCT_BIG_ENDIAN);
			stream.writeShort(0xa1b1);
			stream.writeInt(0xa2b2c2d2);
		}
	}
	return 0;
}

