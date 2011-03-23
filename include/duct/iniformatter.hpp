/**
@file iniformatter.hpp
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

duct++ IniFormatter class.
*/

#ifndef _DUCT_INIFORMATTER_HPP
#define _DUCT_INIFORMATTER_HPP

#include <duct/config.hpp>
#include <duct/filestream.hpp>
#include <duct/characterset.hpp>
#include <duct/variables.hpp>
#include <exception>

namespace duct {

/**
	IniToken types.
*/
enum IniTokenType {
	NoToken=0,
	
	StringToken,
	QuotedStringToken,
	NumberToken,
	DoubleToken,
	
	EqualsToken,
	NodeToken,
	
	CommentToken,
	EOFToken,
	EOLToken
};

class IniParser; // Forward declaration
class IniParserHandler; // Forward declaration
class IniParserException; // Forward declaration

/**
	IniParser token.
*/
class DUCT_API IniToken {
public:
	friend class IniParser;
	friend class IniParserException;
	
	/**
		Constructor.
	*/
	IniToken();
	
	/**
		Constructor with type.
	*/
	IniToken(IniTokenType type);
	
	/**
		Destructor.
	*/
	~IniToken();
	
	/**
		Reset the token.
		@returns Nothing.
		@param type The token's new type.
	*/
	void reset(IniTokenType type);
	
	/**
		Set the token's beginning position.
		@returns Nothing.
		@param line The line the token starts on.
		@param col The character/column the token starts on.
	*/
	void setBeginningPosition(int line, int col);
	
	/**
		Get the token's type.
		@returns The token's type.
	*/
	IniTokenType getType() const;
	
	/**
		Add the given character to the token's buffer.
		@returns Nothing.
		@param c The character to add to the token.
	*/
	void addChar(UChar32 c);
	
	/**
		Cache the token's buffer as a string.
		@returns Nothing.
	*/
	void cacheString();
	
	/**
		Convert the token to an integer.
		@returns The token as an integer.
	*/
	int32_t asInt();
	
	/**
		Convert the token to a double.
		@returns The token as a double.
	*/
	double asDouble();
	
	/**
		Convert the token to a UnicodeString.
		@returns Nothing.
		@param str The string to store the result in.
	*/
	void asString(UnicodeString& str);
	
	/**
		Convert the token to a UnicodeString.
		@returns A constant reference to the converted string.
	*/
	const UnicodeString& asString();
	
	/**
		Get the token's name.
		@returns The name of the token's type.
	*/
	const char* typeAsString() const;
	
protected:
	IniTokenType _type;
	int _beg_line;
	int _beg_col;
	UChar32* _buffer;
	size_t _bufsize;
	size_t _buflength;
	UnicodeString _bufstring;
	bool _cached;
	
};

/**
	INI parser.
*/
class DUCT_API IniParser {
public:
	friend class IniParserException;
	
	/**
		Constructor.
	*/
	IniParser();
	
	/**
		Constructor with stream.
		The user is responsible for closing the stream.
		@param stream The stream to read from.
	*/
	IniParser(Stream* stream);
	
	/**
		Destructor.
	*/
	~IniParser();
	
	/**
		Initialize the parser with the given stream.
		@returns Nothing.
		@param stream The stream to initialize with.
	*/
	void initWithStream(Stream* stream);
	
	/**
		Set the parser's handler.
		@returns Nothing.
		@param handler The new handler.
	*/
	void setHandler(IniParserHandler* handler);
	
	/**
		Get the parser's handler.
		@returns The parser's handler.
	*/
	IniParserHandler* getHandler();
	
	/**
		Get the parser's current token.
		@returns The parser's current token.
	*/
	const IniToken& getToken() const;
	
	/**
		Get the parser's stream.
		@returns The parser's stream.
	*/
	Stream* getStream();
	
	/**
		Clean the parser.
		@returns Nothing.
	*/
	void clean();
	
