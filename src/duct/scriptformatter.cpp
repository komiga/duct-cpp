/**
@file scriptformatter.cpp
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
#include <duct/scriptformatter.hpp>
#include <duct/charutils.hpp>

#include <stdio.h>
#include <unicode/numfmt.h>

namespace duct {

char const* script_tokenName__(Token const& token) {
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
	case OpenBraceToken:
		return "OpenBraceToken";
	case CloseBraceToken:
		return "CloseBraceToken";
	case CommentToken:
		return "CommentToken";
	case CommentBlockToken:
		return "CommentBlockToken";
	case EOFToken:
		return "EOFToken";
	case EOLToken:
		return "EOLToken";
	}
	return "UNKNOWNToken";
}

// class ScriptParser implementation

CharacterSet ScriptParser::s_whitespaceset=CharacterSet("\t ");
//CharacterSet ScriptParser::s_eolset=CharacterSet('\n');
CharacterSet ScriptParser::s_numberset=CharacterSet("0-9\\-+");
CharacterSet ScriptParser::s_signset=CharacterSet("\\-+");

ScriptParser::ScriptParser()
	: m_handler(NULL)
{
	reset();
}

ScriptParser::ScriptParser(Stream* stream)
	: m_handler(NULL)
{
	initWithStream(stream);
}

ScriptParser::~ScriptParser() {
	reset();
}

void ScriptParser::setHandler(ParserHandler* handler) {
	m_handler=(ScriptParserHandler*)handler;
}

ParserHandler* ScriptParser::getHandler() {
	return m_handler;
}

bool ScriptParser::parse() {
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

void ScriptParser::skipWhitespace() {
	while (m_curchar!=CHAR_EOF && s_whitespaceset.contains(m_curchar))
		nextChar();
}

Token& ScriptParser::nextToken() {
	m_token.reset(NULL_TOKEN);
	m_token.setPosition(m_line, m_column);
	switch (m_curchar) {
	case CHAR_QUOTE:
		m_token.setType(QuotedStringToken);
		break;
	case CHAR_ASTERISK:
		if (peekChar()==CHAR_SLASH)
			throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::nextToken", &m_token, this, "Encountered unexpected end of block comment");
		m_token.setType(StringToken);
		break;
	case CHAR_SLASH:
		if (peekChar()==CHAR_SLASH)
			m_token.setType(CommentToken);
		else if (m_peekchar==CHAR_ASTERISK)
			m_token.setType(CommentBlockToken);
		else
			m_token.setType(StringToken);
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
	case CHAR_OPENBRACE:
		m_token.setType(OpenBraceToken);
		break;
	case CHAR_CLOSEBRACE:
		m_token.setType(CloseBraceToken);
		break;
	default:
		m_token.setType((s_numberset.contains(m_curchar)) ? NumberToken : StringToken);
		break;
	}
	return m_token;
}

void ScriptParser::readToken() {
	//printf("(ScriptParser::readToken) token-type:%s line:%d, col:%d\n", script_tokenName__(m_token), m_token.getLine(), m_token.getColumn());
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
	case CommentBlockToken:
		readCommentBlockToken();
		break;
	case OpenBraceToken:
	case CloseBraceToken:
		nextChar();
		break;
	case EOLToken:
		nextChar();
		break;
	case EOFToken:
		// Do nothing
		break;
	default:
		throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::readToken", NULL, this, "Unhandled token: %s", script_tokenName__(m_token));
		break;
	}
	// Special resolve when Number and Double tokens only contain signs or periods
	switch (m_token.getType()) {
	case NumberToken:
		if (m_token.compare(s_signset))
			m_token.setType(StringToken);
		break;
	case DoubleToken:
		if (m_token.compare(s_signset) || m_token.compare(CHAR_DECIMALPOINT))
			m_token.setType(StringToken);
		break;
	default:
		break;
	}
	m_handler->handleToken(m_token);
}

void ScriptParser::readNumberToken() {
	while (m_curchar!=CHAR_EOF) {
		if (m_curchar==CHAR_QUOTE) {
			throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::readNumberToken", &m_token, this, "Unexpected quote");
		} else if (m_curchar==CHAR_SLASH) {
			if (peekChar()==CHAR_SLASH || m_peekchar==CHAR_ASTERISK) {
				break;
			} else {
				m_token.setType(StringToken);
				readStringToken();
				return;
			}
		} else if (m_curchar==CHAR_NEWLINE || s_whitespaceset.contains(m_curchar)
				|| m_curchar==CHAR_OPENBRACE || m_curchar==CHAR_CLOSEBRACE
				|| m_curchar==CHAR_EQUALSIGN) {
			break;
		} else if (s_numberset.contains(m_curchar)) {
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
		nextChar();
	}
}

void ScriptParser::readDoubleToken() {
	while (m_curchar!=CHAR_EOF) {
		if (m_curchar==CHAR_QUOTE) {
			throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::readDoubleToken", &m_token, this, "Unexpected quote");
		} else if (m_curchar==CHAR_SLASH) {
			if (peekChar()==CHAR_SLASH || m_peekchar==CHAR_ASTERISK) {
				break;
			} else {
				m_token.setType(StringToken);
				readStringToken();
				return;
			}
		} else if (m_curchar==CHAR_NEWLINE || s_whitespaceset.contains(m_curchar)
				|| m_curchar==CHAR_OPENBRACE || m_curchar==CHAR_CLOSEBRACE
				|| m_curchar==CHAR_EQUALSIGN) {
			break;
		} else if (s_numberset.contains(m_curchar)) {
			m_token.addChar(m_curchar);
		} else { // (m_curchar==CHAR_DECIMALPOINT)
			// The token should've already contained a decimal point, so it must be a string.
			m_token.setType(StringToken);
			readStringToken();
			return;
		}
		nextChar();
	}
}

void ScriptParser::readStringToken() {
	while (m_curchar!=CHAR_EOF) {
		if (m_curchar==CHAR_QUOTE) {
			throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::readStringToken", &m_token, this, "Unexpected quote");
		} else if (m_curchar==CHAR_BACKSLASH) {
			UChar32 c=CharUtils::getEscapeChar(nextChar());
			if (c!=CHAR_EOF) {
				m_token.addChar(c);
			} else {
				throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::readStringToken", &m_token, this, "Unexpected escape sequence: %c", m_curchar);
			}
		} else if (m_curchar==CHAR_NEWLINE || s_whitespaceset.contains(m_curchar)
				|| (m_curchar==CHAR_SLASH && (peekChar()==CHAR_SLASH || m_peekchar==CHAR_ASTERISK))
				|| m_curchar==CHAR_OPENBRACE || m_curchar==CHAR_CLOSEBRACE || m_curchar==CHAR_EQUALSIGN) {
			break;
		} else {
			m_token.addChar(m_curchar);
		}
		nextChar();
	}
}

void ScriptParser::readQuotedStringToken() {
	bool eolreached=false;
	nextChar(); // Skip the first character (it will be the initial quote)
	while (m_curchar!=CHAR_QUOTE) {
		if (m_curchar==CHAR_EOF) {
			throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::readQuotedStringToken", &m_token, this, "Encountered EOF whilst reading quoted string");
		} else if (m_curchar==CHAR_BACKSLASH) {
			UChar32 c=CharUtils::getEscapeChar(nextChar());
			if (c!=CHAR_EOF) {
				m_token.addChar(c);
			} else {
				throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::readStringToken", &m_token, this, "Unexpected escape sequence: %c", m_curchar);
			}
		} else {
			if (!eolreached)
				m_token.addChar(m_curchar);
			if (m_curchar==CHAR_NEWLINE) {
				//throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::readQuotedStringToken", &m_token, this, "Unclosed quote (met EOL character)");
				eolreached=true;
			} else if (eolreached && !s_whitespaceset.contains(m_curchar)) {
				eolreached=false;
				m_token.addChar(m_curchar);
			}
		}
		nextChar();
	}
}

void ScriptParser::readCommentBlockToken() {
	nextChar(); // Skip the first character (it will be an asterisk)
	if (m_curchar!=CHAR_EOF) {
		while (skipToChar(CHAR_ASTERISK)) {
			if (nextChar()==CHAR_SLASH) {
				nextChar(); // Get the next character, otherwise the nextToken() call will try to handle the slash
				return;
			}
		}
	}
	throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::readCommentBlock", &m_token, this, "Unexpected EOF");
}

// class ScriptParserException implementation

ScriptParserException::ScriptParserException(ScriptParserError error, char const* reporter, Token const* token, ScriptParser const* parser, char const* fmt, ...)
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

char const* ScriptParserException::what() const throw() {
	return m_message;
}

char const* ScriptParserException::errorToString(ScriptParserError error) {
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

// class ScriptParserHandler implementation

ScriptParserHandler::ScriptParserHandler(ScriptParser& parser)
	: m_parser(parser), m_equals(false), m_currentiden(NULL), m_currentvalue(NULL), m_rootnode(NULL), m_currentnode(NULL)
{
	m_parser.setHandler(this);
}

ScriptParserHandler::~ScriptParserHandler() {
}

void ScriptParserHandler::setParser(Parser& parser) {
	m_parser=(ScriptParser&)parser;
	m_parser.setHandler(this);
}

Parser& ScriptParserHandler::getParser() {
	return m_parser;
}

void ScriptParserHandler::throwex(ScriptParserException e) {
	freeData();
	throw e;
}

void ScriptParserHandler::clean() {
	m_varname.remove();
	m_equals=false;
	m_currentvalue=NULL;
	m_currentiden=NULL;
}

bool ScriptParserHandler::process() {
	m_rootnode=new Node(NULL);
	m_currentnode=m_rootnode;
	while (m_parser.parse()) {
	}
	finish();
	if (m_currentnode!=m_rootnode) {
		//throwex(ScriptParserException(PARSERERROR_HIERARCHY, "ScriptParserHandler::process", NULL, NULL, "The current node does not match the root node"));
		return false;
	}
	return true;
}

void ScriptParserHandler::handleToken(Token& token) {
	switch (token.getType()) {
	case StringToken: {
		if (m_varname.length()>0 && m_equals) {
			int bv=Variable::stringToBool(token.toString());
			if (bv!=-1) {
				addVariableAndReset(m_currentnode, new BoolVariable(bv!=0, m_varname), false, false);
				return;
			} else {
				addVariableAndReset(m_currentnode, new StringVariable(token.toString(), m_varname), false, false);
			}
		} else if ((m_varname.length()>0 || m_currentiden) && !m_equals) {
			makeIdentifier(&token);
			int bv=Variable::stringToBool(token.toString());
			if (bv!=-1) {
				addVariableAndReset(m_currentiden, new BoolVariable(bv!=0), false, false);
				return;
			}
			addVariableAndReset(m_currentiden, new StringVariable(token.toString()), false, false);
		} else {// if (!m_varname.length()>0 && !m_equals) {
			m_varname=token.toString();
		}
		}
		break;
	case QuotedStringToken:
		if (m_varname.length()>0 && m_equals) {
			addVariableAndReset(m_currentnode, new StringVariable(token.toString(), m_varname), false, false);
		} else if ((m_varname.length()>0 || m_currentiden) && !m_equals) {
			makeIdentifier(&token);
			addVariableAndReset(m_currentiden, new StringVariable(token.toString()), false, false);
		} else {// if (!m_varname.length()>0 && !m_equals) {
			m_varname=token.toString();
		}
		break;
	case NumberToken:
		if (m_varname.length()>0 && m_equals) {
			m_currentvalue=new IntVariable(token.toInt(), m_varname);
			addVariableAndReset(m_currentnode, m_currentvalue, false, false);
		} else if ((m_varname.length()>0 || m_currentiden) && !m_equals) {
			makeIdentifier(&token);
			addVariableAndReset(m_currentiden, new IntVariable(token.toInt()), false, false);
		} else {
			//throwex(ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::handleToken", &token, &m_parser, "A number cannot be an identifier"));
			m_varname.setTo(token.toString());
		}
		break;
	case DoubleToken:
		if (m_varname.length()>0 && m_equals) {
			m_currentvalue=new FloatVariable((float)token.toDouble(), m_varname);
			addVariableAndReset(m_currentnode, m_currentvalue, false, false);
		} else if ((m_varname.length()>0 || m_currentiden) && !m_equals) {
			makeIdentifier(&token);
			addVariableAndReset(m_currentiden, new FloatVariable((float)token.toDouble()), false, false);
		} else {
			//throwex(ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::handleToken", &token, &m_parser, "A number cannot be an identifier"));
			m_varname.setTo(token.toString());
		}
		break;
	case EqualsToken:
		if (m_currentiden) {
			throwex(ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::handleToken", &token, &m_parser, "Unexpected equality sign after identifier declaration"));
		} else if (m_varname.length()==0) {
			throwex(ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::handleToken", &token, &m_parser, "Expected string, got equality sign"));
		} else if (m_equals) {
			throwex(ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::handleToken", &token, &m_parser, "Expected value, got equality sign"));
		} else {
			m_equals=true;
		}
		break;
	case OpenBraceToken: {
		if (m_currentiden) {
			throwex(ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::handleToken", &token, &m_parser, "Node cannot contain values (possible openbrace typo)"));
		}
		Node* tempnode=new Node(m_varname, m_currentnode);
		addVariableAndReset(m_currentnode, tempnode, false, false);
		m_currentnode=tempnode;
		}
		break;
	case CloseBraceToken:
		if (!m_currentnode->getParent()) {
			throwex(ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::handleToken", &token, &m_parser, "Mismatched node brace"));
		} else if (m_equals) {
			throwex(ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::handleToken", &token, &m_parser, "Expected value, got close-brace"));
		} else {
			if (m_currentiden) {
				reset(true, true);
			} else if (m_varname.length()>0) { // brace acts terminatively on a yet-realized identifier
				makeIdentifier(NULL, true, true, true);
			}
			m_currentnode=(Node*)m_currentnode->getParent();
		}
		break;
	case CommentToken:
	case CommentBlockToken:
		// Do nothing
		break;
	case EOLToken:
	case EOFToken:
		finish();
		break;
	default:
		//printf("(ScriptParserHandler::handleToken) Unhandled token of type "+script_tokenName__(token))
		break;
	}
}

void ScriptParserHandler::finish() {
	if (m_parser.getToken().getType()==EOLToken && m_equals) {
		throwex(ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::finish", &m_parser.getToken(), &m_parser, "Expected value, got EOL/EOF"));
	} else if (m_varname.length()>0) { // no-value identifier
		makeIdentifier(NULL, true, true, true);
	} else {
		reset(true, true);
	}
}

Node* ScriptParserHandler::processFromStream(Stream* stream) {
	m_parser.initWithStream(stream);
	process();
	Node* node=m_rootnode; // Store before cleaning
	clean();
	m_parser.reset();
	return node;
}

void ScriptParserHandler::freeData() {
	if (m_currentvalue) {
		delete m_currentvalue;
	}
	if (m_currentiden) {
		delete m_currentiden;
	}
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

void ScriptParserHandler::reset(bool iden, bool value) {
	m_varname.remove();
	m_equals=false;
	if (value) {
		m_currentvalue=NULL;
	}
	if (iden) {
		m_currentiden=NULL;
	}
}

void ScriptParserHandler::addVariableAndReset(CollectionVariable* collection, Variable* variable, bool iden, bool value) {
	collection->add(variable);
	reset(iden, value);
}

void ScriptParserHandler::makeIdentifier(Token const* /*token*/, bool resetiden, bool resetvalue, bool force) {
	//if (m_currentvalue) {
	//	throw ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::makeIdentifier", token, NULL, "Value already defined on line");
	if (!m_currentiden || force) {
		/*int bv=Variable::stringToBool(m_varname);
		if (bv!=-1) {
			throwex(ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::makeIdentifier", token, m_parser, "Identifier name cannot be boolean value"));
		}*/
		m_currentiden=new Identifier(m_varname);
		addVariableAndReset(m_currentnode, m_currentiden, resetiden, resetvalue);
	//} else {
		//throw ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::makeIdentifier", token, NULL, "Unknown error")
	}
}

