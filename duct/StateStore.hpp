/**
@file StateStore.hpp
@brief StateStore class.

@author Timothy Howard
@copyright 2010-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_STATESTORE_HPP_
#define DUCT_STATESTORE_HPP_

#include "./config.hpp"
#include "./utility.hpp"

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
	value_type m_value{};

public:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with states.

		@tparam Rest Rest types. See enum_bitor().
		@param first First state to enable.
		@param rest Rest of states to enable.
	*/
	template<
		typename... Rest
	>
	constexpr
	StateStore(
		state_type const first,
		Rest const... rest
	) noexcept
		: m_value(
			enum_bitor<state_type, value_type>(first, rest...)
		)
	{}

	/** Default constructor. */
	StateStore() noexcept = default;
	/** Copy constructor. */
	StateStore(StateStore const&) noexcept = default;
	/** Move constructor. */
	StateStore(StateStore&&) noexcept = default;
	/** Destructor. */
	~StateStore() noexcept = default;
/// @}

/** @name Operators */ /// @{
	/** Copy-assignment operator. */
	StateStore& operator=(StateStore const&) noexcept = default;
	/** Move-assignment operator. */
	StateStore& operator=(StateStore&&) noexcept = default;
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
		Get states by mask.

		@param mask Mask.
	*/
	constexpr state_type
	get_states(
		state_type const mask
	) const noexcept {
		return static_cast<state_type>(
			m_value & static_cast<value_type const>(mask)
		);
	}

	/**
		Test value of states (all-of).

		@note If @a states is equal to @c 0, this will return @c true
		regardless of the state store value.

		@returns
		- @c true if all of the states are enabled;
		- @c false if at least one of the states are disabled.
		@param states States to test.
	*/
	constexpr bool
	test(
		state_type const states
	) const noexcept {
		return
			static_cast<value_type const>(states)
			== (m_value & static_cast<value_type const>(states))
		;
	}

	/**
		Test value of states (any-of).

		@returns
		- @c true if any of the states are enabled;
		- @c false if all of the states are disabled.
		@param states States to test.
	*/
	constexpr bool
	test_any(
		state_type const states
	) const noexcept {
		return m_value & static_cast<value_type const>(states);
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
		Remove states by mask and set states.

		@param mask State mask.
		@param states States to set.
	*/
	/*constexpr*/ void
	set_masked(
		state_type const mask,
		state_type const states
	) noexcept {
		m_value
			= (m_value & ~static_cast<value_type const>(mask))
			| static_cast<value_type const>(states)
		;
	}

	/**
		Remove states by mask.

		Removes states by ANDing @c ~mask.

		@param mask State mask.
	*/
	/*constexpr*/ void
	remove(
		state_type const mask
	) noexcept {
		m_value &= ~static_cast<value_type const>(mask);
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