	/**
		Parse the next token.
		@returns true if more data is left to be handled, or false if there is no more data to parse.
	*/
	bool parse();
	
	/**
		Get the next character in the stream.
		@returns The next character.
	*/
	UChar32 nextChar();
	
	/**
		Skip whitespace characters.
		@returns Nothing.
	*/
	void skipWhitespace();
	/**
		Skip to EOL character.
		@returns Nothing.
	*/
	void skipToEOL();
	
	/**
		Get the next token.
		@returns The next token.
	*/
	IniToken& nextToken();
	/**
		Read the current token.
		@returns Nothing.
	*/
	void readToken();
	
	/**
		Read a number token.
		@returns Nothing.
	*/
	void readNumberToken();
	/**
		Read a floating-point number token.
		@returns Nothing.
	*/
	void readDoubleToken();
	/**
		Read a string token.
		@returns Nothing.
	*/
	void readStringToken();
	/**
		Read a quoted-string token.
		@returns Nothing.
	*/
	void readQuotedStringToken();
	/**
		Read a node token.
		@returns Nothing.
	*/
	void readNodeToken();
	
protected:
	static CharacterSet _whitespaceset;
	static CharacterSet _numberset;
	static CharacterSet _digitset;
	
	Stream* _stream;
	IniParserHandler* _handler;
	int _line, _col;
	UChar32 _curchar;
	IniToken _token;
};

class DUCT_API IniParserHandler {
public:
	/**
		Destructor.
	*/
	virtual ~IniParserHandler();
	
	/**
		Initialize the handler.
		@returns Nothing.
	*/
	void init();
	
	/**
		Throw an exception.
		@returns Nothing.
	*/
	virtual void throwex(IniParserException e);
	
	/**
		Clean the handler.
		@returns Nothing.
	*/
	virtual void clean();
	
	/**
		Run the parser.
		@returns Nothing.
	*/
	virtual void process();
	
	/**
		Handle the given token.
		@returns Nothing.
		@param token The token to handle.
	*/
	virtual void handleToken(IniToken& token)=0;
	
	/**
		Finish processing.
		Close any handles, clean nodes, etc.
		@returns Nothing.
	*/
	virtual void finish()=0;
	
	/**
		Process the given stream.
		The user is responsible for closing the stream.
		Calls made by this function may throw a IniParserException.
		@returns The node processed from the given stream.
		@param stream The stream to process.
	*/
	Node* processFromStream(Stream* stream);
	
protected:
	IniParser _parser;
	Node* _rootnode;
	Node* _currentnode;
};

/**
	IniParser errors.
*/
enum IniParserError {
	/**
		Parser error.
	*/
	PARSERERROR_UNKNOWN=0,
	/**
		Parser error.
	*/
	PARSERERROR_PARSER,
	/**
		Hierarchy error (e.g. node opened but not closed).
	*/
	PARSERERROR_HIERARCHY,
	/**
		Memory allocation error (e.g. out of memory).
	*/
	PARSERERROR_MEMALLOC
};

/**
	A IniParser exception.
*/
class DUCT_API IniParserException : public std::exception {
public:
	/**
		Constructor with values.
	*/
	IniParserException(IniParserError error, const char* reporter, const IniToken* token, const IniParser* parser, const char* fmt, ...);
	
	/**
		Get the exception's message.
		@returns The exception's message.
	*/
	virtual const char* what() const throw();
	
	/**
		Convert an exception error to a NUL-terminated string.
		@returns The error as a string.
		@param error The error to convert.
	*/
	static const char* errorToString(IniParserError error);
	
protected:
	char _message[512];
	IniParserError _error;
	const char* _reporter;
	const IniToken* _token;
	const IniParser* _parser;
};

/**
	Standard IniParser handler.
*/
class DUCT_API StandardIniParserHandler : public IniParserHandler {
public:
	/**
		Constructor.
	*/
	StandardIniParserHandler();
	
