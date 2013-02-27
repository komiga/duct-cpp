/**
@file Token.hpp
@brief Token class.

@author Tim Howard
@copyright 2010-2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_TOKEN_HPP_
#define DUCT_TOKEN_HPP_

#include "./config.hpp"
#include "./CharBuf.hpp"

namespace duct {

// Forward declarations
class Token;

/**
	@addtogroup parser
	@{
*/

enum {
	/** Null/invalid Token type constant. */
	NULL_TOKEN=(signed)0xCA11ACAB
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
	Token()=default;
	/**
		Construct with type.
		@param type Token type.
	*/
	explicit Token(signed const type)
		: m_type{type}
	{}
	/** Copy constructor (deleted). */
	Token(Token const&)=delete;
	/** Move constructor. */
	Token(Token&&)=default;
	/** Destructor. */
	virtual ~Token()=default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator (deleted). */
	Token& operator=(Token const&)=delete;
	/** Move assignment operator. */
	Token& operator=(Token&&)=default;
/// @}

/** @name Properties */ /// @{
	/**
		Set type.
		@param type New type.
	*/
	void set_type(signed const type) { m_type=type; }
	/**
		Get type.
		@returns The current type.
	*/
	signed get_type() const { return m_type; }

	/**
		Set position.
		@param line New line position.
		@param column New column position.
	*/
	void set_position(signed const line, signed const column) {
		m_line=line;
		m_column=column;
	}

	/**
		Set line position.
		@param line New line position.
	*/
	void set_line(signed const line) { m_line=line; }
	/**
		Get line position.
		@returns The current line position.
	*/
	signed get_line() const { return m_line; }

	/**
		Set column position.
		@param column New column position.
	*/
	void set_column(signed const column) { m_column=column; }
	/**
		Get column position.
		@returns The current column position.
	*/
	signed get_column() const { return m_column; }

	/**
		Get character buffer.
		@returns The token's character buffer.
	*/
	CharBuf& get_buffer() { return m_buffer; }
	/** @copydoc get_buffer() */
	CharBuf const& get_buffer() const { return m_buffer; }

	/**
		Test the token's type.
		@returns @c true if @c get_type()==type.
		@param type Type to test against.
	*/
	bool is_type(signed const type) const { return type==m_type; }
/// @}

/** @name Operations */ /// @{
	/**
		Reset the token.
		@note This will reset the character buffer and set the type (and
		call @c set_position(-1, -1) if @c position==true).
		@param type Type to reset to (generic @c NULL_TOKEN is
		provided for invalidity purposes).
		@param position Whether to reset the token's position;
		if @c true, will call @c set_position(-1, -1).
	*/
	void reset(signed const type, bool const position) {
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

#endif // DUCT_TOKEN_HPP_
