/**
@file IniFormatter.cpp
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
*/

#include <duct/debug.hpp>
#include <duct/IniFormatter.hpp>
#include <duct/CharUtils.hpp>

#include <stdio.h>
#include <unicode/numfmt.h>

namespace duct {

char const* ini_tokenName__(Token const& token) {
	switch (token.getType()) {
	case NULL_TOKEN:
		return "NULLToken";
	case StringToken:
		return "StringToken";
	case QuotedStringToken:
		return "QuotedStringToken";
	case NumberToken:
		return "NumberToken";
	case DoubleToken:
		return "DoubleToken";
	case EqualsToken:
		return "EqualsToken";
	case NodeToken:
		return "NodeToken";
	case CommentToken:
		return "CommentToken";
	case EOFToken:
		return "EOFToken";
	case EOLToken:
		return "EOLToken";
	}
	return "UNKNOWNToken";
}

// class IniParser implementation

CharacterSet IniParser::s_whitespaceset=CharacterSet("\t ");
CharacterSet IniParser::s_numberset=CharacterSet("0-9\\-+");
CharacterSet IniParser::s_digitset=CharacterSet(".0-9\\-+");

IniParser::IniParser()
	: m_handler(NULL)
{
	reset();
}

IniParser::IniParser(Stream* stream)
	: m_handler(NULL)
{
	initWithStream(stream);
}

IniParser::~IniParser() {
	reset();
}

void IniParser::setHandler(ParserHandler* handler) {
	m_handler=(IniParserHandler*)handler;
}

ParserHandler* IniParser::getHandler() {
	return m_handler;
}

bool IniParser::parse() {
	//nextChar();
	skipWhitespace();
	nextToken();
	readToken();
	if (m_curchar==CHAR_EOF) {
		m_token.reset(EOFToken);
		m_handler->handleToken(m_token); // Just to make sure the EOF gets handled (data might not end with a newline, causing an EOFToken)
		return false;
	} else if (m_token.getType()==EOFToken) {
		return false;
	}
	return true;
}

void IniParser::skipWhitespace() {
	while (m_curchar!=CHAR_EOF && s_whitespaceset.contains(m_curchar))
		nextChar();
}

Token& IniParser::nextToken() {
	m_token.reset(NULL_TOKEN);
	switch (m_curchar) {
	case CHAR_QUOTE:
		m_token.setType(QuotedStringToken);
		break;
	case CHAR_SEMICOLON:
		m_token.setType(CommentToken);
		break;
	case CHAR_EOF:
		m_token.setType(EOFToken);
		break;
	case CHAR_NEWLINE:
		m_token.setType(EOLToken);
		break;
	case CHAR_DECIMALPOINT:
		m_token.setType(DoubleToken);
		m_token.addChar(m_curchar); // Add the decimal
		break;
	case CHAR_EQUALSIGN:
		m_token.setType(EqualsToken);
		break;
	case CHAR_OPENBRACKET:
		m_token.setType(NodeToken);
		break;
	default:
		if (s_numberset.contains(m_curchar)) {
			m_token.setType(NumberToken);
		} else {
			m_token.setType(StringToken);
		}
		break;
	}
	m_token.setPosition(m_line, m_column);
	return m_token;
}

void IniParser::readToken() {
	//printf("(IniParser::readToken) token-type:%s line:%d, col:%d\n", ini_tokenName__(m_token), m_token.getLine(), m_token.getColumn());
	switch (m_token.getType()) {
	case QuotedStringToken:
		readQuotedStringToken();
		nextChar();
		break;
	case StringToken:
		readStringToken();
		break;
	case NumberToken:
		readNumberToken();
		break;
	case DoubleToken:
		nextChar();
		readDoubleToken();
		break;
	case EqualsToken:
		nextChar();
		break;
	case CommentToken:
		skipToEOL();
		//nextChar(); // Bad to get the next char, as it could be the EOL needed to terminate the current identifier
		break;
	case NodeToken:
		readNodeToken();
		nextChar();
		break;
	case EOLToken:
		nextChar();
		break;
	case EOFToken:
		// Do nothing
		break;
	default:
		throw IniParserException(PARSERERROR_PARSER, "IniParser::readToken", NULL, this, "Unhandled token: %s", ini_tokenName__(m_token));
		break;
	}
	m_handler->handleToken(m_token);
}

void IniParser::readNumberToken() {
	while (m_curchar!=CHAR_EOF) {
		if (m_curchar==CHAR_QUOTE) {
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readNumberToken", &m_token, this, "Unexpected quote");
		} else if (m_curchar==CHAR_NEWLINE || s_whitespaceset.contains(m_curchar) || m_curchar==CHAR_SEMICOLON /*|| m_curchar==CHAR_EQUALSIGN*/) {
			break;
		} else {
			if (s_numberset.contains(m_curchar)) {
				m_token.addChar(m_curchar);
			} else if (m_curchar==CHAR_DECIMALPOINT) {
				m_token.addChar(m_curchar);
				nextChar();
				m_token.setType(DoubleToken);
				readDoubleToken();
				return;
			} else {
				m_token.setType(StringToken);
				readStringToken();
				return;
			}
		}
		nextChar();
	}
}

void IniParser::readDoubleToken() {
	while (m_curchar!=CHAR_EOF) {
		if (m_curchar==CHAR_QUOTE) {
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readDoubleToken", &m_token, this, "Unexpected quote");
		} else if (m_curchar==CHAR_NEWLINE || s_whitespaceset.contains(m_curchar) || m_curchar==CHAR_SEMICOLON /*^^^|| m_curchar==CHAR_EQUALSIGN*/) {
			break;
		} else {
			if (s_numberset.contains(m_curchar)) {
				m_token.addChar(m_curchar);
			} else { // (m_curchar==CHAR_DECIMALPOINT)
				// The token should've already contained a decimal point, so it must be a string.
				m_token.setType(StringToken);
				readStringToken();
				return;
			}
		}
		nextChar();
	}
}

void IniParser::readStringToken() {
	while (m_curchar!=CHAR_EOF) {
		if (m_curchar==CHAR_QUOTE) {
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readStringToken", &m_token, this, "Unexpected quote");
		} else if (m_curchar==CHAR_NEWLINE /*^^^|| s_whitespaceset.contains(m_curchar)*/ || m_curchar==CHAR_SEMICOLON || m_curchar==CHAR_EQUALSIGN) {
			break;
		} else {
			m_token.addChar(m_curchar);
		}
		nextChar();
	}
}

void IniParser::readQuotedStringToken() {
	nextChar(); // Skip the first character (will be the initial quote)
	while (m_curchar!=CHAR_QUOTE) {
		switch (m_curchar) {
		case CHAR_EOF:
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readQuotedStringToken", &m_token, this, "Encountered EOF whilst reading quoted string");
			break;
		case CHAR_NEWLINE:
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readQuotedStringToken", &m_token, this, "Unexpected EOL (expected quote)");
		default:
			m_token.addChar(m_curchar);
			break;
		}
		nextChar();
	}
}

void IniParser::readNodeToken() {
	nextChar(); // Skip initial bracket
	while (m_curchar!=CHAR_EOF) {
		if (m_curchar==CHAR_OPENBRACKET) {
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readNodeToken", &m_token, this, "Unexpected open bracket");
		} else if (m_curchar==CHAR_SEMICOLON) {
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readNodeToken", &m_token, this, "Unexpected semicolon");
		} else if (m_curchar==CHAR_NEWLINE) {
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readNodeToken", &m_token, this, "Unexpected end of line");
		} else if (m_curchar==CHAR_CLOSEBRACKET /*|| ss_whitespaceset.contains(m_curchar)*/) {
			break;
		} else {
			m_token.addChar(m_curchar);
		}
		nextChar();
	}
}

// class IniParserException implementation

IniParserException::IniParserException(IniParserError error, char const* reporter, Token const* token, IniParser const* parser, char const* fmt, ...)
	: m_error(error), m_reporter(reporter), m_token(token), m_parser(parser)
{
	char temp[256];
	va_list args;
	va_start(args, fmt);
	vsprintf(temp, fmt, args);
	va_end(args);
	temp[255]='\0';
	if (m_parser && !m_token) {
		m_token=&m_parser->getToken();
	}
	if (m_token && m_parser) {
		sprintf(m_message, "(%s) [%s] from line: %d, col: %d to line: %d, col: %d: %s", m_reporter, errorToString(m_error), m_token->getLine(), m_token->getColumn(), m_parser->getLine(), m_parser->getColumn(), temp);
	} else if (m_token) {
		sprintf(m_message, "(%s) [%s] at line: %d, col: %d: %s", m_reporter, errorToString(m_error), m_token->getLine(), m_token->getColumn(), temp);
	} else if (m_parser) {
		sprintf(m_message, "(%s) [%s] at line: %d, col: %d: %s", m_reporter, errorToString(m_error), m_parser->getLine(), m_parser->getColumn(), temp);
	} else {
		sprintf(m_message, "(%s) [%s]: %s", m_reporter, errorToString(m_error), temp);
	}
	m_message[511]='\0';
}

char const* IniParserException::what() const throw() {
	return m_message;
}

char const* IniParserException::errorToString(IniParserError error) {
	switch (error) {
	case PARSERERROR_PARSER:
		return "ERROR_PARSER";
	case PARSERERROR_HIERARCHY:
		return "ERROR_HIERARCHY";
	case PARSERERROR_MEMALLOC:
		return "ERROR_MEMALLOC";
	default:
		return "ERROR_UNKNOWN";
	}
}

// class IniParserHandler implementation

IniParserHandler::IniParserHandler(IniParser& parser)
	: m_parser(parser), m_equals(false), m_rootnode(NULL), m_currentnode(NULL)
{
	m_parser.setHandler(this);
}

void IniParserHandler::setParser(Parser& parser) {
	m_parser=(IniParser&)parser;
	m_parser.setHandler(this);
}

Parser& IniParserHandler::getParser() {
	return m_parser;
}

void IniParserHandler::throwex(IniParserException e) {
	freeData();
	throw e;
}

void IniParserHandler::clean() {
	m_currentnode=NULL;
	m_rootnode=NULL;
	m_varname.remove();
	m_equals=false;
}

bool IniParserHandler::process() {
	m_rootnode=new Node(NULL);
	m_currentnode=m_rootnode;
	while (m_parser.parse()) {
	}
	finish();
	return true;
}

Node* IniParserHandler::processFromStream(Stream* stream) {
	m_parser.initWithStream(stream);
	process();
	Node* node=m_rootnode; // Store before cleaning
	clean();
	m_parser.reset();
	return node;
}

void IniParserHandler::freeData() {
	if (m_currentnode) {
		if (m_rootnode==m_currentnode || m_currentnode->getParent()!=m_rootnode) { // delete the root if the root and the current node are the same or if the current node has been parented
			delete m_rootnode;
		} else if (m_currentnode->getParent()==NULL) { // delete the root and the current node if the current node has not been parented
			delete m_rootnode;
			delete m_currentnode;
		}
	} else if (m_rootnode) {
		delete m_rootnode;
	}
	clean();
}

void IniParserHandler::handleToken(Token& token) {
	switch (token.getType()) {
	case StringToken:
	case QuotedStringToken: {
		if (m_varname.length()>0 && m_equals) {
			if (token.getType()==StringToken) {
				int bv=Variable::stringToBool(token.toString());
				if (bv!=-1) {
					addValueAndReset(new BoolVariable((bv==1) ? true : false, m_varname));
					return;
				}
			}
			addValueAndReset(new StringVariable(token.toString(), m_varname));
		} else if (m_varname.length()>0) {
			throwex(IniParserException(PARSERERROR_PARSER, "IniParserHandler::handleToken", &token, &m_parser, "Expected equals sign, got string"));
		} else {
			m_varname.setTo(token.toString()).trim();
		}
		}
		break;
	case NumberToken:
		if (m_varname.length()>0 && m_equals) {
			addValueAndReset(new IntVariable(token.toInt(), m_varname));
		} else {
			throwex(IniParserException(PARSERERROR_PARSER, "IniParserHandler::handleToken", &token, &m_parser, "A number cannot be an identifier"));
		}
		break;
	case DoubleToken:
		if (m_varname.length()>0 && m_equals) {
			addValueAndReset(new FloatVariable(token.toFloat(), m_varname));
		} else {
			throwex(IniParserException(PARSERERROR_PARSER, "IniParserHandler::handleToken", &token, &m_parser, "A number cannot be an identifier"));
		}
		break;
	case EqualsToken:
		if (m_varname.length()==0) {
			throwex(IniParserException(PARSERERROR_PARSER, "IniParserHandler::handleToken", &token, &m_parser, "Expected string, got equality sign"));
		} else if (m_equals) {
			throwex(IniParserException(PARSERERROR_PARSER, "IniParserHandler::handleToken", &token, &m_parser, "Expected value, got equality sign"));
		} else {
			m_equals=true;
		}
		break;
	case NodeToken: {
		if (m_varname.length()==0) {
			m_varname.setTo(token.toString()).trim();
			m_currentnode=new Node(m_varname, m_rootnode); // Trim whitespace
			m_varname.remove(); // clear the string
			m_rootnode->add(m_currentnode);
		} else {
			throwex(IniParserException(PARSERERROR_PARSER, "IniParserHandler::handleToken", &token, &m_parser, "NodeToken: Unknown error. m_varname length is>0"));
		}
		}
		break;
	case CommentToken:
		// Do nothing
		break;
	case EOLToken:
	case EOFToken:
		finish();
		break;
	default:
		//printf("IniParserHandler::handleToken Unhandled token of type: %s\n", ini_tokenName__(token));
		break;
	}
}

void IniParserHandler::finish() {
	if (m_varname.length()>0 && m_equals) {
		addValueAndReset(new StringVariable("", m_varname));
	} else if (m_varname.length()>0) {
		throwex(IniParserException(PARSERERROR_PARSER, "IniParserHandler::finish", NULL, &m_parser, "Expected equality sign, got EOL or EOF"));
	}
}

void IniParserHandler::reset() {
	m_varname.remove();
	m_equals=false;
}

void IniParserHandler::addValueAndReset(ValueVariable* value) {
	m_currentnode->add(value);
	reset();
}

// class IniFormatter implementation

IniParser IniFormatter::s_parser=IniParser();
IniParserHandler IniFormatter::s_handler=IniParserHandler(IniFormatter::s_parser);

bool IniFormatter::formatValue(ValueVariable const& value, icu::UnicodeString& result, unsigned int nameformat, unsigned int varformat) {
	if (value.getName().length()>0) {
		value.getNameFormatted(result, nameformat);
		icu::UnicodeString temp;
		value.getValueFormatted(temp, varformat);
		result.append('=').append(temp);
		return true;
	} else {
		result.remove(); // clear the result string
		debug_print("Value name is 0-length");
	}
	return false;
}

Node* IniFormatter::loadFromFile(char const* path, char const* encoding) {
	Stream* stream=FileStream::readFile(path, encoding);
	if (stream) {
		Node* root=s_handler.processFromStream(stream);
		stream->close();
		delete stream;
		return root;
	}
	return NULL;
}

Node* IniFormatter::loadFromFile(std::string const& path, char const* encoding) {
	return loadFromFile(path.c_str(), encoding);
}

Node* IniFormatter::loadFromFile(icu::UnicodeString const& path, char const* encoding) {
	std::string temp;
	path.toUTF8String(temp);
	return loadFromFile(temp.c_str(), encoding);
}

Node* IniFormatter::loadFromStream(Stream* stream) {
	if (stream) {
		return s_handler.processFromStream(stream);
	}
	return NULL;
}

bool IniFormatter::writeToFile(Node const* root, char const* path, char const* encoding, unsigned int nameformat, unsigned int varformat) {
	Stream* stream=FileStream::writeFile(path, encoding);
	if (stream) {
		writeToStream(root, stream, 0, nameformat, varformat);
		stream->close();
		return true;
	}
	return false;
}

bool IniFormatter::writeToFile(Node const* root, std::string const& path, char const* encoding, unsigned int nameformat, unsigned int varformat) {
	return writeToFile(root, path.c_str(), encoding, nameformat, varformat);
}

bool IniFormatter::writeToFile(Node const* root, icu::UnicodeString const& path, char const* encoding, unsigned int nameformat, unsigned int varformat) {
	std::string temp;
	path.toUTF8String(temp);
	return writeToFile(root, temp.c_str(), encoding, nameformat, varformat);
}

bool IniFormatter::writeToStream(Node const* root, Stream* stream, unsigned int tcount, unsigned int nameformat, unsigned int varformat) {
	if (root && stream) {
		icu::UnicodeString temp;
		if (root->getParent() && root->getName().length()>0) { // cheap way of saying the node is not a root node
			writeTabs(stream, tcount, false);
			root->getNameFormatted(temp, nameformat);
			stream->writeChar16('[');
			temp.append(']');
			stream->writeLine(temp);
		}
		Node* node;
		ValueVariable* value;
		for (VarList::const_iterator iter=root->begin(); iter!=root->end(); ++iter) {
			value=dynamic_cast<ValueVariable*>(*iter);
			node=dynamic_cast<Node*>(*iter);
			if (node) {
				writeToStream(node, stream, tcount, nameformat, varformat);
			} else if (value) {
				if (formatValue(*value, temp, nameformat, varformat)) {
					writeTabs(stream, tcount, false);
					stream->writeLine(temp);
				}
			}
		}
		return true;
	}
	return false;
}

void IniFormatter::writeTabs(Stream* stream, unsigned int count, bool newline) {
	while (0<count--) {
		stream->writeChar16('\t');
	}
	if (newline)
		stream->writeChar16('\n');
}

} // namespace duct
