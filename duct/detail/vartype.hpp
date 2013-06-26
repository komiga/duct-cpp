/**
@file detail/vartype.hpp
@brief Variable type traits.

@author Tim Howard
@copyright 2010-2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_DETAIL_VARTYPE_HPP_
#define DUCT_DETAIL_VARTYPE_HPP_

#include "./../config.hpp"
#include "./../traits.hpp"
#include "./../string.hpp"

#include <strings.h>
#include <type_traits>

// TODO: documentation on struct members

namespace duct {

// Forward declarations
enum VariableType : unsigned;
enum VariableClass : unsigned;
enum VariableMasks : unsigned;

/**
	@addtogroup variable
	@{
*/

/**
	Variable types.

	@note @c VARTYPE_NULL is a special type to indicate nullness;
	it has no associated data field.

	@sa VariableClass, VariableMasks
*/
enum VariableType : unsigned {
	/** Null; value-less. */
	VARTYPE_NULL		= 1 << 0,
	/** Value type: String. */
	VARTYPE_STRING		= 1 << 1,
	/** Value type: Integer. */
	VARTYPE_INTEGER		= 1 << 2,
	/** Value type: Float. */
	VARTYPE_FLOAT		= 1 << 3,
	/** Value type: Boolean. */
	VARTYPE_BOOL		= 1 << 4,
	/** Collection type: Array. */
	VARTYPE_ARRAY		= 1 << 5,
	/** Collection type: Node. */
	VARTYPE_NODE		= 1 << 6,
	/** Collection type: Identifier. */
	VARTYPE_IDENTIFIER	= 1 << 7
};

/**
	Variable classes.

	@note @c VARTYPE_NULL is essentially its own class and is
	thus not included in any defined classes.

	@sa VariableType, VariableMasks
*/
enum VariableClass : unsigned {
	/**
		Numerical variable types.
		Equal to:
		@code
			VARTYPE_INTEGER | VARTYPE_FLOAT
		@endcode
	*/
	VARCLASS_NUMERICAL
		= VARTYPE_INTEGER
		| VARTYPE_FLOAT
	,
	/**
		Value variable types.
		Equal to:
		@code
			VARTYPE_STRING | VARTYPE_INTEGER |
			VARTYPE_FLOAT | VARTYPE_BOOL
		@endcode
	*/
	VARCLASS_VALUE
		= VARTYPE_STRING
		| VARTYPE_INTEGER
		| VARTYPE_FLOAT
		| VARTYPE_BOOL
	,
	/**
		Collection variable types.
		Equal to:
		@code
			VARTYPE_ARRAY | VARTYPE_NODE | VARTYPE_IDENTIFIER
		@endcode
	*/
	VARCLASS_COLLECTION
		= VARTYPE_ARRAY
		| VARTYPE_NODE
		| VARTYPE_IDENTIFIER
};

/**
	Various VariableType masks.

	@sa VariableType, VariableClass
*/
enum VariableMasks : unsigned {
	/** Matches no types. */
	VARMASK_NONE
		= 0x00
	,
	/** Matches all types. */
	VARMASK_ALL
		= VARCLASS_VALUE
		| VARCLASS_COLLECTION
		| VARTYPE_NULL
	,
	/** Matches VARCLASS_VALUE or VARTYPE_NULL. */
	VARMASK_VALUE_OR_NULL
		= VARCLASS_VALUE
		| VARTYPE_NULL
};

