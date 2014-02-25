/**
@file VarUtils.hpp
@brief Var utilities.

@author Tim Howard
@copyright 2010-2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_VARUTILS_HPP_
#define DUCT_VARUTILS_HPP_

#include "./config.hpp"
#include "./string.hpp"
#include "./detail/string_traits.hpp"
#include "./detail/var.hpp"
#include "./StateStore.hpp"
#include "./CharacterRange.hpp"
#include "./Var.hpp"

namespace duct {
namespace VarUtils {

/**
	@addtogroup var
	@{
*/

namespace {
using var_char_type
= detail::string_traits<
	detail::var_config::string_type
>::char_type;

static var_char_type const
	s_sv_false[]{"false"},
	s_sv_true[]{"true"},
	s_sv_null[]{"null"}
;

static CharacterRange const
	s_range_numeral{char32{'0'}, '9' - '0'}
;

bool
parse_lit(
	detail::var_config::string_type const& value,
	var_char_type const* lit
) {
	auto it = value.cbegin() + 1;
	for (
		++lit;
		'\0' != *lit && value.cend() != it;
		++lit, ++it
	) {
		if (*it != *lit) {
			return false;
		}
	}
	if ('\0' == *lit && value.cend() == it) {
		return true;
	} else {
		return false;
	}
}

} // anonymous namespace

/**
	Convert string to value-class variable.

	@note Conversion is (case sensitive):
	- @c "false" morphs to a @c VarType::boolean with @c false.
	- @c "true" morphs to a @c VarType::boolean with @c true.
	- @c "null" morphs to a @c VarType::null.
	- @c [+\-]?[0-9.]* morphs to a @c VarType::integer.
	  or @c VarType::floatp.
	- Else morphs to a @c VarType::string with @c value.

	@param var Var to modify.
	@param value String to convert.
*/
void
convert_typed(
	Var& var,
	detail::var_config::string_type const& value
) {
	bool has_sign = false;
	bool has_decimal = false;
	bool has_numeral = false;
	var_char_type const* literal = nullptr;

	if (value.empty()) {
		goto l_string;
	}

	// Deduce states from first character
	switch (value[0]) {
	case 'f': literal = s_sv_false; break;
	case 't': literal = s_sv_true; break;
	case 'n': literal = s_sv_null; break;

	case '-': // fall-through
	case '+': has_sign = true; break;
	case '.': has_decimal = true; break;
	}

	// Try to parse literals
	if (literal) {
		if (parse_lit(value, literal)) {
			switch (value[0]) {
			case 'f': var.morph(false); break;
			case 't': var.morph(true); break;
			case 'n': var.nullify(); break;
			}
			return;
		} else {
			goto l_string;
		}
	}

	// Try to parse numeric value
	for (
		auto it = value.cbegin() + unsigned{has_sign || has_decimal};
		value.cend() != it;
		++it
	) {
		switch (*it) {
		case '-': // fall-through
		case '+':
			// Sign can only occur at start of value
			goto l_string;
			break;

		case '.':
			if (has_decimal) {
				// Only one decimal can occur
				goto l_string;
			} else {
				has_decimal = true;
			}
			break;

		default:
			if (!s_range_numeral.contains(*it)) {
				goto l_string;
			} else {
				has_numeral = true;
			}
			break;
		}
	}

	if (!has_numeral) {
		// Consists of only signs or only a decimal
		goto l_string;
	} else {
		aux::istringstream stream(value);
		if (has_decimal) {
			var.morph(0.0f);
			stream >> var.get_float_ref();
			if (stream.fail()) {
				var.assign(0.0f);
			}
		} else {
			var.morph(0);
			stream >> var.get_int_ref();
			if (stream.fail()) {
				var.assign(0);
			}
		}
	}
	return;

l_string:
	var.reset();
	var.morph(value);
	return;
}

/**
	Convert string to value-class variable.

	@returns Var containing converted value.
	@param value String to convert.

	@sa void convert_typed(Var&, detail::var_config::string_type const&)
*/
Var
convert_typed(
	detail::var_config::string_type const& value
) {
	Var var;
	convert_typed(var, value);
	return var;
}

/**
	Convert string to value-class variable with name.

	@returns Var containing converted value.
	@param name Name of result variable.
	@param value String to convert.

	@sa void convert_typed(Var&, detail::var_config::string_type const&)
*/
Var
convert_typed(
	detail::var_config::name_type name,
	detail::var_config::string_type const& value
) {
	Var var{std::move(name), VarType::null};
	convert_typed(var, value);
	return var;
}

/** @} */ // end of doc-group var

} // namespace VarUtils
} // namespace duct

#endif // DUCT_VARUTILS_HPP_
