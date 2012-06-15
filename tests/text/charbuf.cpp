
#include <duct/char.hpp>
#include <duct/string.hpp>
#include <duct/CharBuf.hpp>

#include <iostream>
#include <string>

using duct::char32;
using duct::u8string;
using duct::CharBuf;

int main(int argc, char* argv[]) {
	CharBuf buf;
	buf.push_back(0x3042);
	std::cout<<"size: "<<buf.get_size()<<"  capacity: "<<buf.get_capacity()<<"  str: \""<<buf.to_string<u8string>()<<'\"'<<std::endl;

	std::wstring wstr;
	buf.to_string(wstr);
	std::wcout<<"wide: \""<<wstr<<"\""<<std::endl; // FIXME: 'B'!?
	return 0;
}
