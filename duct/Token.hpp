/**
@file Token.hpp
@brief Token class.

@author Tim Howard
@copyright 2010-2012 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_TOKEN_HPP_
#define DUCT_TOKEN_HPP_

#include "./config.hpp"
#include "./CharBuf.hpp"

namespace duct {

// Forward declarations
class Token;

/**
	@defgroup parser Parser framework
	@{
*/

enum {
	/** Null/invalid Token type constant. */
	NULL_TOKEN=(int)0xCA11ACAB
};

/**
	Generic token.
*/
class Token {
protected:
	int m_type; /**< Type. */
	int m_line; /**< Line position. */
	int m_column; /**< Column position. */
	CharBuf m_buffer; /**< Character buffer. */

private:
	DUCT_DISALLOW_COPY_AND_ASSIGN(Token);

public:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with type @c NULL_TOKEN.
	*/
	Token()
		: m_type(NULL_TOKEN)
		, m_line(-1)
		, m_column(-1)
		, m_buffer()
	{}
	/**
		Constructor with type.
		@param type Token type.
	*/
	explicit Token(int const type)
		: m_type(type)
		, m_line(-1)
		, m_column(-1)
		, m_buffer()
	{}
	/**
		Destructor.
	*/
	virtual ~Token() {}
/// @}

/** @name Properties */ /// @{
	/**
		Set type.
		@param type New type.
	*/
	inline void set_type(int const type) { m_type=type; }
	/**
		Get type.
		@returns The current type.
	*/
	inline int get_type() const { return m_type; }

	/**
		Set position.
		@param line New line position.
		@param column New column position.
	*/
	inline void set_position(int const line, int const column) {
		m_line=line;
		m_column=column;
	}

	/**
		Set line position.
		@param line New line position.
	*/
	inline void set_line(int const line) { m_line=line; }
	/**
		Get line position.
		@returns The current line position.
	*/
	inline int get_line() const { return m_line; }

	/**
		Set column position.
		@param column New column position.
	*/
	inline void set_column(int const column) { m_column=column; }
	/**
		Get column position.
		@returns The current column position.
	*/
	inline int get_column() const { return m_column; }

	/**
		Get character buffer.
		@returns The token's character buffer.
	*/
	inline CharBuf& get_buffer() { return m_buffer; }
	/** @copydoc get_buffer() */
	inline CharBuf const& get_buffer() const { return m_buffer; }

	/**
		Test the token's type.
		@returns @c true if @c get_type()==type.
		@param type Type to test against.
	*/
	inline bool is_type(int type) const { return type==m_type; }
/// @}

/** @name Operations */ /// @{
	/**
		Reset the token.
		@note This will reset the character buffer and set the type (and call @c set_position(-1, -1) if @c position==true).
		@param type Type to reset to (generic @c NULL_TOKEN is provided for invalidity purposes).
		@param position Whether to reset the token's position; if @c true, will call @c set_position(-1, -1).
	*/
	void reset(int const type, bool const position) {
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
