
#include <duct/string.hpp>
#include <duct/detail/vartype.hpp>
#include <duct/Var.hpp>

#include <iomanip>
#include <iostream>

#include "common.inl"

signed
main() {
	duct::Var
		string_var{duct::u8string("string_var"), duct::u8string("bar")},
		int_var{duct::u8string("int_var"), 12341234},
		float_var{duct::u8string("float_var"), 3.14159f},
		bool_var{duct::u8string("bool_var"), true},
		null_var{duct::u8string("null_var"), duct::VarType::null}
	;

	print_var(string_var);
	print_var(int_var);
	print_var(float_var);
	print_var(bool_var);
	print_var(null_var);

	duct::Var
		morph_var{duct::u8string("morph_var"), duct::u8string("qwerty")}
	;

	print_var(morph_var);
	morph_var.morph(142);
	print_var(morph_var);
	morph_var.morph(23.12f);
	print_var(morph_var);

	std::cout.flush();
	return 0;
}
