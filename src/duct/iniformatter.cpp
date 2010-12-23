/**
@file iniformatter.cpp
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

duct++ IniFormatter implementation.
*/

#include <unicode/numfmt.h>
#include <duct/debug.hpp>
#include <duct/iniformatter.hpp>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

namespace duct {

// Character constants
const UChar32 CHAR_EOF = U_SENTINEL;
const UChar32 CHAR_NEWLINE = '\n';
const UChar32 CHAR_CARRIAGERETURN = '\r';

const UChar32 CHAR_DECIMALPOINT = '.';

const UChar32 CHAR_QUOTE = '\"';
const UChar32 CHAR_SEMICOLON = ';';

const UChar32 CHAR_OPENBRACKET = '[';
const UChar32 CHAR_CLOSEBRACKET = ']';
const UChar32 CHAR_EQUALSIGN = '=';

// class IniToken implementation

IniToken::IniToken() {
	_type = NoToken;
}

IniToken::IniToken(IniTokenType type) : _type(type) {
}

IniToken::~IniToken() {
	if (_buffer) {
		free(_buffer);
	}
}

void IniToken::reset(IniTokenType type) {
	_type = type;
	_buflength = 0; // Reset the write-position of the buffer, but keep the buffer intact
	_cached = false;
}

void IniToken::setBeginningPosition(int line, int col) {
	_beg_line = line;
	_beg_col = col;
}

IniTokenType IniToken::getType() const {
	return _type;
}

void IniToken::addChar(UChar32 c) {
	const size_t BUFFER_INITIAL_SIZE = 68;
	const double BUFFER_MULTIPLIER = 1.75;
	if (!_buffer) {
		_bufsize = BUFFER_INITIAL_SIZE;
		_buffer = (UChar32*)malloc(_bufsize * 4);
		_buflength = 0;
	} else if (_buflength >= _bufsize) {
		size_t newsize = ceil(_bufsize * BUFFER_MULTIPLIER);
		if (newsize < _buflength) {
			newsize = ceil(_buflength * BUFFER_MULTIPLIER);
		}
		_bufsize = newsize;
		void* temp = realloc(_buffer, newsize * 4);
		if (!temp) {
			throw IniParserException(PARSERERROR_MEMALLOC, "IniToken::addChar", NULL, NULL, "Unable to allocate buffer of size %d bytes", (newsize * 4));
		}
		_buffer = (UChar32*)temp;
	}
	_buffer[_buflength++] = c;
}

void IniToken::cacheString() {
	if (_buffer) {
		_bufstring = UnicodeString::fromUTF32(_buffer, _buflength);
	} else {
		_bufstring.remove();
	}
	_cached = true;
}

int32_t IniToken::asInt() {
	if (!_cached) {
		cacheString();
	}
	UErrorCode status = U_ZERO_ERROR;
	NumberFormat *nf = NumberFormat::createInstance(status);
	Formattable formattable;
	nf->parse(_bufstring, formattable, status);
	if (U_FAILURE(status)) {
		debug_printp(this, u_errorName(status));
		delete nf;
		return 0;
	} else {
		delete nf;
		return formattable.getLong();
	}
}

double IniToken::asDouble() {
	if (!_cached) {
		cacheString();
	}
	UErrorCode status = U_ZERO_ERROR;
	NumberFormat *nf = NumberFormat::createInstance(status);
	Formattable formattable;
	nf->parse(_bufstring, formattable, status);
	if (U_FAILURE(status)) {
		debug_printp(this, u_errorName(status));
		delete nf;
		return 0.0;
	} else {
		//printf("(IniToken::asDouble) formattable.getDouble:%f\n", formattable.getDouble());
		delete nf;
		return formattable.getDouble(status);
	}
}

void IniToken::asString(UnicodeString& str) {
	if (!_cached) {
		cacheString();
	}
	str.setTo(_bufstring);
}

const UnicodeString& IniToken::asString() {
	if (!_cached) {
		cacheString();
	}
	return _bufstring;
}

const char* IniToken::typeAsString() const {
	switch (_type) {
		case NoToken:
			return "NoToken";
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

CharacterSet IniParser::_whitespaceset = CharacterSet("\t ");
CharacterSet IniParser::_numberset = CharacterSet("0-9\\-+");
CharacterSet IniParser::_digitset = CharacterSet(".0-9\\-+");

IniParser::IniParser() {
	clean();
}

IniParser::IniParser(Stream* stream) {
	initWithStream(stream);
}

IniParser::~IniParser() {
	clean();
}

void IniParser::initWithStream(Stream* stream) {
	clean();
	_stream = stream;
	nextChar(); // Get the first character
}

void IniParser::setHandler(IniParserHandler* handler) {
	_handler = handler;
}

IniParserHandler* IniParser::getHandler() {
	return _handler;
}

const IniToken& IniParser::getToken() const {
	return _token;
}

Stream* IniParser::getStream() {
	return _stream;
}

void IniParser::clean() {
	_token.reset(NoToken);
	_line = 1;
	_col = 0;
	_stream = NULL;
	_curchar = CHAR_EOF;
}

bool IniParser::parse() {
	//nextChar();
	skipWhitespace();
	nextToken();
	readToken();
	if (_curchar == CHAR_EOF) {
		_token.reset(EOFToken);
		_handler->handleToken(_token); // Just to make sure the EOF gets handled (data might not end with a newline, causing an EOFToken)
		return false;
	} else if (_token._type == EOFToken) {
		return false;
	}
	return true;
}

UChar32 IniParser::nextChar() {
	if (_curchar == CHAR_NEWLINE) {
		_line++;
		_col = 0;
	}
	if (!_stream->eof()) {
		_curchar = _stream->readChar();
	} else {
		_curchar = CHAR_EOF;
	}
	if (_curchar == CHAR_CARRIAGERETURN) { // Skip \r
		nextChar();
	} else if (_curchar != CHAR_EOF) {
		_col++;
	}
	return _curchar;
}

void IniParser::skipWhitespace() {
	while (_curchar != CHAR_EOF && _whitespaceset.contains(_curchar))
		nextChar();
}

void IniParser::skipToEOL() {
	while (_curchar != CHAR_EOF && _curchar != CHAR_NEWLINE)
		nextChar();
}

IniToken& IniParser::nextToken() {
	_token.reset(NoToken);
	switch (_curchar) {
		case CHAR_QUOTE:
			_token._type = QuotedStringToken;
			break;
		case CHAR_SEMICOLON:
			_token._type = CommentToken;
			break;
		case CHAR_EOF:
			_token._type = EOFToken;
			break;
		case CHAR_NEWLINE:
			_token._type = EOLToken;
			break;
		case CHAR_DECIMALPOINT:
			_token._type = DoubleToken;
			_token.addChar(_curchar); // Add the decimal
			break;
		case CHAR_EQUALSIGN:
			_token._type = EqualsToken;
			break;
		case CHAR_OPENBRACKET:
			_token._type = NodeToken;
			break;
		default:
			if (_numberset.contains(_curchar)) {
				_token._type = NumberToken;
			} else {
				_token._type = StringToken;
			}
			break;
	}
	_token.setBeginningPosition(_line, _col);
	return _token;
}

void IniParser::readToken() {
	//printf("(IniParser::readToken) token-type:%s line:%d, col:%d\n", _token.typeAsString(), _token._beg_line, _token._beg_col);
	switch (_token._type) {
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
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readToken", NULL, this, "Unhandled token: %s", _token.typeAsString());
			break;
	}
	_handler->handleToken(_token);
}

void IniParser::readNumberToken() {
	while (_curchar != CHAR_EOF) {
		if (_curchar == CHAR_QUOTE) {
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readNumberToken", &_token, this, "Unexpected quote");
		} else if (_curchar == CHAR_NEWLINE || _whitespaceset.contains(_curchar) || _curchar == CHAR_SEMICOLON /*|| _curchar == CHAR_EQUALSIGN*/) {
			break;
		} else {
			if (_numberset.contains(_curchar)) {
				_token.addChar(_curchar);
			} else if (_curchar == CHAR_DECIMALPOINT) {
				_token.addChar(_curchar);
				nextChar();
				_token._type = DoubleToken;
				readDoubleToken();
				return;
			} else {
				_token._type = StringToken;
				readStringToken();
				return;
			}
		}
		nextChar();
	}
}

void IniParser::readDoubleToken() {
	while (_curchar != CHAR_EOF) {
		if (_curchar == CHAR_QUOTE) {
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readDoubleToken", &_token, this, "Unexpected quote");
		} else if (_curchar == CHAR_NEWLINE || _whitespaceset.contains(_curchar) || _curchar == CHAR_SEMICOLON /*^^^|| _curchar == CHAR_EQUALSIGN*/) {
			break;
		} else {
			if (_numberset.contains(_curchar)) {
				_token.addChar(_curchar);
			} else { // (_curchar == CHAR_DECIMALPOINT)
				// The token should've already contained a decimal point, so it must be a string.
				_token._type = StringToken;
				readStringToken();
				return;
			}
		}
		nextChar();
	}
}

void IniParser::readStringToken() {
	while (_curchar != CHAR_EOF) {
		if (_curchar == CHAR_QUOTE) {
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readStringToken", &_token, this, "Unexpected quote");
		} else if (_curchar == CHAR_NEWLINE /*^^^|| _whitespaceset.contains(_curchar)*/ || _curchar == CHAR_SEMICOLON || _curchar == CHAR_EQUALSIGN) {
			break;
		} else {
			_token.addChar(_curchar);
		}
		nextChar();
	}
}

void IniParser::readQuotedStringToken() {
	nextChar(); // Skip the first character (will be the initial quote)
	while (_curchar != CHAR_QUOTE) {
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
	while (_curchar != CHAR_EOF) {
		if (_curchar == CHAR_OPENBRACKET) {
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readNodeToken", &_token, this, "Unexpected open bracket");
		} else if (_curchar == CHAR_SEMICOLON) {
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readNodeToken", &_token, this, "Unexpected semicolon");
		} else if (_curchar == CHAR_NEWLINE) {
			throw IniParserException(PARSERERROR_PARSER, "IniParser::readNodeToken", &_token, this, "Unexpected end of line");
		} else if (_curchar == CHAR_CLOSEBRACKET /*|| _whitespaceset.contains(_curchar)*/) {
			break;
		} else {
			_token.addChar(_curchar);
		}
		nextChar();
	}
}

// class IniParserHandler implementation

IniParserHandler::~IniParserHandler() {
}

void IniParserHandler::init() {
	_parser.setHandler(this);
}

void IniParserHandler::throwex(IniParserException e) {
	throw e;
}

void IniParserHandler::clean() {
	_currentnode = NULL;
	_rootnode = NULL;
}

void IniParserHandler::process() {
	_rootnode = new Node(NULL);
	_currentnode = _rootnode;
	while (_parser.parse()) {
	}
	finish();
}

Node* IniParserHandler::processFromStream(Stream* stream) {
	_parser.initWithStream(stream);
	process();
	Node* node = _rootnode; // Store before cleaning
	clean();
	_parser.clean();
	return node;
}

// class IniParserException implementation

IniParserException::IniParserException(IniParserError error, const char* reporter, const IniToken* token, const IniParser* parser, const char* fmt, ...) {
	_error = error;
	_reporter = reporter;
	_token = token;
	_parser = parser;
	
	char temp[256];
	va_list args;
	va_start(args, fmt);
	vsprintf(temp, fmt, args);
	va_end(args);
	temp[255] = '\0';
	if (_parser && !_token)
		_token = &_parser->getToken();
	if (_token && _parser)
		sprintf(_message, "(%s) [%s] from line: %d, col: %d to line: %d, col: %d: %s", _reporter, errorToString(_error), _token->_beg_line, _token->_beg_col, _parser->_line, _parser->_col, temp);
	if (_token)
		sprintf(_message, "(%s) [%s] at line: %d, col: %d: %s", _reporter, errorToString(_error), _token->_beg_line, _token->_beg_col, temp);
	else if (_parser)
		sprintf(_message, "(%s) [%s] at line: %d, col: %d: %s", _reporter, errorToString(_error), _parser->_line, _parser->_col, temp);
	else
		sprintf(_message, "(%s) [%s]: %s", _reporter, errorToString(_error), temp);
	_message[511] = '\0';
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

// class StandardIniParserHandler implementation

StandardIniParserHandler::StandardIniParserHandler() {
	init();
}

void StandardIniParserHandler::throwex(IniParserException e) {
	freeData();
	throw e;
}

void StandardIniParserHandler::clean() {
	IniParserHandler::clean();
	_varname.remove();
	_equals = false;
}

void StandardIniParserHandler::freeData() {
	if (_currentnode) {
		if (_rootnode == _currentnode || _currentnode->getParent() != _rootnode) { // delete the root if the root and the current node are the same or if the current node has been parented
			delete _rootnode;
		} else if (_currentnode->getParent() == NULL) { // delete the root and the current node if the current node has not been parented
			delete _rootnode;
			delete _currentnode;
		}
	} else if (_rootnode) {
		delete _rootnode;
	}
	clean();
}

void StandardIniParserHandler::handleToken(IniToken& token) {
	switch (token.getType()) {
		case StringToken:
		case QuotedStringToken: {
			if (_varname.length() > 0 && _equals) {
				if (token.getType() == StringToken) {
					int bv = Variable::stringToBool(token.asString());
					if (bv != -1) {
						addValueAndReset(new BoolVariable((bool)bv, _varname));
						return;
					}
				}
				addValueAndReset(new StringVariable(token.asString(), _varname));
			} else if (_varname.length() > 0) {
				throwex(IniParserException(PARSERERROR_PARSER, "StandardIniParserHandler::handleToken", &token, &_parser, "Expected equals sign, got string"));
			} else {
				_varname.setTo(token.asString()).trim();
			}
			}
			break;
		case NumberToken:
			if (_varname.length() > 0 && _equals) {
				addValueAndReset(new IntVariable(token.asInt(), _varname));
			} else {
				throwex(IniParserException(PARSERERROR_PARSER, "StandardIniParserHandler::handleToken", &token, &_parser, "A number cannot be an identifier"));
			}
			break;
		case DoubleToken:
			if (_varname.length() > 0 && _equals) {
				addValueAndReset(new FloatVariable(token.asDouble(), _varname));
			} else {
				throwex(IniParserException(PARSERERROR_PARSER, "StandardIniParserHandler::handleToken", &token, &_parser, "A number cannot be an identifier"));
			}
			break;
		case EqualsToken:
			if (_varname.length() == 0) {
				throwex(IniParserException(PARSERERROR_PARSER, "StandardIniParserHandler::handleToken", &token, &_parser, "Expected string, got equality sign"));
			} else if (_equals) {
				throwex(IniParserException(PARSERERROR_PARSER, "StandardIniParserHandler::handleToken", &token, &_parser, "Expected value, got equality sign"));
			} else {
				_equals = true;
			}
			break;
		case NodeToken: {
			if (_varname.length() == 0) {
				_varname.setTo(token.asString()).trim();
				_currentnode = new Node(_varname, _rootnode); // Trim whitespace
				_varname.remove(); // clear the string
				_rootnode->addVariable(_currentnode);
			} else {
				throwex(IniParserException(PARSERERROR_PARSER, "StandardIniParserHandler::handleToken", &token, &_parser, "NodeToken: Unknown error. _varname length is > 0"));
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
			//printf("StandardIniParserHandler::handleToken Unhandled token of type: %s\n", token.typeAsString());
			break;
	}
}

void StandardIniParserHandler::finish() {
	if (_varname.length() > 0 && _equals) {
		addValueAndReset(new StringVariable("", _varname));
	} else if (_varname.length() > 0) {
		throwex(IniParserException(PARSERERROR_PARSER, "StandardIniParserHandler::finish", NULL, &_parser, "Expected equality sign, got EOL or EOF"));
	}
}

void StandardIniParserHandler::reset() {
	_varname.remove();
	_equals = false;
}

void StandardIniParserHandler::addValueAndReset(ValueVariable* value) {
	_currentnode->addVariable(value);
	reset();
}

// class IniFormatter implementation

StandardIniParserHandler IniFormatter::_handler = StandardIniParserHandler();

bool IniFormatter::formatValue(const ValueVariable& value, UnicodeString& result, unsigned int nameformat, unsigned int varformat) {
	if (value.getName().length() > 0) {
		UnicodeString temp;
		value.getNameFormatted(temp, nameformat);
		result.setTo(temp);
		value.getValueFormatted(temp, varformat);
		result += "=" + temp;
		return true;
	} else {
		result.remove(); // clear the result string
		debug_print("Value name is 0-length");
	}
	return false;
}

Node* IniFormatter::loadFromFile(const char* path, const char* encoding) {
	Stream* stream = FileStream::readFile(path, encoding);
	if (stream) {
		Node* root = _handler.processFromStream(stream);
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
	Stream* stream = FileStream::writeFile(path, encoding);
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
		if (root->getParent() && root->getName().length() > 0) { // cheap way of saying the node is not a root node
			writeTabs(stream, tcount, false);
			root->getNameFormatted(temp, nameformat);
			stream->writeChar16('[');
			temp += ']';
			stream->writeLine(temp);
		}
		Node* node;
		ValueVariable* value;
		for (VarList::const_iterator iter = root->begin(); iter != root->end(); ++iter) {
			value = dynamic_cast<ValueVariable*>(*iter);
			node = dynamic_cast<Node*>(*iter);
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
	while (0 < count--) {
		stream->writeChar16('\t');
	}
	if (newline)
		stream->writeChar16('\n');
}

} // namespace duct

