
#include <duct/char.hpp>
#include <duct/string.hpp>
#include <duct/CharBuf.hpp>

#include <string>
#include <iomanip>
#include <iostream>

signed main(signed, char*[]) {
	duct::CharBuf buf;
	buf.push_back(0x3042);
	std::cout<<"size: "<<buf.get_size()<<"  capacity: "<<buf.get_capacity()<<"  str: \""<<buf.to_string<duct::u8string>()<<'\"'<<std::endl;

	std::wstring wstr;
	buf.to_string(wstr);
	std::wcout<<"units in wstring: ";
	for (duct::char32 const cu : wstr) {
		std::wcout<<L"0x"<<std::hex<<static_cast<duct::char32>(cu)<<", ";
	}
	std::wcout<<std::endl;
	std::wcout<<"wide: \""<<wstr<<"\""<<std::endl; // Woo, locales. This will probably output 'B'.
	return 0;
}
