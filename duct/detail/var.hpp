/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Var internals.
*/

#pragma once

#include "./../config.hpp"
#include "./../traits.hpp"
#include "./../string.hpp"
#include "./../VarType.hpp"

#include <type_traits>

namespace duct {

/**
	@addtogroup var
	@{
*/

namespace detail {

#ifndef DUCT_CONFIG_VAR_LARGE_NUMERIC_TYPES
	/**
		@ingroup config

		Whether to use 64-bit and double-precision for
		VarType::integer and VarType::decimal, respectively.

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
	using integer_type = std::int32_t;
	/**
		typename for @c VarType::decimal.
		@c float by default; @c double
		with #DUCT_CONFIG_VAR_LARGE_NUMERIC_TYPES.
	*/
	using decimal_type = float;
#else
	using integer_type = std::int64_t;
	using decimal_type = double;
#endif

	/** typename for @c VarType::boolean. */
	using boolean_type = bool;
};

/**
	Check if a typename has an associated value-class type.
*/
template<class T>
struct is_valtype final
	: public traits::restrict_all
	, public std::false_type
{};

/**
	Translate a value-class type to a typename.
*/
template<VarType const V>
struct valtype_to_type;

/**
	Translate a typename to value-class types.
*/
template<class T>
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
DUCT_DETAIL_TRAITS_(VarType::integer, var_config::integer_type);
DUCT_DETAIL_TRAITS_(VarType::decimal, var_config::decimal_type);
DUCT_DETAIL_TRAITS_(VarType::boolean, var_config::boolean_type);

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
	DUCT_DETAIL_TRAITS_(VarType::decimal, float);
#endif

#undef DUCT_DETAIL_TRAITS_
/** @endcond */ // INTERNAL

} // namespace detail

/** @} */ // end of doc-group var

} // namespace duct
