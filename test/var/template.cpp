
#include <duct/string.hpp>
#include <duct/Variable.hpp>
#include <duct/Template.hpp>

#include <array>
#include <iomanip>
#include <iostream>

#include "common.inl"

using duct::u8string;
using duct::Variable;
using duct::VarType;
using duct::VarMask;
using duct::Template;

void
signature(
	Template& tpl,
	VarMask const type_mask,
	Template::identity_vector_type&& identity,
	Template::layout_vector_type&& layout
) {
	tpl.set_type_mask(type_mask);
	tpl.set_identity(std::move(identity));
	tpl.set_layout(std::move(layout));
}

void
morph(
	Variable& var,
	VarType const type
) {
	var.morph(type);
}

void
morph(
	Variable& var,
	u8string&& name,
	VarType const type
) {
	var.set_name(std::move(name));
	var.morph(type);
}

void
morph(
	Variable& var,
	VarType const type,
	Variable::vector_type&& children
) {
	var.morph(type, std::move(children));
}

void
morph(
	Variable& var,
	u8string&& name,
	VarType const type,
	Variable::vector_type&& children
) {
	var.set_name(std::move(name));
	var.morph(type, std::move(children));
}

template<
	typename T
>
void
morph(
	Variable& var,
	T value
) {
	var.morph(value);
}

template<
	typename T
>
void
morph(
	Variable& var,
	u8string&& name,
	T value
) {
	var.set_name(std::move(name));
	var.morph(value);
}

void
validate(
	Template const& tpl,
	Variable const& var,
	bool expected[4u]
) {
	static char const* const
	s_msg_expectation[]{
		" [#  UNEXPECTED  #]",
		" [expected]"
	};
	std::cout << "\nvar:\n";
	print_var(var, 1);
	bool result[4u]{
		tpl.validate(var),
		tpl.validate_type(var),
		tpl.validate_identity(var),
		tpl.validate_layout(var)
	};
	std::cout
		<< std::left
		<< "  validate  : "
			<< std::setw(5) << result[0]
			<< s_msg_expectation[expected[0] == result[0]] << '\n'
		<< "  type    : "
			<< std::setw(5) << result[1]
			<< s_msg_expectation[expected[1] == result[1]] << '\n'
		<< "  identity: "
			<< std::setw(5) << result[2]
			<< s_msg_expectation[expected[2] == result[2]] << '\n'
		<< "  layout  : "
			<< std::setw(5) << result[3]
			<< s_msg_expectation[expected[3] == result[3]]
	<< std::endl;
}

#define do_validation_single(tpl, var, v, vt, vi, vl) \
	expected[0] = v; expected[1] = vt; expected[2] = vi; expected[3] = vl; \
	validate(tpl, var, expected)

#define do_validation( \
	series, \
	vt_match, vi_match, vl_match, \
	vt_not_match, vi_not_match, vl_not_match \
) { \
	std::cout << "\n# " series ":\n"; \
	do_validation_single( \
		tpl, match, true, vt_match, vi_match, vl_match \
	); \
	do_validation_single( \
		tpl, not_match, false, vt_not_match, vi_not_match, vl_not_match \
	); \
}

signed
main() {
	std::cout << std::boolalpha;

	bool expected[4u];
	Template tpl;
	Variable match, not_match;

	// String value
	signature(tpl, duct::var_mask(VarType::string), {}, {});
	morph(match, u8string("match"), u8string("rampant penguin"));
	morph(not_match, u8string("not_match"), 1234);
	do_validation("Value - string",
		true, true, false,
		false, true, false
	);

	// Identity
	signature(tpl, VarMask::any, {u8string("match")}, {});
	do_validation("Identity",
		true, true, false,
		true, false, false
	);

	// Layout - normal
	signature(tpl, VarMask::collection, {}, {
		VarMask::value,
		VarMask::value,
		{VarMask::value, Template::Field::Flags::optional}
	});
	morph(match, VarType::array, {Variable(1), Variable(2), Variable(3)});
	morph(not_match, VarType::node, {Variable(1)});
	do_validation("Layout - normal",
		true, true, true,
		true, true, false
	);
	morph(match, VarType::array, {Variable(1), Variable(2)});
	do_validation_single(tpl, match, true, true, true, true);

	// Layout - empty field
	signature(tpl, VarMask::collection, {}, {
		{Template::Field::Flags::optional}
	});
	morph(match, VarType::array, {});
	do_validation("Layout - empty field",
		true, true, true,
		true, true, false
	);

	std::cout.flush();
	return 0;
}