// class ScriptFormatter implementation

ScriptParser ScriptFormatter::s_parser=ScriptParser();
ScriptParserHandler ScriptFormatter::s_handler=ScriptParserHandler(ScriptFormatter::s_parser);

bool ScriptFormatter::formatIdentifier(Identifier const& iden, icu::UnicodeString& result, unsigned int nameformat, unsigned int varformat) {
	if (iden.getName().length()>0) {
		icu::UnicodeString temp;
		iden.getNameFormatted(temp, nameformat);
		result.setTo(temp);
		ValueVariable* val;
		for (VarList::const_iterator iter=iden.begin(); iter!=iden.end(); ++iter) {
			val=dynamic_cast<ValueVariable*>(*iter);
			if (val) {
				/*temp=icu::UnicodeString(val->getTypeAsString());
				result+=" <"+temp+">";
				val->getValueFormatted(temp, varformat);
				result+=temp;*/
				val->getValueFormatted(temp, varformat);
				result.append(' ').append(temp);
			}
		}
		return true;
	} else {
		result.remove(); // clear the result string
		debug_print("Identifier name is 0-length");
	}
	return false;
}

bool ScriptFormatter::formatValue(ValueVariable const& value, icu::UnicodeString& result, unsigned int nameformat, unsigned int varformat) {
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

Node* ScriptFormatter::loadFromFile(char const* path, char const* encoding) {
	Stream* stream=FileStream::readFile(path, encoding);
	if (stream) {
		Node* root=s_handler.processFromStream(stream);
		stream->close();
		delete stream;
		return root;
	}
	return NULL;
}

Node* ScriptFormatter::loadFromFile(std::string const& path, char const* encoding) {
	return loadFromFile(path.c_str(), encoding);
}

Node* ScriptFormatter::loadFromFile(icu::UnicodeString const& path, char const* encoding) {
	std::string temp;
	path.toUTF8String(temp);
	return loadFromFile(temp.c_str(), encoding);
}

Node* ScriptFormatter::loadFromStream(Stream* stream) {
	if (stream) {
		return s_handler.processFromStream(stream);
	}
	return NULL;
}

bool ScriptFormatter::writeToFile(Node const* root, char const* path, char const* encoding, unsigned int nameformat, unsigned int varformat) {
	Stream* stream=FileStream::writeFile(path, encoding);
	if (stream) {
		writeToStream(root, stream, 0, nameformat, varformat);
		stream->close();
		return true;
	}
	return false;
}

bool ScriptFormatter::writeToFile(Node const* root, std::string const& path, char const* encoding, unsigned int nameformat, unsigned int varformat) {
	return writeToFile(root, path.c_str(), encoding, nameformat, varformat);
}

bool ScriptFormatter::writeToFile(Node const* root, icu::UnicodeString const& path, char const* encoding, unsigned int nameformat, unsigned int varformat) {
	std::string temp;
	path.toUTF8String(temp);
	return writeToFile(root, temp.c_str(), encoding, nameformat, varformat);
}

bool ScriptFormatter::writeToStream(Node const* root, Stream* stream, unsigned int tcount, unsigned int nameformat, unsigned int varformat) {
	if (root && stream) {
		icu::UnicodeString temp;
		unsigned int tcountd=tcount;
		if (root->getParent()) {
			writeTabs(stream, tcount);
			if (root->getName().length()>0) {
				root->getNameFormatted(temp, nameformat);
				temp.append(" {");
				stream->writeLine(temp);
			} else {
				temp.setTo("{");
				stream->writeLine(temp);
			}
			tcountd+=1;
		} else {
			//stream->writeChar16('\n');
		}
		Node* node;
		Identifier* iden;
		ValueVariable* value;
		bool writtenvariable=false;
		for (VarList::const_iterator iter=root->begin(); iter!=root->end(); ++iter) {
			value=dynamic_cast<ValueVariable*>(*iter);
			iden=dynamic_cast<Identifier*>(*iter);
			node=dynamic_cast<Node*>(*iter);
			if (!node && (*iter)->getName().length()==0) {
				debug_print("(ScriptFormatter::writeToStream) Ignored ValueVariable or Identifier because of 0-length name");
			} else {
				if (value) {
					if (formatValue(*value, temp, nameformat, varformat)) {
						writeTabs(stream, tcountd);
						stream->writeLine(temp);
						writtenvariable=true;
					}
				} else if (iden) {
					if (formatIdentifier(*iden, temp, nameformat, varformat)) {
						writeTabs(stream, tcountd);
						stream->writeLine(temp);
						writtenvariable=true;
					}
				} else if (node) {
					if (!root->getParent() && writtenvariable)
						writeTabs(stream, tcountd, true);
					writeToStream(node, stream, tcountd, nameformat, varformat);
					if (!root->getParent())
						writeTabs(stream, tcountd, true);
					writtenvariable=false;
				}
			}
		}
		if (root->getParent()) {
			writeTabs(stream, tcount);
			temp.setTo('}');
			stream->writeLine(temp);
		}
		return true;
	}
	return false;
}

void ScriptFormatter::writeTabs(Stream* stream, unsigned int count, bool newline) {
	while (0<count--) {
		stream->writeChar16('\t');
	}
	if (newline)
		stream->writeChar16('\n');
}

} // namespace duct

