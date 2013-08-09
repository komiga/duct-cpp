
#include <duct/IO/multistream.hpp>

#include <cstdio>
#include <type_traits>
#include <iostream>
#include <sstream>

// Sufficiently small to test overflow
char multicast_buffer[10];

signed
main() {
	std::ostringstream sstream;
	duct::IO::omultistream multicast{
		{std::cout, sstream},
		multicast_buffer,
		sizeof(multicast_buffer)
	};
	multicast
		<< "'string' "
		<< 42 << ' '
		<< 3.14159265f
	;
	multicast.flush();
	std::cout
		<< '\n'
		<< "stringstream: \"" << sstream.str() << "\""
	<< std::endl;
	return 0;
}
