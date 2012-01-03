/**
@file scriptformatter.hpp
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
	<li>ductScript</li>
</ul>
*/

#ifndef _DUCT_SCRIPTFORMATTER_HPP
#define _DUCT_SCRIPTFORMATTER_HPP

#include <duct/config.hpp>
#include <duct/parser.hpp>
#include <duct/filestream.hpp>
#include <duct/characterset.hpp>
#include <duct/variables.hpp>

#include <string>
#include <exception>

namespace duct {

/**
	Token types for the ScriptParser.
*/
enum ScriptTokenType {
	StringToken=1,
	QuotedStringToken,
	NumberToken,
	DoubleToken,
	
	EqualsToken,
	
	OpenBraceToken,
	CloseBraceToken,
	
	CommentToken,
	CommentBlockToken,
	EOFToken,
	EOLToken
};

// forward declarations
class ScriptParserHandler;
class ScriptParserException;

/**
	ductScript parser.
*/
class DUCT_API ScriptParser : public Parser {
public:
	friend class ScriptParserException;
	
	/**
		Constructor.
	*/
	ScriptParser();
	/**
		Constructor with stream.
		The user is responsible for closing the stream.
		@param stream The stream to read from.
	*/
	ScriptParser(Stream* stream);
	/**
		Destructor.
	*/
	~ScriptParser();
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
		Read a block comment token.
		@returns Nothing.
	*/
	void readCommentBlockToken();
	
protected:
	static CharacterSet s_whitespaceset;
	//static CharacterSet s_eolset;
	static CharacterSet s_numberset;
	static CharacterSet s_signset;
	
