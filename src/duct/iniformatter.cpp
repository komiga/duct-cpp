/**
@file iniformatter.cpp
@author Tim Howard

@section LICENSE
Copyright (c) 2010-2011 Tim Howard

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

#include <stdio.h>
#include <unicode/numfmt.h>
#include <duct/debug.hpp>
#include <duct/iniformatter.hpp>
#include <duct/charutils.hpp>

namespace duct {

const char* __ini_tokenName(const Token& token) {
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

CharacterSet IniParser::_whitespaceset=CharacterSet("\t ");
CharacterSet IniParser::_numberset=CharacterSet("0-9\\-+");
CharacterSet IniParser::_digitset=CharacterSet(".0-9\\-+");

IniParser::IniParser() : _handler(NULL) {
	reset();
}
IniParser::IniParser(Stream* stream) : _handler(NULL) {
	initWithStream(stream);
}

IniParser::~IniParser() {
	reset();
}

void IniParser::setHandler(ParserHandler* handler) {
	_handler=(IniParserHandler*)handler;
}

ParserHandler* IniParser::getHandler() {
	return _handler;
}

bool IniParser::parse() {
	//nextChar();
	skipWhitespace();
	nextToken();
	readToken();
	if (_curchar==CHAR_EOF) {
		_token.reset(EOFToken);
		_handler->handleToken(_token); // Just to make sure the EOF gets handled (data might not end with a newline, causing an EOFToken)
		return false;
	} else if (_token.getType()==EOFToken) {
		return false;
	}
	return true;
}

void IniParser::skipWhitespace() {
	while (_curchar!=CHAR_EOF && _whitespaceset.contains(_curchar))
		nextChar();
}

Token& IniParser::nextToken() {
	_token.reset(NULL_TOKEN);
	switch (_curchar) {
		case CHAR_QUOTE:
			_token.setType(QuotedStringToken);
			break;
		case CHAR_SEMICOLON:
			_token.setType(CommentToken);
			break;
		case CHAR_EOF:
			_token.setType(EOFToken);
			break;
		case CHAR_NEWLINE:
			_token.setType(EOLToken);
			break;
		case CHAR_DECIMALPOINT:
			_token.setType(DoubleToken);
			_token.addChar(_curchar); // Add the decimal
			break;
		case CHAR_EQUALSIGN:
			_token.setType(EqualsToken);
			break;
		case CHAR_OPENBRACKET:
			_token.setType(NodeToken);
			break;
		default:
			if (_numberset.contains(_curchar)) {
				_token.setType(NumberToken);
			} else {
				_token.setType(StringToken);
			}
			break;
	}
	_token.setPosition(_line, _column);
	return _token;
}

void IniParser::readToken() {
	//printf("(IniParser::readToken) token-type:%s line:%d, col:%d\n", __ini_tokenName(_token), _token.getLine(), _token.getColumn());
	switch (_token.getType()) {
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
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readToken", NULL, this, "Unhandled token: %s", __ini_tokenName(_token));
			break;
	}
	_handler->handleToken(_token);
}

void IniParser::readNumberToken() {
	while (_curchar!=CHAR_EOF) {
		if (_curchar==CHAR_QUOTE) {
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readNumberToken", &_token, this, "Unexpected quote");
		} else if (_curchar==CHAR_NEWLINE || _whitespaceset.contains(_curchar) || _curchar==CHAR_SEMICOLON /*|| _curchar==CHAR_EQUALSIGN*/) {
			break;
		} else {
			if (_numberset.contains(_curchar)) {
				_token.addChar(_curchar);
			} else if (_curchar==CHAR_DECIMALPOINT) {
				_token.addChar(_curchar);
				nextChar();
				_token.setType(DoubleToken);
				readDoubleToken();
				return;
			} else {
				_token.setType(StringToken);
				readStringToken();
				return;
			}
		}
		nextChar();
	}
}

void IniParser::readDoubleToken() {
	while (_curchar!=CHAR_EOF) {
		if (_curchar==CHAR_QUOTE) {
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readDoubleToken", &_token, this, "Unexpected quote");
		} else if (_curchar==CHAR_NEWLINE || _whitespaceset.contains(_curchar) || _curchar==CHAR_SEMICOLON /*^^^|| _curchar==CHAR_EQUALSIGN*/) {
			break;
		} else {
			if (_numberset.contains(_curchar)) {
				_token.addChar(_curchar);
			} else { // (_curchar==CHAR_DECIMALPOINT)
				// The token should've already contained a decimal point, so it must be a string.
				_token.setType(StringToken);
				readStringToken();
				return;
			}
		}
		nextChar();
	}
}

void IniParser::readStringToken() {
	while (_curchar!=CHAR_EOF) {
		if (_curchar==CHAR_QUOTE) {
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readStringToken", &_token, this, "Unexpected quote");
		} else if (_curchar==CHAR_NEWLINE /*^^^|| _whitespaceset.contains(_curchar)*/ || _curchar==CHAR_SEMICOLON || _curchar==CHAR_EQUALSIGN) {
			break;
		} else {
			_token.addChar(_curchar);
		}
		nextChar();
	}
}

