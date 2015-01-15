/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief VarType and type utilities.
*/

#pragma once

#include "./config.hpp"
#include "./utility.hpp"

namespace duct {

// Forward declarations
enum class VarType : unsigned;
enum class VarMask : unsigned;

/**
	@addtogroup var
	@{
*/

/**
	Var types.

	@sa VarMask
*/
enum class VarType : unsigned {
	/** Null; value-less. */
	null		= bit(0),
	/** Value type: String. */
	string		= bit(1),
	/** Value type: Integer. */
	integer		= bit(2),
	/** Value type: Float. */
	floatp		= bit(3),
	/** Value type: Boolean. */
	boolean		= bit(4),
	/** Collection type: Array. */
	array		= bit(5),
	/** Collection type: Node. */
	node		= bit(6),
	/** Collection type: Identifier. */
	identifier	= bit(7)
};

/**
	Var type classes and masks.

	@note An arbitrary mask can be constructed with var_mask().

	@sa VarType
*/
enum class VarMask : unsigned {
	/**
		No type.
	*/
	none
		= 0x00
	,

	/**
		Null class.
	*/
	null
		= enum_cast(VarType::null)
	,

	/**
		Numeric class.
	*/
	numeric
		= enum_cast(VarType::integer)
		| enum_cast(VarType::floatp)
	,

	/**
		Value class (excluding null).
	*/
	value
		= enum_cast(VarType::string)
		| enum_cast(VarType::integer)
		| enum_cast(VarType::floatp)
		| enum_cast(VarType::boolean)
	,

	/**
		Value class (including null).
	*/
	value_nullable
		= value
		| null
	,

	/**
		Collection class.
	*/
	collection
		= enum_cast(VarType::array)
		| enum_cast(VarType::node)
		| enum_cast(VarType::identifier)
	,

	/**
		Any type.
	*/
	any
		= value_nullable
		| collection
	,
};

/**
	Test type against a mask.

	@returns @c true if @a type is of @a mask.
	@param type Type to test.
	@param mask Type mask to test against.
*/
inline constexpr bool
var_type_is_of(
	VarType const type,
	VarMask const mask
) noexcept {
	return enum_cast(type) & enum_cast(mask);
}

/**
	Get the name of a variable type.

	@returns The name of the variable type or @c "invalid" if @a type
	is invalid.
	@param type Var type.
*/
inline char const*
var_type_name(
	VarType const type
) noexcept {
	switch (type) {
	case VarType::null		: return "null";
	case VarType::string	: return "string";
	case VarType::integer	: return "integer";
	case VarType::floatp	: return "floatp";
	case VarType::boolean	: return "boolean";
	case VarType::array		: return "array";
	case VarType::node		: return "node";
	case VarType::identifier: return "identifier";
	default:
		return "invalid";
	};
}

/** @} */ // end of doc-group var

} // namespace duct
