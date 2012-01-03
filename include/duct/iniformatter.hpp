/**
@file iniformatter.hpp
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

Implements component parts:
<ul>
	<li>ductIni</li>
</ul>
*/

#ifndef _DUCT_INIFORMATTER_HPP
#define _DUCT_INIFORMATTER_HPP

#include <duct/config.hpp>
#include <duct/parser.hpp>
#include <duct/filestream.hpp>
#include <duct/characterset.hpp>
#include <duct/variables.hpp>

#include <string>
#include <exception>

namespace duct {

/**
	ductIni Token types.
*/
enum TokenType {
	StringToken=1,
	QuotedStringToken,
	NumberToken,
	DoubleToken,
	
	EqualsToken,
	NodeToken,
	
	CommentToken,
	EOFToken,
	EOLToken
};

class IniParserHandler; // Forward declaration
class IniParserException; // Forward declaration

/**
	ductIni parser.
*/
class DUCT_API IniParser : public Parser {
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
	void setHandler(ParserHandler* handler);
	ParserHandler* getHandler();
	/**
		Skip whitespace characters.
		@returns Nothing.
	*/
	void skipWhitespace();
	Token& nextToken();
	void readToken();
	bool parse();
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
	static CharacterSet s_whitespaceset;
	static CharacterSet s_numberset;
	static CharacterSet s_digitset;
	
	IniParserHandler* m_handler;
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
	IniParser exception.
*/
class DUCT_API IniParserException : public std::exception {
public:
	/**
		Constructor with values.
	*/
	IniParserException(IniParserError error, char const* reporter, Token const* token, IniParser const* parser, char const* fmt, ...);
	/**
		Get the exception's message.
		@returns The exception's message.
	*/
	virtual char const* what() const throw();
	/**
		Convert an exception error to a NUL-terminated string.
		@returns The error as a string.
		@param error The error to convert.
	*/
	static char const* errorToString(IniParserError error);
	
protected:
	char m_message[512];
	IniParserError m_error;
	char const* m_reporter;
	Token const* m_token;
	IniParser const* m_parser;
};

/**
	IniParser handler.
*/
class DUCT_API IniParserHandler : public ParserHandler {
public:
	/**
		Constructor.
	*/
	IniParserHandler(IniParser& parser);
	void setParser(Parser& parser);
	Parser& getParser();
	/**
		Throw an exception.
		@returns Nothing.
	*/
	void throwex(IniParserException e);
	void clean();
	bool process();
	/**
		Process the given stream.
		The user is responsible for closing the stream.
		Calls made by this function may throw a IniParserException.
		@returns The node processed from the given stream.
		@param stream The stream to process.
	*/
	Node* processFromStream(Stream* stream);
	void handleToken(Token& token);
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
	IniParser& m_parser;
	icu::UnicodeString m_varname;
	bool m_equals;
	Node* m_rootnode;
	Node* m_currentnode;
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
	static bool formatValue(ValueVariable const& value, icu::UnicodeString& result, unsigned int nameformat=FMT_NAME_DEFAULT, unsigned int varformat=FMT_ALL_DEFAULT);
	/**
		Load a node from the given file path.
		The user owns the returned node.
		Calls made by this function may throw IniParserExceptions.
		@returns The root node for the ini, or NULL if the file could not be opened/read.
		@param path The file path to read.
		@param encoding File character encoding. Default is UTF8.
	*/
	static Node* loadFromFile(char const* path, char const* encoding="utf8");
	static Node* loadFromFile(std::string const& path, char const* encoding="utf8");
	static Node* loadFromFile(icu::UnicodeString const& path, char const* encoding="utf8");
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
	static bool writeToFile(Node const* root, char const* path, char const* encoding="utf8", unsigned int nameformat=FMT_NAME_DEFAULT, unsigned int varformat=FMT_ALL_DEFAULT);
	static bool writeToFile(Node const* root, std::string const& path, char const* encoding="utf8", unsigned int nameformat=FMT_NAME_DEFAULT, unsigned int varformat=FMT_ALL_DEFAULT);
	static bool writeToFile(Node const* root, icu::UnicodeString const& path, char const* encoding="utf8", unsigned int nameformat=FMT_NAME_DEFAULT, unsigned int varformat=FMT_ALL_DEFAULT);
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
	static bool writeToStream(Node const* root, Stream* stream, unsigned int tcount, unsigned int nameformat=FMT_NAME_DEFAULT, unsigned int varformat=FMT_ALL_DEFAULT);
	
protected:
	static IniParser s_parser;
	static IniParserHandler s_handler;
	
private:
	static void writeTabs(Stream* stream, unsigned int count, bool newline=false);
};

} // namespace duct

#endif // _DUCT_INIFORMATTER_HPP

