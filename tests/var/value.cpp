
#include <duct/config.hpp>
#include <duct/string.hpp>
#include <duct/Variable.hpp>
#include <duct/detail/vartype.hpp>

#include <iomanip>
#include <iostream>

#include "common.inl"

int main(int argc, char* argv[]) {
	duct::Variable string_var(duct::u8string("string_var"), duct::u8string("bar"));
	duct::Variable int_var(duct::u8string("int_var"), 12341234);
	duct::Variable float_var(duct::u8string("float_var"), 3.14159f);
	duct::Variable bool_var(duct::u8string("bool_var"), true);
	duct::Variable null_var(duct::u8string("null_var"), duct::VARTYPE_NULL);
	print_var(string_var);
	print_var(int_var);
	print_var(float_var);
	print_var(bool_var);
	print_var(null_var);
	
	duct::Variable morph_var(duct::u8string("morph_var"), duct::u8string("qwerty"));
	print_var(morph_var);
	morph_var.morph(142);
	print_var(morph_var);
	morph_var.morph(23.12f);
	print_var(morph_var);
	std::cout.flush();
	return 0;
}