	void throwex(IniParserException e);
	void clean();
	void handleToken(IniToken& token);
	void finish();
	
	/**
		Free any state data.
		NOTE: Only to be called when an exception is to be thrown, or when the handler does not return a valid node.
		@returns Nothing.
	*/
	void freeData();
	
	/**
		Reset the hanlder's state.
		@returns Nothing.
	*/
	void reset();
	
	/**
		Add the given variable to the current node and reset the handler's state.
		@returns Nothing.
		@param value The value to add.
	*/
	void addValueAndReset(ValueVariable* value);
	
protected:
	UnicodeString _varname;
	bool _equals;
};

/**
	INI formatter for duct's variable framework.
*/
class DUCT_API IniFormatter {
public:
	/**
		Format the given value as an ini value.
		@returns true if the value was formatted, or false if the given value did not have a name (ini values require a name).
		@param value The value to format.
		@param result The result string.
		@param nameformat The format for names.
		@param varformat The format for values.
	*/
	static bool formatValue(const ValueVariable& value, UnicodeString& result, unsigned int nameformat=FMT_NAME_DEFAULT, unsigned int varformat=FMT_ALL_DEFAULT);
	
	/**
		Load a node from the given file path.
		The user owns the returned node.
		Calls made by this function may throw IniParserExceptions.
		@returns The root node for the ini, or NULL if the file could not be opened/read.
		@param path The file path to read.
		@param encoding File character encoding. Default is UTF8.
	*/
	static Node* loadFromFile(const char* path, const char* encoding="utf8");
	static Node* loadFromFile(const std::string& path, const char* encoding="utf8");
	static Node* loadFromFile(const UnicodeString& path, const char* encoding="utf8");
	/**
		Load a node from the given stream.
		The user owns the returned node.
		Calls made by this function may throw IniParserExceptions.
		@returns The root node for the ini, or NULL if the given stream was NULL.
		@param stream The stream to read from.
	*/
	static Node* loadFromStream(Stream* stream);
	
	/**
		Write the given node to the given file path.
		NOTE: Identifiers are unexpected variables for this formatter. They will be ignored.
		@returns true if the node was written to the file, or false if either the given node was NULL or the given path could not be opened for writing.
		@param root The node to write.
		@param path The file path.
		@param encoding The character encoding to use. Default is UTF8.
		@param nameformat The format for names.
		@param varformat The format for values.
		@see writeToStream()
	*/
	static bool writeToFile(const Node* root, const char* path, const char* encoding="utf8", unsigned int nameformat=FMT_NAME_DEFAULT, unsigned int varformat=FMT_ALL_DEFAULT);
	static bool writeToFile(const Node* root, const std::string& path, const char* encoding="utf8", unsigned int nameformat=FMT_NAME_DEFAULT, unsigned int varformat=FMT_ALL_DEFAULT);
	static bool writeToFile(const Node* root, const UnicodeString& path, const char* encoding="utf8", unsigned int nameformat=FMT_NAME_DEFAULT, unsigned int varformat=FMT_ALL_DEFAULT);
	/**
		Write the given node to the given stream.
		NOTE: Identifiers are unexpected variables for this formatter. They will be ignored.
		@returns true if the node was written to the stream, or false if the given node or stream was NULL.
		@param root The node to write.
		@param stream The stream to write to.
		@param tcount The starting number of tabs to write before lines.
		@param nameformat The format for names.
		@param varformat The format for values.
		@see writeToFile()
	*/
	static bool writeToStream(const Node* root, Stream* stream, unsigned int tcount, unsigned int nameformat=FMT_NAME_DEFAULT, unsigned int varformat=FMT_ALL_DEFAULT);
	
protected:
	static StandardIniParserHandler _handler;
	
private:
	static void writeTabs(Stream* stream, unsigned int count, bool newline=false);
};

} // namespace duct

#endif // _DUCT_INIFORMATTER_HPP

