/**
@file parser.hpp
@author Tim Howard

@section LICENSE
Copyright (c) 2010 Tim Howard

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

Implements component parts:
<ul>
	<li>ductParser</li>
	<ul>
		<li>class Token</li>
		<li>class Parser</li>
		<li>class ParserHandler</li>
	</ul>
</ul>
*/

#ifndef _DUCT_PARSER_HPP
#define _DUCT_PARSER_HPP

#include <unicode/unistr.h>
#include <duct/config.hpp>
#include <duct/charbuf.hpp>
#include <duct/stream.hpp>

namespace duct {

/**
	NULL/reset token type constant.
*/
const int NULL_TOKEN=0xCA11ACAB;

/**
	Generic token class.
*/
class DUCT_API Token {
public:
	/**
		Set the token's type.
		@returns Nothing.
		@param type The token's type.
	*/
	void setType(int type);
	/**
		Get the token's type.
		@returns The token's type.
	*/
	int getType() const;
	/**
		Set the token's line number.
		@returns Nothing.
		@param line The line of the token.
	*/
	void setLine(int line);
	/**
		Get the token's line number.
		@returns The token's line number.
	*/
	int getLine() const;
	/**
		Set the token's column number.
		@returns Nothing.
		@param column The token's column number.
	*/
	void setColumn(int column);
	/**
		Get the token's column number.
		@returns The token's column number.
	*/
	int getColumn() const;
	/**
		Set the line and column of the token.
		@returns Nothing.
		@param line The line of the token.
		@param column The column of the token.
	*/
	void setPosition(int line, int column);
	/**
		Add the given character to the token's buffer.
		@returns Nothing.
		@param c The character to add.
	*/
	void addChar(UChar32 c);
	/**
		Reset the token.
		This will clear the token's buffer and set the token's type.
		@returns Nothing.
		@param type The token's type (should be a null token constant depending on use).
	*/
	void reset(int type=NULL_TOKEN);
	/**
		Compare every character in the token's buffer to the given character.
		@returns true if the given character matches every character in the token's buffer, or false otherwise.
		@param c The character to compare against.
	*/
	bool compare(UChar32 c) const;
	/**
		Compare every character in the token's buffer to each character in the given set.
		@returns true if every character in the token's buffer matches a character in the given set, or false otherwise.
		@param charset The character set to compare against.
	*/
	bool compare(const CharacterSet& charset) const;
	/**
		Convert the token's buffer to a string.
		If conversion fails, <em>str</em> is unmodified.
		@returns true if the string was converted, or false on cache failure (likely because of an invalid surrogate pair).
		@param str The output string.
	*/
	bool toString(UnicodeString& str);
	/**
		Convert the token's buffer as a string.
		The string returned is only a snapshot of the buffer's current state, and will be emptied upon buffer reset or caching a new buffer state, or bogus'd on cache failure.
		@returns A reference to the cached string (which will be bogus if conversion failed).
	*/
	const UnicodeString& toString();
	/**
		Convert the token's buffer to a 32-bit integer.
		@returns The buffer as an integer, or 0 if the buffer was not a numeric value.
	*/
	int32_t toInt();
	/**
		Convert the token's buffer to a 32-bit integer (with error-return).
		If the conversion fails, <em>value</em> is unmodified.
		@returns true if the buffer was a numeric value (<em>value</em> is set), or false otherwise.
		@param value Output value.
	*/
	bool toInt(int32_t& value);
	/**
		Convert the token's buffer to a 64-bit integer.
		@returns The buffer as a long, or 0 if the buffer was not a numeric value.
	*/
	int64_t toLong();
	/**
		Convert the token's buffer to a long (with error-return).
		If the conversion fails, <em>value</em> is unmodified.
		@returns true if the buffer was a numeric value (<em>value</em> is set), or false otherwise.
		@param value Output value.
	*/
	bool toLong(int64_t& value);
	/**
		Convert the token's buffer to a float.
		@returns The buffer as a float, or 0.0 if the buffer was not a numeric value.
	*/
	float toFloat();
	/**
		Convert the token's buffer to a float (with error-return).
		If the conversion fails, <em>value</em> is unmodified.
		@returns true if the buffer was a numeric value (<em>value</em> is set), or false otherwise.
		@param value Output value.
	*/
	bool toFloat(float& value);
	/**
		Convert the token's buffer to a double.
		@returns The buffer as a double, or 0.0 if the buffer was not a numeric value.
	*/
	double toDouble();
	/**
		Convert the token's buffer to a double (with error-return).
		If the conversion fails, <em>value</em> is unmodified.
		@returns true if the buffer was a numeric value (<em>value</em> is set), or false otherwise.
		@param value Output value.
	*/
	bool toDouble(double& value);
	
protected:
	int _type;
	int _line, _column;
	CharBuf _buffer;
};

// forward declaration
class ParserHandler;

/**
	Base parser class.
*/
class DUCT_API Parser {
public:
	/**
		Constructor.
	*/
	Parser();
	/**
		Destructor.
	*/
	virtual ~Parser();
	/**
		Get the parser's current line.
		@returns The parser's current line.
	*/
	virtual int getLine();
	/**
		Get the parser's current column.
		@returns The parser's current column.
	*/
	virtual int getColumn();
	/**
		Set the parser's handler.
		@returns Nothing.
		@param handler The parser's new handler.
	*/
	virtual void setHandler(ParserHandler* handler)=0;
	/**
		Get the parser's handler.
		@returns The parser's handler.
	*/
	virtual ParserHandler* getHandler()=0;
	/**
		Get the parser's token.
		@returns The parser's token.
	*/
	virtual Token& getToken();
	/**
		Get the parser's stream.
		@returns The parser's stream (which may be NULL).
	*/
	virtual Stream* getStream();
	/**
		Initialize the parser with the given stream.
		This reset()s the current state. Calls nextChar() to get the first character in the parser (since the reset will clear the current character).
		@returns true if the stream was initialized with the given stream, or false if an error occurred (likely if the stream is NULL).
		@param stream The stream to initialize with.
	*/
	virtual bool initWithStream(Stream* stream);
	/**
		Reset the parser's state.
		This will reset the Line, Column, Token and Stream properties and the curchar and peekchar fields.
		@returns Nothing.
	*/
	virtual void reset();
	/**
		Get the next character from the stream.
		If peekchar is not equal to CHAR_EOF, curchar is set to peekchar, and then peekchar is reset to CHAR_EOF. If there is no more data in the stream, curchar is set to CHAR_EOF.
		@returns The next character from the stream.
	*/
	virtual UChar32 nextChar();
	/**
		Peek the next character in the stream, without advancing the parser's position.
		This sets the peekchar field.
		@returns The peeked character.
	*/
	virtual UChar32 peekChar();
	/**
		Skip data in the stream until the given character is met.
		@returns true if the given character was met, or false if CHAR_EOF was met.
		@param c The character to skip to.
	*/
	virtual bool skipToChar(UChar32 c);
	/**
		Skip to the end of the line.
		This is an alias for skipToChar('\n');
		@returns true if the end of the line was met, or false if CHAR_EOF was met.
	*/
	virtual bool skipToEOL();
	/**
		Determine and set the current token based on the current character.
		@returns The next token.
	*/
	virtual Token& nextToken()=0;
	/**
		Read the current token.
		@returns Nothing.
	*/
	virtual void readToken()=0;
	/**
		Get and read the next token.
		@returns true if there is more data in the parser's stream, or false if there is no more data in the stream (generally meaning an EOF token was met).
	*/
	virtual bool parse()=0;
	
protected:
	int _line, _column;
	UChar32 _curchar, _peekchar;
	bool _peeked;
	Token _token;
	Stream* _stream;
};

/**
	Base handler class for a Parser.
*/
class DUCT_API ParserHandler {
public:
	/**
		Destructor.
	*/
	virtual ~ParserHandler();
	/**
		Set the handler's parser.
		@returns Nothing.
		@param parser The handler's parser.
	*/
	virtual void setParser(Parser* parser)=0;
	/**
		Get the handler's parser.
		@returns The handler's parser.
	*/
	virtual Parser* getParser()=0;
	/**
		Clean the handler's state.
		This should <em>not</em> not reset() the parser.
		@returns Nothing.
	*/
	virtual void clean()=0;
	/**
		Process the parser's stream.
		@returns true on success, or false if an error occurred.
	*/
	virtual bool process()=0;
	/**
		Handle the given token.
		This is called from Parser::readToken().
		@returns Nothing.
		@param token The token to handle.
	*/
	virtual void handleToken()=0;
	/**
		Finish handling the parser's stream.
		Called when there is no more data in the parser's stream.
		@returns Nothing.
	*/
	virtual void finish()=0;
};

} // namespace duct

#endif // _DUCT_PARSER_HPP

