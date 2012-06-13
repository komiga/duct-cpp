/**
@file Parser.hpp
@author Tim Howard

@section LICENSE
Copyright (c) 2010-2012 Tim Howard

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

@section DESCRIPTION

Generic Parser class and associated classes.

@defgroup parser Parser framework
*/

#ifndef DUCT_PARSER_HPP_
#define DUCT_PARSER_HPP_

#include <duct/config.hpp>
#include <duct/char.hpp>
#include <duct/CharBuf.hpp>
#include <duct/IO.hpp>

#include <istream>

namespace duct {

// Forward declarations
class Token;
class Parser;
class ParserHandler;

/**
	@addtogroup parser
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
private:
	DUCT_DISALLOW_COPY_AND_ASSIGN(Token);
	
public:
	/**
		Constructor with type.
		@param type Token type.
	*/
	Token(int type=NULL_TOKEN)
		: m_type(type)
		, m_line(-1)
		, m_column(-1)
		, m_buffer()
	{}
	/**
		Destructor.
	*/
	~Token() {}

	/**
		Set the token's type.
		@param type The token's type.
	*/
	inline void set_type(int type) { m_type=type; }
	/**
		Get the token's type.
		@returns The token's type.
	*/
	inline int get_type() const { return m_type; }
	/**
		Set the token's line number.
		@param line The line of the token.
	*/
	inline void set_line(int line) { m_line=line; }
	/**
		Get the token's line number.
		@returns The token's line number.
	*/
	inline int get_line() const { return m_line; }
	/**
		Set the token's column number.
		@param column The token's column number.
	*/
	inline void set_column(int column) { m_column=column; }
	/**
		Get the token's column number.
		@returns The token's column number.
	*/
	inline int get_column() const { return m_column; }
	/**
		Set the line and column of the token.
		@param line Line position.
		@param column Column position.
	*/
	void set_position(int line, int column) {
		m_line=line;
		m_column=column;
	}

	/** @{ */
	/**
		Get the token's character buffer.
		@returns The token's buffer.
	*/
	CharBuf& get_buffer() { return m_buffer; }
	CharBuf const& get_buffer() const { return m_buffer; }
	/** @} */

	/**
		Reset the token.
		@note This will reset the character buffer and set the type (and call @c set_position(-1, -1) if @c position==true).
		@param type Type to reset to (generic @c NULL_TOKEN is provided for invalidity purposes).
		@param position Whether to reset the token's position; if @c true, will call @c set_position(-1, -1).
	*/
	void reset(int type, bool position) {
		m_buffer.reset();
		set_type(type);
		if (position) {
			set_position(-1, -1);
		}
	}

protected:
	int m_type; /**< Type. */
	int m_line; /**< Line position. */
	int m_column; /**< Column position. */
	CharBuf m_buffer; /**< Character buffer. */
};

/**
	Base parser class.
	@note This class and deriving classes shall not take ownership of input streams.
*/
class Parser {
private:
	DUCT_DISALLOW_COPY_AND_ASSIGN(Parser);

public:
	/**
		Default constructor.
	*/
	Parser()
		: m_line(1)
		, m_column(1)
		, m_curchar(CHAR_EOF)
		, m_peekchar(CHAR_EOF)
		, m_peeked(false)
		, m_stream(nullptr)
		, m_stream_ctx()
	{}
	/**
		Destructor.
	*/
	virtual ~Parser() {}

// properties
	/**
		Get current line.
		@returns Current line position.
	*/
	virtual int get_line() const { return m_line; }
	/**
		Get current column.
		@returns Current column position.
	*/
	virtual int get_column() const { return m_column; }

	/** @{ */
	/**
		Get token.
		@returns Current token.
	*/
	virtual Token& get_token() { return m_token; }
	virtual Token const& get_token() const { return m_token; }
	/** @} */

	/**
		Get input stream.
		@returns Current input stream.
	*/
	virtual std::istream* get_stream() { return m_stream; }
	/**
		Get stream context.
		@returns Current stream context.
	*/
	virtual IO::StreamContext const get_stream_context() const { return m_stream_ctx; }

