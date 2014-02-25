/**
@file VarType.hpp
@brief VarType and type utilities.

@author Tim Howard
@copyright 2010-2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_VARTYPE_HPP_
#define DUCT_VARTYPE_HPP_

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
	null		= 1 << 0,
	/** Value type: String. */
	string		= 1 << 1,
	/** Value type: Integer. */
	integer		= 1 << 2,
	/** Value type: Float. */
	floatp		= 1 << 3,
	/** Value type: Boolean. */
	boolean		= 1 << 4,
	/** Collection type: Array. */
	array		= 1 << 5,
	/** Collection type: Node. */
	node		= 1 << 6,
	/** Collection type: Identifier. */
	identifier	= 1 << 7
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

/** @cond INTERNAL */

namespace {

template<
	typename U,
	typename E
>
inline constexpr U
pack_var_mask(
	U const value
) noexcept {
	return value;
}

template<
	typename U,
	typename E,
	typename... Rest
>
inline constexpr U
pack_var_mask(
	U const value,
	E const head,
	Rest const... rest
) noexcept {
	using bare_type =
		typename std::remove_cv<
			typename std::remove_reference<E>::type
		>::type;
	;
	static_assert(
		std::is_same<bare_type, VarType>::value ||
		std::is_same<bare_type, VarMask>::value,
		"arguments must be of type VarType or VarMask"
	);
	return pack_var_mask<U, E>(
		value | static_cast<U const>(head),
		rest...
	);
}

} // anonymous namespace

/** @endcond */ // INTERNAL

/**
	Construct an arbitrary variable type mask from types and masks.

	@returns The constructed mask.
	@tparam ArgP Argument types. Type deduction will fail if any
	type in this parameter pack is not VarType or VarMask.
	@param args Arguments.
*/
template<
	typename... ArgP
>
inline constexpr VarMask
var_mask(
	ArgP const... args
) noexcept {
	using U = std::underlying_type<VarMask>::type;
	return static_cast<VarMask>(pack_var_mask<U>(
		enum_cast<VarMask, U>(VarMask::none),
		args...
	));
}

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

#endif // DUCT_VARTYPE_HPP_
