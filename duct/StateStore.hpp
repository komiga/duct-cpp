/**
@file StateStore.hpp
@brief StateStore class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_STATESTORE_HPP_
#define DUCT_STATESTORE_HPP_

#include "./config.hpp"

#include <type_traits>

/*
FIXME: constexpr implies const on non-static member functions in
C++11, but many member functions here can and should be constexpr in
C++1y.

TODO: When constexpr doesn't imply const, operations should have
parameter pack variants.
*/

namespace duct {

// Forward declarations
template<
	typename S,
	typename V
>
class StateStore;

/** @cond INTERNAL */

template<
	typename V,
	typename S
>
constexpr V
pack_bitor(
	V const value
) noexcept {
	return value;
}

// FIXME: Oh for the love of Mo'Jaal.. C++11 doesn't allow you to
// take a function parameter pack with an ordinary type. It _must_
// be a template type parameter pack. Ergo, this is valid:
//
// template<typename... T> void f(T...);
//
// But this is not:
//
// void f(T...);

template<
	typename V,
	typename S,
	typename Head,
	// NB: To prevent unwanted types leaking into this util...
	// Nuke when this parameter pack terror is resolved.
	class = typename std::enable_if<
		std::is_same<S, Head>::value
	>::type,
	typename... Tail
>
constexpr V
pack_bitor(
	V const value,
	Head const head,
	Tail const... tail
) noexcept {
	return pack_bitor<V, S>(
		value | static_cast<V const>(head),
		tail...
	);
}

/** @endcond */ // INTERNAL

/**
	@addtogroup utils
	@{
*/

/**
	Typesafe state storage for bitflags.

	@note Both @a S and @a V must be plain non-const, non-volatile,
	non-reference, and non-pointer types.

	@tparam S State type. This type must be an enum or non-boolean
	integral type.

	@tparam V Value type. Defaults to the underlying type of @a S
	if @a S is an enum, or @a S otherwise. This type must be a non-
	boolean integral type to which @a S is @c static_cast-able.
*/
template<
	typename S,
	typename V
	= typename std::conditional<
		std::is_enum<S>::value,
		typename std::underlying_type<S>::type,
		S
	>::type
>
class StateStore {
	static_assert(
		std::is_same<typename std::decay<S>::type, S>::value &&
		(
			std::is_enum<S>::value ||
			(
				std::is_integral<S>::value &&
				!std::is_same<typename std::decay<S>::type, bool>::value
			)
		),
		"state type must be a non-reference, non-cv enum "
		"or non-boolean integral type"
	);
	static_assert(
		std::is_same<typename std::decay<S>::type, S>::value &&
		(
			std::is_integral<V>::value &&
			!std::is_same<typename std::remove_cv<V>::type, bool>::value
		),
		"value type must be a non-reference, non-cv, non-boolean "
		"integral type"
	);

public:
	/**
		State type.
	*/
	using state_type = S;
	/**
		Value type.
	*/
	using value_type = V;

private:
	value_type m_value;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with states.

		@tparam SFaux Faux template type parameter pack required to
		enable function parameter pack parameter in C++11. Don't ask.
		Types must be equal to @c state_type; SFINAE will fail
		otherwise.
		@param states Parameter pack of states to enable.
	*/
	template<
		typename... SFaux
	>
	constexpr
	StateStore(
		SFaux const... states
	) noexcept
		: m_value(
			pack_bitor<value_type, state_type>(
				value_type(0),
				states...
			)
		)
	{}

	// FIXME: Defect in GCC 4.7.3: compiler trips itself by
	// not initializing m_value in its own generated ctor.
	/** Default constructor. */
	StateStore() = default;
	/** Copy constructor. */
	StateStore(StateStore const&) = default;
	/** Move constructor. */
	StateStore(StateStore&&) = default;
	/** Destructor. */
	~StateStore() = default;
/// @}

/** @name Operators */ /// @{
	/** Copy-assignment operator. */
	StateStore& operator=(StateStore const&) = default;
	/** Move-assignment operator. */
	StateStore& operator=(StateStore&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Get value.

		@returns Value.
	*/
	constexpr value_type
	get_value() const noexcept {
		return m_value;
	}

	/**
		Test value of state.

		@returns
		- @c true if the state is enabled;
		- @c false if the state is disabled.
		@param state State to test.
	*/
	constexpr bool
	test(
		state_type const state
	) const noexcept {
		return m_value & static_cast<value_type const>(state);
	}
/// @}

/** @name Operations */ /// @{
	/**
		Enable state.

		@param state State to enable.
	*/
	/*constexpr*/ void
	enable(
		state_type const state
	) noexcept {
		m_value |= static_cast<value_type const>(state);
	}

	/**
		Disable state.

		@param state State to disable.
	*/
	/*constexpr*/ void
	disable(
		state_type const state
	) noexcept {
		m_value &= ~static_cast<value_type const>(state);
	}

	/**
		Enable or disable state.

		@param state State to enable or disable.
		@param enable Whether to enable or disable the state.
	*/
	/*constexpr*/ void
	set(
		state_type const state,
		bool const enable
	) noexcept {
		enable
			? this->enable(state)
			: this->disable(state)
		;
	}

	/**
		Clear all states.
	*/
	/*constexpr*/ void
	clear() noexcept {
		m_value = value_type(0);
	}
/// @}
};

/** @} */ // end of doc-group utils

} // namespace duct

#endif // DUCT_STATESTORE_HPP_
