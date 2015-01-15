
#include <duct/utility.hpp>

#include <iostream>

using namespace duct::enum_ops;

using duct::bit;
using duct::enum_cast;

enum class E : unsigned {
	none = 0u,
	A = bit(0),
	B = bit(1),
	C = bit(2),
};

static char const* const
e_names[]{
	"none",
	"A",
	"B",
	"A,B",
	"C",
	"A,C",
	"B,C",
	"A,B,C",
};

void
print_e(
	E const value
) {
	auto const uv = enum_cast(value);
	if (std::extent<decltype(e_names)>::value > uv) {
		std::cout
			<< "E{"
			<< e_names[uv]
			<< "}\n"
		;
	} else {
		std::cout << "E{INVALID}\n";
	}
}

signed
main() {
	constexpr E c = E::A | E::B | E::C;
	static_assert(enum_cast(c & E::A), "");
	static_assert(!enum_cast(c & E::none), "");

	print_e(E::none);
	print_e(E::A);
	print_e(E::B);
	print_e(E::A | E::B);
	print_e(E::C);
	print_e(E::A | E::C);
	print_e(E::B | E::C);
	print_e(E::A | E::B | E::C);
	return 0;
}
