
#include <duct/string.hpp>
#include <duct/Var.hpp>
#include <duct/VarTemplate.hpp>

#include <array>
#include <iomanip>
#include <iostream>

#include "common.inl"

using duct::u8string;
using duct::Var;
using duct::VarType;
using duct::VarMask;
using duct::VarTemplate;

void
signature(
	VarTemplate& tpl,
	VarMask const type_mask,
	VarTemplate::identity_vector_type&& identity,
	VarTemplate::layout_vector_type&& layout
) {
	tpl.set_type_mask(type_mask);
	tpl.set_identity(std::move(identity));
	tpl.set_layout(std::move(layout));
}

void
morph(
	Var& var,
	VarType const type
) {
	var.morph(type);
}

void
morph(
	Var& var,
	u8string&& name,
	VarType const type
) {
	var.set_name(std::move(name));
	var.morph(type);
}

void
morph(
	Var& var,
	VarType const type,
	Var::vector_type&& children
) {
	var.morph(type, std::move(children));
}

void
morph(
	Var& var,
	u8string&& name,
	VarType const type,
	Var::vector_type&& children
) {
	var.set_name(std::move(name));
	var.morph(type, std::move(children));
}

template<
	typename T
>
void
morph(
	Var& var,
	T value
) {
	var.morph(value);
}

template<
	typename T
>
void
morph(
	Var& var,
	u8string&& name,
	T value
) {
	var.set_name(std::move(name));
	var.morph(value);
}

void
validate(
	VarTemplate const& tpl,
	Var const& var,
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
	VarTemplate tpl;
	Var match, not_match;

	// String value
	signature(tpl, static_cast<VarMask>(VarType::string), {}, {});
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
		{VarMask::value, VarTemplate::Field::Flags::optional}
	});
	morph(match, VarType::array, {Var(1), Var(2), Var(3)});
	morph(not_match, VarType::node, {Var(1)});
	do_validation("Layout - normal",
		true, true, true,
		true, true, false
	);
	morph(match, VarType::array, {Var(1), Var(2)});
	do_validation_single(tpl, match, true, true, true, true);

	// Layout - empty field
	signature(tpl, VarMask::collection, {}, {
		{VarTemplate::Field::Flags::optional}
	});
	morph(match, VarType::array, {});
	do_validation("Layout - empty field",
		true, true, true,
		true, true, false
	);

	std::cout.flush();
	return 0;
}