void IniParser::readQuotedStringToken() {
	nextChar(); // Skip the first character (will be the initial quote)
	while (_curchar!=CHAR_QUOTE) {
		switch (_curchar) {
			case CHAR_EOF:
				throw IniParserException(PARSERERROR_PARSER, "IniParser::readQuotedStringToken", &_token, this, "Encountered EOF whilst reading quoted string");
				break;
			case CHAR_NEWLINE:
				throw IniParserException(PARSERERROR_PARSER, "IniParser::readQuotedStringToken", &_token, this, "Unexpected EOL (expected quote)");
			default:
				_token.addChar(_curchar);
				break;
		}
		nextChar();
	}
}

void IniParser::readNodeToken() {
	nextChar(); // Skip initial bracket
	while (_curchar!=CHAR_EOF) {
		if (_curchar==CHAR_OPENBRACKET) {
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readNodeToken", &_token, this, "Unexpected open bracket");
		} else if (_curchar==CHAR_SEMICOLON) {
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readNodeToken", &_token, this, "Unexpected semicolon");
		} else if (_curchar==CHAR_NEWLINE) {
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readNodeToken", &_token, this, "Unexpected end of line");
		} else if (_curchar==CHAR_CLOSEBRACKET /*|| _whitespaceset.contains(_curchar)*/) {
			break;
		} else {
			_token.addChar(_curchar);
		}
		nextChar();
	}
}

// class IniParserException implementation

IniParserException::IniParserException(IniParserError error, const char* reporter, const Token* token, const IniParser* parser, const char* fmt, ...) {
	_error=error;
	_reporter=reporter;
	_token=token;
	_parser=parser;
	char temp[256];
	va_list args;
	va_start(args, fmt);
	vsprintf(temp, fmt, args);
	va_end(args);
	temp[255]='\0';
	if (_parser && !_token) {
		_token=&_parser->getToken();
	}
	if (_token && _parser) {
		sprintf(_message, "(%s) [%s] from line: %d, col: %d to line: %d, col: %d: %s", _reporter, errorToString(_error), _token->getLine(), _token->getColumn(), _parser->getLine(), _parser->getColumn(), temp);
	} else if (_token) {
		sprintf(_message, "(%s) [%s] at line: %d, col: %d: %s", _reporter, errorToString(_error), _token->getLine(), _token->getColumn(), temp);
	} else if (_parser) {
		sprintf(_message, "(%s) [%s] at line: %d, col: %d: %s", _reporter, errorToString(_error), _parser->getLine(), _parser->getColumn(), temp);
	} else {
		sprintf(_message, "(%s) [%s]: %s", _reporter, errorToString(_error), temp);
	}
	_message[511]='\0';
}

const char* IniParserException::what() const throw() {
	return _message;
}

