
#include <duct/utility.hpp>

#include <iostream>

using duct::bit;
using duct::enum_cast;
using duct::enum_bitand;
using duct::enum_combine;

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
	constexpr E c = enum_combine(E::A, E::B, E::C);
	static_assert(enum_bitand(c, E::A), "");
	static_assert(!enum_bitand(c, E::none), "");

	E e = E::none; print_e(e);
	e = enum_combine(E::A); print_e(e);
	e = enum_combine(E::B); print_e(e);
	e = enum_combine(E::A, E::B); print_e(e);
	e = enum_combine(E::C); print_e(e);
	e = enum_combine(E::A, E::C); print_e(e);
	e = enum_combine(E::B, E::C); print_e(e);
	e = enum_combine(E::A, E::B, E::C); print_e(e);

	// Will fail to compile:
	//enum_combine(E::A, int());
	return 0;
}
