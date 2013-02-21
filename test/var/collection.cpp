
#include <duct/config.hpp>
#include <duct/string.hpp>
#include <duct/detail/vartype.hpp>
#include <duct/Variable.hpp>

#include <iomanip>
#include <iostream>

#include "common.inl"

signed main() {
	duct::Variable top{
	{"top"}, duct::VARTYPE_ARRAY, {
		{{"string_var"}, {"bar"}},
		{{"int_var"}, 12341234},
		{{"float_var"}, 3.14159f},
		{{"bool_var"}, true},
		{{"null_var"}, duct::VARTYPE_NULL},
		{{"coll_l1"}, duct::VARTYPE_IDENTIFIER, {
			duct::Variable{81354},
			duct::Variable{42.0f},
			duct::Variable{true},
			duct::Variable{duct::VARTYPE_NULL},
			duct::Variable{duct::VARTYPE_NODE}
		}}
	}};
	print_var(top);
	std::cout.flush();
	return 0;
}
