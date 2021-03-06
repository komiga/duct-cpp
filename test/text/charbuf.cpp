
#include <duct/char.hpp>
#include <duct/string.hpp>
#include <duct/CharBuf.hpp>

#include <string>
#include <iomanip>
#include <iostream>

signed
main() {
	duct::CharBuf buf;
	buf.push_back(0x3042);
	std::cout
		<< "size: " << buf.size() << '\n'
		<< "capacity: " << buf.capacity() << '\n'
		<< "cache: \"" << buf.to_string() << "\"\n"
		<< "str: \"" << buf.to_string<duct::u8string>() << "\"\n"
	; std::cout.flush();

	std::wstring wstr;
	buf.to_string(wstr);

	std::wcout << L"units in wstring: ";
	for (duct::char32 const cu : wstr) {
		std::wcout
			<< L"0x" << std::hex << static_cast<duct::char32>(cu) << ", "
		;
	}
	std::wcout << L'\n';
	// Woo, locales. This will probably output 'B'.
	std::wcout
		<< L"wide: \"" << wstr << L"\""
	<< std::endl;
	return 0;
}
