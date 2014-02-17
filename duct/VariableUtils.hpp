/**
@file VariableUtils.hpp
@brief Variable utilities.

@author Tim Howard
@copyright 2010-2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_VARIABLEUTILS_HPP_
#define DUCT_VARIABLEUTILS_HPP_

#include "./config.hpp"
#include "./string.hpp"
#include "./detail/string_traits.hpp"
#include "./detail/vartype.hpp"
#include "./Variable.hpp"

namespace duct {
namespace VariableUtils {

/**
	@addtogroup variable
	@{
*/

namespace {
static detail::string_traits<detail::var_config::string_type>::char_type const
	s_sv_false[]{"false"},
	s_sv_true[]{"true"},
	s_sv_null[]{"null"}
;
} // anonymous namespace

/**
	Convert string to appropriate type and set Variable value.

	@note Conversion is (case sensitive):
	- @c "false" morphs to a @c VarType::boolean with @c false
	- @c "true" morphs to a @c VarType::boolean with @c true
	- @c "null" morphs to a @c VarType::null
	- else morphs to a @c VarType::string with @c value

	@param var Variable to modify.
	@param value String value to convert.
*/
void
convert_typed(
	Variable& var,
	detail::var_config::string_type value
) {
	if (0 == value.compare(s_sv_false)) {
		var.morph(false);
	} else if (0 == value.compare(s_sv_true)) {
		var.morph(true);
	} else if (0 == value.compare(s_sv_null)) {
		var.nullify();
	} else {
		var.morph(std::move(value));
	}
}

/**
	Convert string to appropriate Variable.

	@note Conversion is (case sensitive):
	- @c "false" returns a @c VarType::boolean with @c false
	- @c "true" returns a @c VarType::boolean with @c true
	- @c "null" returns a @c VarType::null
	- else returns a @c VarType::string with @c value

	@returns String converted to either a @c VarType::boolean,
	@c VarType::null or @c VarType::string Variable.
	@param value String value to convert.
*/
Variable
convert_typed(
	detail::var_config::string_type value
) {
	if (0 == value.compare(s_sv_false)) {
		return Variable(false);
	} else if (0 == value.compare(s_sv_true)) {
		return Variable(true);
	} else if (0 == value.compare(s_sv_null)) {
		return Variable(VarType::null);
	} else {
		return Variable(std::move(value));
	}
}

/**
	Convert string to appropriate Variable with name.

	@note Conversion is (case sensitive):
	- @c "false" returns a @c VarType::boolean with @c false
	- @c "true" returns a @c VarType::boolean with @c true
	- @c "null" returns a @c VarType::null
	- else returns a @c VarType::string with @c value

	@returns String converted to either a @c VarType::boolean,
	@c VarType::null or @c VarType::string Variable.
	@param name Name of constructed variable.
	@param value String value to convert.
*/
Variable
convert_typed(
	detail::var_config::name_type name,
	detail::var_config::string_type value
) {
	if (0 == value.compare(s_sv_false)) {
		return Variable(std::move(name), false);
	} else if (0 == value.compare(s_sv_true)) {
		return Variable(std::move(name), true);
	} else if (0 == value.compare(s_sv_null)) {
		return Variable(std::move(name), VarType::null);
	} else {
		return Variable(std::move(name), std::move(value));
	}
}

/** @} */ // end of doc-group variable

} // namespace VariableUtils
} // namespace duct

#endif // DUCT_VARIABLEUTILS_HPP_
