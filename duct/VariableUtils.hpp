/**
@file VariableUtils.hpp
@brief Variable utilities

@author Tim Howard
@copyright 2010-2012 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_VARIABLEUTILS_HPP_
#define DUCT_VARIABLEUTILS_HPP_

#include "./config.hpp"
#include "./string.hpp"
#include "./detail/vartype.hpp"
#include "./Variable.hpp"

namespace duct {
namespace VariableUtils {

/**
	@addtogroup variable
	@{
*/

namespace {
static detail::var_config::string_type const s_sv_false{"false"};
static detail::var_config::string_type const s_sv_true{"true"};
static detail::var_config::string_type const s_sv_null{"null"};
} // anonymous namespace

/**
	Convert string to appropriate type and set Variable value.
	@note Conversion is (case sensitive):
		@c "false" morphs to a @c VARTYPE_BOOL with @c false
		@c "true" morphs to a @c VARTYPE_BOOL with @c true
		@c "null" morphs to a @c VARTYPE_NULL
		else morphs to a @c VARTYPE_STRING with @c value
	@param var Variable to modify.
	@param value String value to convert.
*/
void convert_typed(Variable& var, detail::var_config::string_type value) {
	if (0==s_sv_false.compare(value)) {
		var.morph(false);
	} else if (0==s_sv_true.compare(value)) {
		var.morph(true);
	} else if (0==s_sv_null.compare(value)) {
		var.nullify();
	} else {
		var.morph(std::move(value));
	}
}

/**
	Convert string to appropriate Variable.
	@note Conversion is (case sensitive):
		@c "false" returns a @c VARTYPE_BOOL with @c false
		@c "true" returns a @c VARTYPE_BOOL with @c true
		@c "null" returns a @c VARTYPE_NULL
		else returns a @c VARTYPE_STRING with @c value
	@returns String converted to either a @c VARTYPE_BOOL, @c VARTYPE_NULL or @c VARTYPE_STRING Variable.
	@param value String value to convert.
*/
Variable convert_typed(detail::var_config::string_type value) {
	if (0==s_sv_false.compare(value)) {
		return Variable(false);
	} else if (0==s_sv_true.compare(value)) {
		return Variable(true);
	} else if (0==s_sv_null.compare(value)) {
		return Variable(VARTYPE_NULL);
	} else {
		return Variable(std::move(value));
	}
}

/**
	Convert string to appropriate Variable with name.
	@note Conversion is (case sensitive):
		@c "false" returns a @c VARTYPE_BOOL with @c false
		@c "true" returns a @c VARTYPE_BOOL with @c true
		@c "null" returns a @c VARTYPE_NULL
		else returns a @c VARTYPE_STRING with @c value
	@returns String converted to either a @c VARTYPE_BOOL, @c VARTYPE_NULL or @c VARTYPE_STRING Variable.
	@param name Name of constructed variable.
	@param value String value to convert.
*/
Variable convert_typed(detail::var_config::name_type name, detail::var_config::string_type value) {
	if (0==s_sv_false.compare(value)) {
		return Variable(std::move(name), false);
	} else if (0==s_sv_true.compare(value)) {
		return Variable(std::move(name), true);
	} else if (0==s_sv_null.compare(value)) {
		return Variable(std::move(name), VARTYPE_NULL);
	} else {
		return Variable(std::move(name), std::move(value));
	}
}

/** @} */ // end of doc-group variable

} // namespace VariableUtils
} // namespace duct

#endif // DUCT_VARIABLEUTILS_HPP_
