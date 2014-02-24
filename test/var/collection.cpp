
#include <duct/config.hpp>
#include <duct/string.hpp>
#include <duct/detail/vartype.hpp>
#include <duct/Var.hpp>

#include <iomanip>
#include <iostream>

#include "common.inl"

signed
main() {
	duct::Var top{
	{"top"}, duct::VarType::array, {
		{{"string_var"}, {"bar"}},
		{{"int_var"}, 12341234},
		{{"float_var"}, 3.14159f},
		{{"bool_var"}, true},
		{{"null_var"}, duct::VarType::null},
		{{"coll_l1"}, duct::VarType::identifier, {
			duct::Var{81354},
			duct::Var{42.0f},
			duct::Var{true},
			duct::Var{duct::VarType::null},
			duct::Var{duct::VarType::node}
		}}
	}};
	print_var(top);
	std::cout.flush();
	return 0;
}
