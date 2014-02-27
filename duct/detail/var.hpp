/**
@file detail/var.hpp
@brief Var internals.

@author Tim Howard
@copyright 2010-2014 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_DETAIL_VARTYPE_HPP_
#define DUCT_DETAIL_VARTYPE_HPP_

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
