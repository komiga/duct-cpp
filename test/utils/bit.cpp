
#include <duct/debug.hpp>
#include <duct/utility.hpp>

#include <iostream>
#include <iomanip>

using duct::bit;

enum B : unsigned {
	b0 = bit(0),
	b1 = bit(1),
};

static_assert((1 << 0) == B::b0, "");
static_assert((1 << 1) == B::b1, "");

using T = unsigned;

signed
main() {
	static constexpr unsigned n = sizeof(T) << 3;
	std::cout
		<< std::setfill(' ')
		<< std::right
	;
	for (unsigned index = 0; n > index; ++index) {
		std::cout
			<< "bit("
			<< std::dec
			<< std::setw(2u)
			<< index
			<< ") == "
			<< std::hex
			<< std::setw(sizeof(T) << 1)
			<< bit<T>(index)
		<< std::endl;
		DUCT_ASSERTE(
			(1 << index) == bit(index)
		);
	}

	return 0;
}
