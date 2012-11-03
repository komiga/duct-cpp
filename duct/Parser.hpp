/**
@file Parser.hpp
@brief Generic Parser and associated classes.

@author Tim Howard
@copyright 2010-2012 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_PARSER_HPP_
#define DUCT_PARSER_HPP_

#include "./config.hpp"
#include "./char.hpp"
#include "./CharBuf.hpp"
#include "./IO.hpp"

#include <istream>

namespace duct {

// Forward declarations
class Token;
class Parser;

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
	Token(int const type)
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
		Set position.
		@param line New line position.
		@param column New column position.
	*/
	void set_position(int const line, int const column) {
		m_line=line;
		m_column=column;
	}

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
	bool is_type(int type) const { return type==m_type; }
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

/**
	Base parser class.
	@note This class and deriving classes shall not take ownership of input streams.
*/
class Parser {
protected:
	int m_line; /**< Line position. */
	int m_column; /**< Column position. */
	char32 m_curchar; /**< Current character. */
	char32 m_peekchar; /**< Peeked character. */
	bool m_peeked; /**< Whether a character has been peeked. */
	Token m_token; /**< Current token. */
	std::istream* m_stream; /**< Current stream. */
	IO::StreamContext m_stream_ctx; /**< Stream context. */

private:
	DUCT_DISALLOW_COPY_AND_ASSIGN(Parser);

public:
/** @name Constructor and destructor */ /// @{
	/**
		Default constructor.
	*/
	Parser()
		: m_line(1)
		, m_column(0)
		, m_curchar(CHAR_EOF)
		, m_peekchar(CHAR_EOF)
		, m_peeked(false)
		, m_stream(nullptr)
		, m_stream_ctx()
	{}
	/**
		Constructor with StreamContext properties.
		@param encoding Encoding to use for StreamContext.
		@param endian Endian to use for StreamContext.
	*/
	Parser(Encoding const encoding, Endian const endian)
		: m_line(1)
		, m_column(0)
		, m_curchar(CHAR_EOF)
		, m_peekchar(CHAR_EOF)
		, m_peeked(false)
		, m_stream(nullptr)
		, m_stream_ctx(encoding, endian)
	{}
	/**
		Constructor with StreamContext.
		@param context StreamContext to copy.
	*/
	Parser(IO::StreamContext const& context)
		: m_line(1)
		, m_column(0)
		, m_curchar(CHAR_EOF)
		, m_peekchar(CHAR_EOF)
		, m_peeked(false)
		, m_stream(nullptr)
		, m_stream_ctx(context)
	{}
	/**
		Destructor.
	*/
	virtual ~Parser() {}
/// @}

/** @name Properties */ /// @{
	/**
		Get current line.
		@returns The current line position.
	*/
	int get_line() const { return m_line; }
	/**
		Get current column.
		@returns The current column position.
	*/
	int get_column() const { return m_column; }

	/**
		Get token.
		@returns The current token.
	*/
	Token& get_token() { return m_token; }
	/** @copydoc get_token() */
	Token const& get_token() const { return m_token; }

	/**
		Get input stream.
		@returns The current input stream.
	*/
	std::istream* get_stream() { return m_stream; }
	/**
		Get stream context.
		@returns The current stream context.
	*/
	IO::StreamContext& get_stream_context() { return m_stream_ctx; }
	/** @copydoc get_stream_context() */
	IO::StreamContext const& get_stream_context() const { return m_stream_ctx; }
/// @}

/** @name State */ /// @{
	/**
		Initialize the parser.
		@note This will @c reset() the current state and call @c next_char().
		@note The input stream is not owned by the parser; its lifetime must be guaranteed by the callee until @c reset() is called.
		@returns
			@c true if the parser was initialized; or
			@c false if an error occurred (in the base implementation: if @c stream.good()==false)
		@param stream Input stream.
	*/
	virtual bool initialize(std::istream& stream) {
		reset();
		if (stream.good()) {
			m_stream=&stream;
			next_char(); // Get the first character
			return true;
		} else {
			return false;
		}
	}

