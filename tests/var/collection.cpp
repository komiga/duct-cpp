
#include <duct/config.hpp>
#include <duct/string.hpp>
#include <duct/Variable.hpp>
#include <duct/detail/vartype.hpp>

#include <iomanip>
#include <iostream>

#include "common.inl"

int main(int argc, char* argv[]) {
	duct::Variable top(duct::u8string("top"), duct::VARTYPE_ARRAY);
	top
		.emplace_back(duct::u8string("string_var"), duct::u8string("bar"))
		.emplace_back(duct::u8string("int_var"), 12341234)
		.emplace_back(duct::u8string("float_var"), 3.14159f)
		.emplace_back(duct::u8string("bool_var"), true)
		.emplace_back(duct::u8string("null_var"), duct::VARTYPE_NULL)
		.emplace_back(std::move(
			duct::Variable(duct::u8string("coll_l1"), duct::VARTYPE_IDENTIFIER)
			.emplace_back(81354)
			.emplace_back(true)
			.emplace_back(duct::VARTYPE_NULL)
			.emplace_back(duct::VARTYPE_NODE))
		);
	print_var(top);

	std::cout.flush();
	return 0;
}
