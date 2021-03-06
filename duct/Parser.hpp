/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Parser class.
*/

#pragma once

#include "./config.hpp"
#include "./char.hpp"
#include "./IO/StreamContext.hpp"
#include "./Token.hpp"

#include <utility>
#include <istream>

namespace duct {

// Forward declarations
class Parser;

/**
	@addtogroup parser
	@{
*/

/**
	Base parser class.

	@note This class and deriving classes shall not take ownership of
	input streams.
*/
class Parser {
protected:
	signed m_line{1}; /**< Line position. */
	signed m_column{0}; /**< Column position. */
	char32 m_curchar{CHAR_EOF}; /**< Current code point. */
	char32 m_peekchar{CHAR_EOF}; /**< Peeked code point. */
	bool m_peeked{false}; /**< Whether a code point has been peeked. */
	Token m_token{}; /**< Current token. */
	std::istream* m_stream{nullptr}; /**< Current stream. */
	IO::StreamContext m_stream_ctx{}; /**< Stream context. */

public:
/** @name Constructor and destructor */ /// @{
	/** Default constructor. */
	Parser() = default;

	/**
		Constructor with StreamContext.

		@param context StreamContext to copy.
	*/
	explicit
	Parser(
		IO::StreamContext context
	) noexcept
		: m_stream_ctx(std::move(context))
	{}

	/** Copy constructor (deleted). */
	Parser(Parser const&) = delete;
	/** Move constructor. */
	Parser(Parser&&) = default;
	/** Destructor. */
	inline virtual ~Parser() = 0;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator (deleted). */
	Parser& operator=(Parser const&) = delete;
	/** Move assignment operator. */
	Parser& operator=(Parser&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Get current line.

		@returns The current line position.
	*/
	signed
	line() const noexcept {
		return m_line;
	}

	/**
		Get current column.

		@returns The current column position.
	*/
	signed
	column() const noexcept {
		return m_column;
	}

	/**
		Get token.

		@returns The current token.
	*/
	Token&
	token() noexcept {
		return m_token;
	}

	/** @copydoc token() */
	Token const&
	token() const noexcept {
		return m_token;
	}

	/**
		Get input stream.

		@returns The current input stream.
	*/
	std::istream*
	stream() noexcept {
		return m_stream;
	}

	/**
		Get stream context.

		@returns The current stream context.
	*/
	IO::StreamContext&
	stream_context() noexcept {
		return m_stream_ctx;
	}

	/** @copydoc stream_context() */
	IO::StreamContext const&
	stream_context() const noexcept {
		return m_stream_ctx;
	}
/// @}

/** @name State */ /// @{
	/**
		Initialize.

		@note This will @c reset() the current state and call @c next_char().
		@note The input stream is not owned by the parser; its lifetime must
		be guaranteed by the callee until @c reset() is called.

		@returns
		- @c true if the parser was initialized; or
		- @c false if an error occurred (in the base implementation: if
		  @c stream.good()==false).
		@param stream Input stream.
	*/
	virtual bool
	initialize(
		std::istream& stream
	) {
		reset();
		if (stream.good()) {
			m_stream = &stream;
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
	virtual void
	reset() noexcept {
		m_line = 1;
		m_column = 0;
		m_curchar = m_peekchar = CHAR_EOF;
		m_peeked = false;
		m_token.reset(NULL_TOKEN, true);
		m_stream = nullptr;
	}
/// @}

/** @name Operations */ /// @{
	/**
		Get the next code point from the stream and advance the parser's
		position.

		@note If @c m_peeked==true, @c m_curchar is set to @c m_peekchar.
		@note If there is no more data in the input stream, @c m_curchar is set
		to @c CHAR_EOF.

		@returns The next code point from the input stream.
	*/
	virtual char32
	next_char() {
		DUCT_DEBUG_ASSERTP(
			nullptr != m_stream, this,
			"Input stream must not be null"
		);
		if (CHAR_NEWLINE == m_curchar) {
			++m_line;
			m_column = 0;
		}
		if (m_peeked) {
			m_curchar = m_peekchar;
			m_peeked = false;
		} else if (m_stream->good()) {
			m_curchar = m_stream_ctx.read_char(*m_stream, CHAR_SENTINEL);
		} else {
			m_curchar = CHAR_EOF;
		}
		if (CHAR_CARRIAGERETURN == m_curchar || CHAR_SENTINEL == m_curchar) {
			return next_char();
		} else if (CHAR_EOF != m_curchar) {
			++m_column;
		}
		return m_curchar;
	}
	/**
		Peek the next code point in the stream, without advancing the parser's
		position.

		@note Will peek multiple points if invalid code points are encountered.

		@returns The next code point in the input stream.
	*/
	virtual char32
	peek_char() {
		DUCT_DEBUG_ASSERTP(
			nullptr != m_stream, this,
			"Input stream must not be null"
		);
		if (!m_peeked) {
			if (m_stream->good()) {
				m_peekchar = m_stream_ctx.read_char(*m_stream, CHAR_SENTINEL);
				if (!m_stream->good()) {
					m_peekchar = CHAR_EOF;
				} else if (CHAR_SENTINEL == m_peekchar) {
					return peek_char();
				}
			} else {
				m_peekchar = CHAR_EOF;
			}
			m_peeked = true;
		}
		return m_peekchar;
	}

	/**
		Skip data in the input stream until a code point is met.
		@note This will check the current code point before stepping
		the stream.

		@returns
		- @c true if @c cp was met (even if @c cp==CHAR_EOF); or
		- @c false if @c CHAR_EOF was met (only if @c cp!=CHAR_EOF).
		@param cp Code point to skip to.
	*/
	virtual bool
	skip_to(
		char32 const cp
	) {
		while (CHAR_EOF != m_curchar && cp != m_curchar) {
			next_char();
		}
		return cp == m_curchar;
	}
	/**
		Skip to the end of the line.

		@note This is an alias for @c skip_to(CHAR_NEWLINE).
		@returns
		- @c true if the end of the line was met; or
		- @c false if @c CHAR_EOF was met.
		@sa skip_to(char32 const)
	*/
	virtual bool
	skip_to_eol() {
		return skip_to(CHAR_NEWLINE);
	}

	/**
		Parse the next token in the stream.

		@returns
		- @c true if there is more data to parse; or
		- @c false if there is no more data to parse (generally meaning an
		  EOF token was met).
	*/
	virtual bool
	parse() = 0;

	/**
		Determine and set the current token type based on the current code
		point.
	*/
	virtual void
	discern_token() = 0;
	/**
		Read the current token.
	*/
	virtual void
	read_token() = 0;
	/**
		Handle the current token.

		@note This is called from @c read_token().
	*/
	virtual void
	handle_token() = 0;
/// @}
};
inline Parser::~Parser() = default;

/** @} */ // end of doc-group parser

} // namespace duct