const char* IniParserException::errorToString(IniParserError error) {
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

IniParserHandler::IniParserHandler(IniParser& parser) : _parser(parser), _equals(false), _rootnode(NULL), _currentnode(NULL) {
	_parser.setHandler(this);
}

void IniParserHandler::setParser(Parser& parser) {
	_parser=(IniParser&)parser;
	_parser.setHandler(this);
}

Parser& IniParserHandler::getParser() {
	return _parser;
}

void IniParserHandler::throwex(IniParserException e) {
	freeData();
	throw e;
}

void IniParserHandler::clean() {
	_currentnode=NULL;
	_rootnode=NULL;
	_varname.remove();
	_equals=false;
}

bool IniParserHandler::process() {
	_rootnode=new Node(NULL);
	_currentnode=_rootnode;
	while (_parser.parse()) {
	}
	finish();
	return true;
}

Node* IniParserHandler::processFromStream(Stream* stream) {
	_parser.initWithStream(stream);
	process();
	Node* node=_rootnode; // Store before cleaning
	clean();
	_parser.reset();
	return node;
}

void IniParserHandler::freeData() {
	if (_currentnode) {
		if (_rootnode==_currentnode || _currentnode->getParent()!=_rootnode) { // delete the root if the root and the current node are the same or if the current node has been parented
			delete _rootnode;
		} else if (_currentnode->getParent()==NULL) { // delete the root and the current node if the current node has not been parented
			delete _rootnode;
			delete _currentnode;
		}
	} else if (_rootnode) {
		delete _rootnode;
	}
	clean();
}

void IniParserHandler::handleToken(Token& token) {
	switch (token.getType()) {
		case StringToken:
		case QuotedStringToken: {
			if (_varname.length()>0 && _equals) {
				if (token.getType()==StringToken) {
					int bv=Variable::stringToBool(token.toString());
					if (bv!=-1) {
						addValueAndReset(new BoolVariable((bool)bv, _varname));
						return;
					}
				}
				addValueAndReset(new StringVariable(token.toString(), _varname));
			} else if (_varname.length()>0) {
				throwex(IniParserException(PARSERERROR_PARSER, "IniParserHandler::handleToken", &token, &_parser, "Expected equals sign, got string"));
			} else {
				_varname.setTo(token.toString()).trim();
			}
			}
			break;
		case NumberToken:
			if (_varname.length()>0 && _equals) {
				addValueAndReset(new IntVariable(token.toInt(), _varname));
			} else {
				throwex(IniParserException(PARSERERROR_PARSER, "IniParserHandler::handleToken", &token, &_parser, "A number cannot be an identifier"));
			}
			break;
		case DoubleToken:
			if (_varname.length()>0 && _equals) {
				addValueAndReset(new FloatVariable(token.toDouble(), _varname));
			} else {
				throwex(IniParserException(PARSERERROR_PARSER, "IniParserHandler::handleToken", &token, &_parser, "A number cannot be an identifier"));
			}
			break;
		case EqualsToken:
			if (_varname.length()==0) {
				throwex(IniParserException(PARSERERROR_PARSER, "IniParserHandler::handleToken", &token, &_parser, "Expected string, got equality sign"));
			} else if (_equals) {
				throwex(IniParserException(PARSERERROR_PARSER, "IniParserHandler::handleToken", &token, &_parser, "Expected value, got equality sign"));
			} else {
				_equals=true;
			}
			break;
		case NodeToken: {
			if (_varname.length()==0) {
				_varname.setTo(token.toString()).trim();
				_currentnode=new Node(_varname, _rootnode); // Trim whitespace
				_varname.remove(); // clear the string
				_rootnode->add(_currentnode);
			} else {
				throwex(IniParserException(PARSERERROR_PARSER, "IniParserHandler::handleToken", &token, &_parser, "NodeToken: Unknown error. _varname length is>0"));
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
			//printf("IniParserHandler::handleToken Unhandled token of type: %s\n", __ini_tokenName(token));
			break;
	}
}

void IniParserHandler::finish() {
	if (_varname.length()>0 && _equals) {
		addValueAndReset(new StringVariable("", _varname));
	} else if (_varname.length()>0) {
		throwex(IniParserException(PARSERERROR_PARSER, "IniParserHandler::finish", NULL, &_parser, "Expected equality sign, got EOL or EOF"));
	}
}

void IniParserHandler::reset() {
	_varname.remove();
	_equals=false;
}

void IniParserHandler::addValueAndReset(ValueVariable* value) {
	_currentnode->add(value);
	reset();
}

// class IniFormatter implementation

IniParser IniFormatter::_parser=IniParser();
IniParserHandler IniFormatter::_handler=IniParserHandler(IniFormatter::_parser);

bool IniFormatter::formatValue(const ValueVariable& value, UnicodeString& result, unsigned int nameformat, unsigned int varformat) {
	if (value.getName().length()>0) {
		UnicodeString temp;
		value.getNameFormatted(temp, nameformat);
		result.setTo(temp);
		value.getValueFormatted(temp, varformat);
		result+="="+temp;
		return true;
	} else {
		result.remove(); // clear the result string
		debug_print("Value name is 0-length");
	}
	return false;
}

Node* IniFormatter::loadFromFile(const char* path, const char* encoding) {
	Stream* stream=FileStream::readFile(path, encoding);
	if (stream) {
		Node* root=_handler.processFromStream(stream);
		stream->close();
		delete stream;
		return root;
	}
	return NULL;
}

Node* IniFormatter::loadFromFile(const std::string& path, const char* encoding) {
	return loadFromFile(path.c_str(), encoding);
}

Node* IniFormatter::loadFromFile(const UnicodeString& path, const char* encoding) {
	std::string temp;
	path.toUTF8String(temp);
	return loadFromFile(temp.c_str(), encoding);
}

Node* IniFormatter::loadFromStream(Stream* stream) {
	if (stream) {
		return _handler.processFromStream(stream);
	}
	return NULL;
}

bool IniFormatter::writeToFile(const Node* root, const char* path, const char* encoding, unsigned int nameformat, unsigned int varformat) {
	Stream* stream=FileStream::writeFile(path, encoding);
	if (stream) {
		writeToStream(root, stream, 0, nameformat, varformat);
		stream->close();
		return true;
	}
	return false;
}

bool IniFormatter::writeToFile(const Node* root, const std::string& path, const char* encoding, unsigned int nameformat, unsigned int varformat) {
	return writeToFile(root, path.c_str(), encoding, nameformat, varformat);
}

bool IniFormatter::writeToFile(const Node* root, const UnicodeString& path, const char* encoding, unsigned int nameformat, unsigned int varformat) {
	std::string temp;
	path.toUTF8String(temp);
	return writeToFile(root, temp.c_str(), encoding, nameformat, varformat);
}

bool IniFormatter::writeToStream(const Node* root, Stream* stream, unsigned int tcount, unsigned int nameformat, unsigned int varformat) {
	if (root && stream) {
		UnicodeString temp;
		if (root->getParent() && root->getName().length()>0) { // cheap way of saying the node is not a root node
			writeTabs(stream, tcount, false);
			root->getNameFormatted(temp, nameformat);
			stream->writeChar16('[');
			temp+=']';
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

