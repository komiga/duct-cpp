/**
@file detail/vartype.hpp
@brief Var type traits.

@author Tim Howard
@copyright 2010-2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_DETAIL_VARTYPE_HPP_
#define DUCT_DETAIL_VARTYPE_HPP_

#include "./../config.hpp"
#include "./../traits.hpp"
#include "./../string.hpp"
#include "./../utility.hpp"

#include <type_traits>

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

namespace detail {

#ifndef DUCT_CONFIG_VAR_LARGE_NUMERIC_TYPES
	/**
		@ingroup config

		Whether to use 64-bit and double-precision for
		VarType::integer and VarType::floatp, respectively.

		@note Defaults to 0.

		@sa var_config
	*/
	#define DUCT_CONFIG_VAR_LARGE_NUMERIC_TYPES 0
#endif

/**
	Var configuration.
*/
struct var_config final
	: public traits::restrict_all
{
public:
	/** typename for Var names */
	using name_type = u8string;
	/** typename for @c VarType::string */
	using string_type = u8string;

#if (0 == DUCT_CONFIG_VAR_LARGE_NUMERIC_TYPES) \
	|| defined(DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI)
	/**
		typename for @c VarType::integer.
		@c int32_t by default; @c int64_t
		with #DUCT_CONFIG_VAR_LARGE_NUMERIC_TYPES.
	*/
	using int_type = std::int32_t;
	/**
		typename for @c VarType::floatp.
		@c float by default; @c double
		with #DUCT_CONFIG_VAR_LARGE_NUMERIC_TYPES.
	*/
	using float_type = float;
#else
	using int_type = std::int64_t;
	using float_type = double;
#endif

	/** typename for @c VarType::boolean. */
	using bool_type = bool;
};

/**
	Get the name of a variable type.

	@returns The name of the variable type.
	@param type Var type.
*/
char const*
get_vartype_name(
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

/**
	Check if a typename has an associated value-class type.
*/
template<
	typename T
>
struct is_valtype final
	: public traits::restrict_all
	, public std::false_type
{};

/**
	Translate a value-class type to a typename.
*/
template<
	VarType const V
>
struct valtype_to_type;

/**
	Translate a typename to value-class types.
*/
template<
	typename T
>
struct type_to_valtype;

/**
	VarType traits for value-class variable types.
*/
template<VarType const V>
struct valtype_traits final
	: public traits::restrict_all
{
public:
	/** Whether the typename is POD. */
	static constexpr bool
	is_pod = std::is_pod<typename valtype_to_type<V>::type>::value;
	/** The value typename for the VarType. */
	using value_type = typename valtype_to_type<V>::type;
};

/** @cond INTERNAL */
#define DUCT_DETAIL_TRAITS_(V, T)	\
	template<> struct valtype_to_type<V> final \
	: public traits::restrict_all \
	{ using type = T; }; \
	template<> struct is_valtype<T> final \
	: public traits::restrict_all \
	, public std::true_type \
	{}; \
	template<> struct type_to_valtype<T> final \
	: public traits::restrict_all \
	{ static constexpr VarType value = V; } /**/

DUCT_DETAIL_TRAITS_(VarType::string, var_config::string_type);
DUCT_DETAIL_TRAITS_(VarType::integer, var_config::int_type);
DUCT_DETAIL_TRAITS_(VarType::floatp, var_config::float_type);
DUCT_DETAIL_TRAITS_(VarType::boolean, var_config::bool_type);

#undef DUCT_DETAIL_TRAITS_

#define DUCT_DETAIL_TRAITS_(V, T)	\
	template<> struct is_valtype<T> final \
	: public traits::restrict_all \
	, public std::true_type \
	{}; \
	template<> struct type_to_valtype<T> final \
	: public traits::restrict_all \
	{ static constexpr VarType value = V; } /**/

#if (0 != DUCT_CONFIG_VAR_LARGE_NUMERIC_TYPES)
	// Define smaller variants, for convenience
	DUCT_DETAIL_TRAITS_(VarType::integer, std::int32_t);
	DUCT_DETAIL_TRAITS_(VarType::floatp, float);
#endif

#undef DUCT_DETAIL_TRAITS_
/** @endcond */ // INTERNAL

} // namespace detail

/** @} */ // end of doc-group var

} // namespace duct

#endif // DUCT_DETAIL_VARTYPE_HPP_