namespace detail {

#ifndef DUCT_CONFIG_VAR_LARGE_NUMERIC_TYPES
	/**
		@ingroup config

		Whether to use 64-bit and double-precision for
		VARTYPE_INTEGER and VARTYPE_FLOAT, respectively.

		@note Defaults to 0.

		@sa var_config
	*/
	#define DUCT_CONFIG_VAR_LARGE_NUMERIC_TYPES 0
#endif

/**
	Variable configuration.
*/
struct var_config final
	: public traits::restrict_all
{
public:
	/** typename for Variable names */
	using name_type = u8string;
	/** typename for @c VARTYPE_STRING */
	using string_type = u8string;

#if (0 == DUCT_CONFIG_VAR_LARGE_NUMERIC_TYPES) \
	|| defined(DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI)
	/**
		typename for @c VARTYPE_INTEGER.
		@c int32_t by default; @c int64_t
		with #DUCT_CONFIG_VAR_LARGE_NUMERIC_TYPES.
	*/
	using int_type = int32_t;
	/**
		typename for @c VARTYPE_FLOAT.
		@c float by default; @c double
		with #DUCT_CONFIG_VAR_LARGE_NUMERIC_TYPES.
	*/
	using float_type = float;
#else
	using int_type = int64_t;
	using float_type = double;
#endif

	/** typename for @c VARTYPE_BOOL. */
	using bool_type = bool;
};

/**
	Get the name of a @c VariableType.
	@returns The name of the variable type.
	@param type Variable type.
*/
char const*
get_vartype_name(
	VariableType const type
) noexcept {
	static char const* const
	names[]{
		"NULL",
		"STRING",
		"INTEGER",
		"FLOAT",
		"BOOL",
		"ARRAY",
		"NODE",
		"IDENTIFIER"
	};
	return names[ffs(static_cast<signed>(type)) - 1];
}

/**
	Check if a typename has an associated @c VARCLASS_VALUE type.
*/
template<
	typename T
>
struct is_valtype final
	: public traits::restrict_all
	, public std::false_type
{};

/**
	Translate a @c VARCLASS_VALUE type to a typename.
*/
template<
	VariableType const VType
>
struct valtype_to_type;

/**
	Translate a typename to @c VARCLASS_VALUE types.
*/
template<
	typename T
>
struct type_to_valtype;

/**
	VariableType traits for @c VARCLASS_VALUE.
*/
template<VariableType const VType>
struct valtype_traits final
	: public traits::restrict_all
{
public:
	/** Whether the typename is POD. */
	static constexpr bool
		is_pod = std::is_pod<typename valtype_to_type<VType>::type>::value;
	/** The value typename for the VariableType. */
	using value_type = typename valtype_to_type<VType>::type;
};

/** @cond INTERNAL */
#define DUCT_DETAIL_TRAITS_(VT, T)	\
	template<> struct valtype_to_type<VT> final \
	: public traits::restrict_all \
	{ using type = T; }; \
	template<> struct is_valtype<T> final \
	: public traits::restrict_all \
	, public std::true_type \
	{}; \
	template<> struct type_to_valtype<T> final \
	: public traits::restrict_all \
	{ static constexpr VariableType value = VT; } /**/

DUCT_DETAIL_TRAITS_(VARTYPE_STRING, var_config::string_type);
DUCT_DETAIL_TRAITS_(VARTYPE_INTEGER, var_config::int_type);
DUCT_DETAIL_TRAITS_(VARTYPE_FLOAT, var_config::float_type);
DUCT_DETAIL_TRAITS_(VARTYPE_BOOL, var_config::bool_type);

#undef DUCT_DETAIL_TRAITS_

#define DUCT_DETAIL_TRAITS_(VT, T)	\
	template<> struct is_valtype<T> final \
	: public traits::restrict_all \
	, public std::true_type \
	{}; \
	template<> struct type_to_valtype<T> final \
	: public traits::restrict_all \
	{ static constexpr VariableType value = VT; } /**/

#if (0 != DUCT_CONFIG_VAR_LARGE_NUMERIC_TYPES)
	// Define smaller variants, for convenience
	DUCT_DETAIL_TRAITS_(VARTYPE_INTEGER, int32_t);
	DUCT_DETAIL_TRAITS_(VARTYPE_FLOAT, float);
#endif

#undef DUCT_DETAIL_TRAITS_
/** @endcond */ // INTERNAL

} // namespace detail

/** @} */ // end of doc-group variable

} // namespace duct

#endif // DUCT_DETAIL_VARTYPE_HPP_