	/**
		Reset all state.
		@note This will nullify the input stream.
		@note The parser's StreamContext is not reset.
	*/
	virtual void reset() {
		m_line=1;
		m_column=0;
		m_curchar=m_peekchar=CHAR_EOF;
		m_peeked=false;
		m_token.reset(NULL_TOKEN, true);
		m_stream=nullptr;
	}
/// @}

/** @name Operations */ /// @{
	/**
		Get the next character from the stream and advance the parser's position.
		@note If @c m_peeked==true, @c m_curchar is set to @c m_peekchar.
		@note If there is no more data in the input stream, @c m_curchar is set to @c CHAR_EOF.
		@returns The next character from the input stream.
	*/
	virtual char32 next_char() {
		DUCT_DEBUG_ASSERTP(nullptr!=m_stream, this, "Input stream must not be null");
		if (CHAR_NEWLINE==m_curchar) {
			++m_line;
			m_column=0;
		}
		if (m_peeked) {
			m_curchar=m_peekchar;
			m_peeked=false;
		} else if (m_stream->good()) {
			m_curchar=m_stream_ctx.read_char(*m_stream, CHAR_SENTINEL);
		} else {
			m_curchar=CHAR_EOF;
		}
		if (CHAR_CARRIAGERETURN==m_curchar || CHAR_SENTINEL==m_curchar) {
			return next_char();
		} else if (CHAR_EOF!=m_curchar) {
			++m_column;
		}
		return m_curchar;
	}
	/**
		Peek the next code point in the stream, without advancing the parser's position.
		@note Will peek multiple points if invalid code points are encountered.
		@returns The next character in the input stream.
	*/
	virtual char32 peek_char() {
		DUCT_DEBUG_ASSERTP(nullptr!=m_stream, this, "Input stream must not be null");
		if (!m_peeked) {
			if (m_stream->good()) {
				m_peekchar=m_stream_ctx.read_char(*m_stream, CHAR_SENTINEL);
				if (!m_stream->good()) {
					m_peekchar=CHAR_EOF;
				} else if (CHAR_SENTINEL==m_peekchar) {
					return peek_char();
				}
			} else {
				m_peekchar=CHAR_EOF;
			}
			m_peeked=true;
		}
		return m_peekchar;
	}

	/**
		Skip data in the input stream until a code point is met.
		@note This will check the current character before stepping the stream.
		@returns
			@c true if @c cp was met (even if @c cp==CHAR_EOF); or
			@c false if @c CHAR_EOF was met and only if @c cp!=CHAR_EOF
		@param cp Code point to skip to.
	*/
	virtual bool skip_to(char32 const cp) {
		while (CHAR_EOF!=m_curchar && cp!=m_curchar) {
			next_char();
		}
		return cp==m_curchar;
	}
	/**
		Skip to the end of the line.
		@note This is an alias for @c skip_to(CHAR_NEWLINE).
		@returns
			@c true if the end of the line was met; or
			@c false if @c CHAR_EOF was met
		@sa skip_to(char32 const)
	*/
	virtual bool skip_to_eol() {
		return skip_to(CHAR_NEWLINE);
	}

	/**
		Parse the next token in the stream.
		@returns
			@c true if there is more data to parse; or
			@c false if there is no more data to parse (generally meaning an EOF token was met)
	*/
	virtual bool parse()=0;

	/**
		Determine and set the current token type based on the current character.
	*/
	virtual void discern_token()=0;
	/**
		Read the current token.
	*/
	virtual void read_token()=0;
	/**
		Handle the current token.
		@note This is called from @c read_token().
	*/
	virtual void handle_token()=0;
/// @}
};

/** @} */ // end of doc-group parser

} // namespace duct

#endif // DUCT_PARSER_HPP_
