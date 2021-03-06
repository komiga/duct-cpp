/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Token class.
*/

#pragma once

#include "./config.hpp"
#include "./CharBuf.hpp"

namespace duct {

// Forward declarations
class Token;

/**
	@addtogroup parser
	@{
*/

enum : signed {
	/** Null/invalid Token type constant. */
	NULL_TOKEN = static_cast<signed>(0xCA11ACAB)
};

/**
	Generic token.
*/
class Token {
protected:
	signed m_type{NULL_TOKEN}; /**< Type. */
	signed m_line{-1}; /**< Line position. */
	signed m_column{-1}; /**< Column position. */
	CharBuf m_buffer{}; /**< Character buffer. */

public:
/** @name Constructors and destructor */ /// @{
	/** Construct @c NULL_TOKEN. */
	Token() = default;

	/**
		Construct with type.

		@param type Token type.
	*/
	explicit
	Token(
		signed const type
	) noexcept
		: m_type(type)
	{}

	/**
		Construct with type and buffer capacity.

		@param type Token type.
		@param capacity Capacity of buffer.
	*/
	Token(
		signed const type,
		std::size_t const capacity
	) noexcept
		: m_type(type)
		, m_buffer(capacity)
	{}

	/** Copy constructor. */
	Token(Token const&) = default;
	/** Move constructor. */
	Token(Token&&) = default;
	/** Destructor. */
	virtual ~Token() = default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator. */
	Token& operator=(Token const&) = default;
	/** Move assignment operator. */
	Token& operator=(Token&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Set type.

		@param type New type.
	*/
	void
	set_type(
		signed const type
	) noexcept {
		m_type = type;
	}

	/**
		Get type.

		@returns The current type.
	*/
	signed
	type() const noexcept {
		return m_type;
	}

	/**
		Set position.

		@param line New line position.
		@param column New column position.
	*/
	void
	set_position(
		signed const line,
		signed const column
	) noexcept {
		m_line = line;
		m_column = column;
	}

	/**
		Set line position.

		@param line New line position.
	*/
	void
	set_line(
		signed const line
	) noexcept {
		m_line = line;
	}

	/**
		Get line position.

		@returns The current line position.
	*/
	signed
	line() const noexcept {
		return m_line;
	}

	/**
		Set column position.

		@param column New column position.
	*/
	void
	set_column(
		signed const column
	) noexcept {
		m_column = column;
	}

	/**
		Get column position.

		@returns The current column position.
	*/
	signed
	column() const noexcept {
		return m_column;
	}

	/**
		Get character buffer.

		@returns The token's character buffer.
	*/
	CharBuf&
	buffer() noexcept {
		return m_buffer;
	}

	/** @copydoc buffer() */
	CharBuf const&
	buffer() const noexcept {
		return m_buffer;
	}

	/**
		Test the token's type.

		@returns @c true if @c type()==type.
		@param type Type to test against.
	*/
	bool
	is_type(
		signed const type
	) const noexcept {
		return type == m_type;
	}

	/**
		Check if the token is @c NULL_TOKEN.
	*/
	bool
	is_null() const noexcept {
		return is_type(NULL_TOKEN);
	}

	/**
		Check if the buffer is empty.
	*/
	bool
	empty() const noexcept {
		return m_buffer.empty();
	}

	/**
		Get the buffer size.
	*/
	std::size_t
	size() const noexcept {
		return m_buffer.size();
	}
/// @}

/** @name Operations */ /// @{
	/**
		Reset the token.

		@note This will reset the character buffer and set the type
		(and call @c set_position(-1, -1) if @c position==true).

		@param type Type to reset to (generic @c NULL_TOKEN is
		provided for invalidity purposes).
		@param position Whether to reset the token's position;
		if @c true, will call @c set_position(-1, -1).
	*/
	void
	reset(
		signed const type,
		bool const position
	) noexcept {
		m_buffer.reset();
		set_type(type);
		if (position) {
			set_position(-1, -1);
		}
	}
/// @}
};

/** @} */ // end of doc-group parser

} // namespace duct