	ScriptParserHandler* m_handler;
};

/**
	ScriptParser errors.
*/
enum ScriptParserError {
	/**
		Unknown error.
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
	A ScriptParser exception.
*/
class DUCT_API ScriptParserException : public std::exception {
public:
	/**
		Constructor with values.
	*/
	ScriptParserException(ScriptParserError error, char const* reporter, Token const* token, ScriptParser const* parser, char const* fmt, ...);
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
	static char const* errorToString(ScriptParserError error);
	
protected:
	char m_message[512];
	ScriptParserError m_error;
	char const* m_reporter;
	Token const* m_token;
	ScriptParser const* m_parser;
};

/**
	Standard ScriptParser handler.
*/
class DUCT_API ScriptParserHandler : public ParserHandler {
public:
	/**
		Constructor.
	*/
	ScriptParserHandler(ScriptParser& parser);
	/**
		Destructor.
	*/
	~ScriptParserHandler();
	void setParser(Parser& parser);
	Parser& getParser();
	/**
		Throw an exception.
		@returns Nothing.
	*/
	void throwex(ScriptParserException e);
	/**
		Clean the handler.
		@returns Nothing.
	*/
	void clean();
	/**
		Run the parser.
		@returns Nothing.
	*/
	bool process();
	/**
		Handle the given token.
		@returns Nothing.
		@param token The token to handle.
	*/
	void handleToken(Token& token);
	/**
		Finish processing.
		Close any handles, clean nodes, etc.
		@returns Nothing.
	*/
	void finish();
	/**
		Process the given stream.
		The user is responsible for closing the stream.
		Calls made by this function may throw a ScriptParserException.
		@returns The node processed from the given stream.
		@param stream The stream to process.
	*/
	Node* processFromStream(Stream* stream);
	/**
		Free any state data.
		NOTE: Only call this when an exception is to be thrown, or when the handler does not return a valid node.
		@returns Nothing.
	*/
	void freeData();
	/**
		Reset the variable state.
		@returns Nothing.
		@param iden Whether to reset the identifier.
		@param value Whether to reset the value.
	*/
	void reset(bool iden, bool value);
	/**
		Add the given variable to the given collection and reset the variable state.
		@returns Nothing.
		@param collection The collection to add the variable to.
		@param variable The variable to add.
		@param iden Whether to reset the identifier.
		@param value Whether to reset the value.
	*/
	void addVariableAndReset(CollectionVariable* collection, Variable* variable, bool iden, bool value);
	/**
		Construct an identifier from the current state/token.
		@returns Nothing.
		@param token The token to construct the identifier from.
		@param resetiden Whether to reset the current identifier.
		@param resetvalue Whether to reset the current value.
		@param force Whether to force creation (create even if the handler has a current identifier).
	*/
	void makeIdentifier(Token const* token=NULL, bool resetiden=false, bool resetvalue=false, bool force=false);
	
protected:
	ScriptParser& m_parser;
	icu::UnicodeString m_varname;
	bool m_equals;
	Identifier* m_currentiden;
	ValueVariable* m_currentvalue;
	Node* m_rootnode;
	Node* m_currentnode;
};

/**
	Quake-style script formatter for duct's variable framework.
*/
class DUCT_API ScriptFormatter {
public:
	/**
		Format the given identifier and all its children as a script identifier.
		@returns true if the identifier was formatted, or false if the given identifier did not have a name (script identifiers require a name).
		@param iden The identifier to format.
		@param result The result string.
		@param nameformat The format for names.
		@param varformat The format for values.
	*/
	static bool formatIdentifier(Identifier const& iden, icu::UnicodeString& result, unsigned int nameformat=FMT_NAME_DEFAULT, unsigned int varformat=FMT_ALL_DEFAULT);
	/**
		Format the given value as a script value.
		@returns true if the value was formatted, or false if the given value did not have a name (script values require a name).
		@param value The value to format.
		@param result The result string.
		@param nameformat The format for names.
		@param varformat The format for values.
	*/
	static bool formatValue(ValueVariable const& value, icu::UnicodeString& result, unsigned int nameformat=FMT_NAME_DEFAULT, unsigned int varformat=FMT_ALL_DEFAULT);
	/**
		Load a node from the given file path.
		The user owns the returned node.
		Calls made by this function may throw ScriptParserExceptions.
		@returns The root node for the script, or NULL if the file could not be opened/read.
		@param path The file path to read.
		@param encoding File character encoding.
	*/
	static Node* loadFromFile(char const* path, char const* encoding="utf8");
	static Node* loadFromFile(std::string const& path, char const* encoding="utf8");
	static Node* loadFromFile(icu::UnicodeString const& path, char const* encoding="utf8");
	/**
		Load a node from the given stream.
		The user owns the returned node.
		Calls made by this function may throw ScriptParserExceptions.
		@returns The root node for the script, or NULL if the given stream was NULL.
		@param stream The stream to read from.
	*/
	static Node* loadFromStream(Stream* stream);
	/**
		Write the given node to the given file path.
		@returns true if the node was written to the file, or false if either the given node was NULL or the given path could not be opened for writing.
		@param root The node to write.
		@param path The file path.
		@param encoding The character encoding to use.
		@param nameformat The format for names.
		@param varformat The format for values.
	*/
	static bool writeToFile(Node const* root, char const* path, char const* encoding="utf8", unsigned int nameformat=FMT_NAME_DEFAULT, unsigned int varformat=FMT_ALL_DEFAULT);
	static bool writeToFile(Node const* root, std::string const& path, char const* encoding="utf8", unsigned int nameformat=FMT_NAME_DEFAULT, unsigned int varformat=FMT_ALL_DEFAULT);
	static bool writeToFile(Node const* root, icu::UnicodeString const& path, char const* encoding="utf8", unsigned int nameformat=FMT_NAME_DEFAULT, unsigned int varformat=FMT_ALL_DEFAULT);
	/**
		Write the given node to the given stream.
		@returns true if the node was written to the stream, or false if the given node or stream was NULL.
		@param root The node to write.
		@param stream The stream to write to.
		@param tcount The starting number of tabs to write before lines.
		@param nameformat The format for names.
		@param varformat The format for values.
	*/
	static bool writeToStream(Node const* root, Stream* stream, unsigned int tcount, unsigned int nameformat=FMT_NAME_DEFAULT, unsigned int varformat=FMT_ALL_DEFAULT);
	
protected:
	static ScriptParser s_parser;
	static ScriptParserHandler s_handler;
	
private:
	static void writeTabs(Stream* stream, unsigned int count, bool newline=false);
};

} // namespace duct

#endif // _DUCT_SCRIPTFORMATTER_HPP