	/**
		Set handler.
		@param handler New handler.
	*/
	virtual void set_handler(ParserHandler* handler)=0;
	/**
		Get handler.
		@returns Current handler.
	*/
	virtual ParserHandler* get_handler()=0;

// operations
	/**
		@c initialize(std::istream&,Encoding const,Endian const) with properties from a StreamContext.
		@returns @c true if the parser was initialized, or @c false if an error occurred.
		@param stream Input stream.
		@param ctx IO::StreamContext to copy.
	*/
	bool initialize(std::istream& stream, IO::StreamContext const& ctx) {
		return initialize(stream, ctx.get_encoding(), ctx.get_endian());
	}
	/**
		Initialize the parser.
		This will @c reset() the current state and call @c next_char() to get the first character in the stream.
		@note The input stream is not owned by the parser; its lifetime must be guaranteed by the callee until @c reset() is called.
		@returns @c true if the parser was initialized, or @c false if an error occurred (in the base implementation: if @c stream.good()==false).
		@param stream Input stream.
		@param encoding Encoding of stream.
		@param endian Endian of stream.
	*/
	virtual bool initialize(std::istream& stream, Encoding const encoding, Endian const endian) {
		reset();
		m_stream=&stream;
		m_stream_ctx.set_properties(encoding, endian);
		if (m_stream->good()) {
			next_char(); // Get the first character
			return true;
		} else {
			return false;
		}
	}
	/**
		Reset state.
		@note This will nullify the input stream.
		@note The parser's StreamContext is not reset.
	*/
	virtual void reset() {
		m_line=m_column=1;
		m_curchar=m_peekchar=CHAR_EOF;
		m_peeked=false;
		m_token.reset(NULL_TOKEN, true);
		m_stream=nullptr;
	}

	/**
		Get the next character from the stream and advance the parser's position.
		@note
		- If @c m_peeked==true, @c m_curchar is set to @c m_peekchar.
		- If there is no more data in the input stream, @c m_curchar is set to @c CHAR_EOF.
		@returns The next character from the input stream.
	*/
	virtual char32 next_char() {
		DUCT_ASSERTP(nullptr!=m_stream, this, "Input stream must not be null");
		if (CHAR_NEWLINE==m_curchar) {
			m_line++;
			m_column=1;
		}
		if (m_peeked) {
			m_curchar=m_peekchar;
			m_peeked=false;
		} else if (!m_stream->eof()) {
			m_curchar=m_stream_ctx.read_char(*m_stream, CHAR_SENTINEL);
		} else {
			m_curchar=CHAR_EOF;
		}
		if (CHAR_CARRIAGERETURN==m_curchar || CHAR_SENTINEL==m_curchar) {
			return next_char();
		} else if (CHAR_EOF!=m_curchar) {
			m_column++;
		}
		return m_curchar;
	}
	/**
		Peek the next code point in the stream, without advancing the parser's position.
		@note Will peek multiple points if invalid code points are encountered.
		@returns The next character in the input stream.
	*/
	virtual char32 peek_char() {
		DUCT_ASSERTP(nullptr!=m_stream, this, "Input stream must not be null");
		if (!m_peeked) {
			if (!m_stream->eof()) {
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
		Skip data in the stream until the given character is met.
		@returns @c true if the given character was met, or @c false if @c CHAR_EOF was met.
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
		@returns @c true if the end of the line was met, or @c false if @c CHAR_EOF was met.
		@sa skip_to(char32 const)
	*/
	virtual bool skip_to_eol() {
		return skip_to(CHAR_NEWLINE);
	}

	/**
		Determine and set the current token type based on the current character.
		@returns The current token.
	*/
	virtual Token& next_token()=0;
	/**
		Read the current token.
	*/
	virtual void read_token()=0;
	/**
		Parse the next token in the stream.
		@returns @c true if there is more data in the input stream, or @c false if there is no more data in the stream (generally meaning an EOF token was met).
	*/
	virtual bool parse()=0;
	
protected:
	int m_line; /**< Line position. */
	int m_column; /**< Column position. */
	char32 m_curchar; /**< Current character. */
	char32 m_peekchar; /**< Peeked character. */
	bool m_peeked; /**< Whether a character has been peeked. */
	Token m_token; /**< Current token. */
	std::istream* m_stream; /**< Current stream. */
	IO::StreamContext m_stream_ctx; /**< Stream context. */
};

/**
	Base handler class for a @c Parser.
*/
class ParserHandler {
private:
	DUCT_DISALLOW_COPY_AND_ASSIGN(ParserHandler);

public:
	/**
		Constructor.
	*/
	ParserHandler()
	{}
	/**
		Destructor.
	*/
	virtual ~ParserHandler() {}

	/**
		Set parser.
		@param parser New parser.
	*/
	virtual void set_parser(Parser& parser)=0;
	/**
		Get parser.
		@returns Current parser.
	*/
	virtual Parser& get_parser()=0;
	/**
		Clean states.
		@c This should @em not not call @c reset() on the parser.
	*/
	virtual void clean()=0;
	/**
		Process parser's input stream.
		@returns @c true on success, or @c false if an error occurred.
	*/
	virtual bool process()=0;
	/**
		Finish handling the parser's input stream.
		@note This is called when there is no more data in the input stream.
	*/
	virtual void finish()=0;

	/**
		Handle a token.
		@note This is called from @c Parser::read_token().
		@param token Token to handle.
	*/
	virtual void handle_token(Token& token)=0;
};

/** @} */ // end of doc-group parser

} // namespace duct

#endif // DUCT_PARSER_HPP_
