/**
@file scriptformatter.cpp
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

duct++ Quake-style script formatter implementation.
*/

#include <duct/debug.hpp>
#include <duct/scriptformatter.hpp>
#include <unicode/numfmt.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

namespace duct {

// Character constants
const UChar32 CHAR_EOF=U_SENTINEL;
const UChar32 CHAR_NEWLINE='\n';
const UChar32 CHAR_CARRIAGERETURN='\r';

const UChar32 CHAR_DECIMALPOINT='.';

const UChar32 CHAR_QUOTE='\"';
const UChar32 CHAR_SLASH='/';
const UChar32 CHAR_BACKSLASH='\\';
const UChar32 CHAR_ASTERISK='*';

const UChar32 CHAR_OPENBRACE='{';
const UChar32 CHAR_CLOSEBRACE='}';
const UChar32 CHAR_EQUALSIGN='=';

// class ScriptToken implementation

ScriptToken::ScriptToken() : _type(NoToken), _buffer(NULL), _bufsize(0) {
}

ScriptToken::ScriptToken(ScriptTokenType type) : _type(type), _buffer(NULL), _bufsize(0) {
}

ScriptToken::~ScriptToken() {
	if (_buffer) {
		free(_buffer);
	}
}

void ScriptToken::reset(ScriptTokenType type) {
	_type=type;
	_buflength=0; // Reset the write-position of the buffer, but keep the buffer intact
	_cached=false;
}

void ScriptToken::setBeginningPosition(int line, int col) {
	_beg_line=line;
	_beg_col=col;
}

ScriptTokenType ScriptToken::getType() const {
	return _type;
}

void ScriptToken::addChar(UChar32 c) {
	const size_t BUFFER_INITIAL_SIZE=68;
	const double BUFFER_MULTIPLIER=1.75;
	if (!_buffer) {
		_bufsize=BUFFER_INITIAL_SIZE;
		_buffer=(UChar32*)malloc(_bufsize*4);
		_buflength=0;
	} else if (_buflength>=_bufsize) {
		size_t newsize=ceil(_bufsize*BUFFER_MULTIPLIER);
		if (newsize<_buflength) {
			newsize=ceil(_buflength*BUFFER_MULTIPLIER);
		}
		_bufsize=newsize;
		void* temp=realloc(_buffer, newsize*4);
		if (!temp) {
			throw ScriptParserException(PARSERERROR_MEMALLOC, "ScriptToken::addChar", NULL, NULL, "Unable to allocate buffer of size %d bytes", (newsize*4));
		}
		_buffer=(UChar32*)temp;
	}
	_buffer[_buflength++]=c;
	_cached=false;
}

void ScriptToken::cacheString() {
	if (_buffer) {
		_bufstring=UnicodeString::fromUTF32(_buffer, _buflength);
	} else {
		_bufstring.remove();
	}
	_cached=true;
}

bool ScriptToken::compare(const CharacterSet& set) {
	for (unsigned int i=0; i<_buflength; ++i) {
		if (!set.contains(_buffer[i]))
			return false;
	}
	return true;
}

bool ScriptToken::compare(UChar32 c) {
	for (unsigned int i=0; i<_buflength; ++i) {
		if (_buffer[i]!=c)
			return false;
	}
	return true;
}

int32_t ScriptToken::asInt() {
	if (!_cached) {
		cacheString();
	}
	UErrorCode status=U_ZERO_ERROR;
	NumberFormat *nf=NumberFormat::createInstance(status);
	Formattable formattable;
	nf->parse(_bufstring, formattable, status);
	if (U_FAILURE(status)) {
		debug_printp_source(this, u_errorName(status));
		delete nf;
		return 0;
	} else {
		delete nf;
		return formattable.getLong();
	}
}

double ScriptToken::asDouble() {
	if (!_cached) {
		cacheString();
	}
	UErrorCode status=U_ZERO_ERROR;
	NumberFormat *nf=NumberFormat::createInstance(status);
	Formattable formattable;
	nf->parse(_bufstring, formattable, status);
	if (U_FAILURE(status)) {
		debug_printp_source(this, u_errorName(status));
		delete nf;
		return 0.0;
	} else {
		//printf("(ScriptToken::asDouble) formattable.getDouble:%f\n", formattable.getDouble());
		delete nf;
		return formattable.getDouble(status);
	}
}

void ScriptToken::asString(UnicodeString& str) {
	if (!_cached) {
		cacheString();
	}
	str.setTo(_bufstring);
}

const UnicodeString& ScriptToken::asString() {
	if (!_cached) {
		cacheString();
	}
	return _bufstring;
}

const char* ScriptToken::typeAsString() const {
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

CharacterSet ScriptParser::_whitespaceset=CharacterSet("\t ");
//CharacterSet ScriptParser::_eolset=CharacterSet('\n');
CharacterSet ScriptParser::_numberset=CharacterSet("0-9\\-+");
CharacterSet ScriptParser::_signset=CharacterSet("\\-+");

ScriptParser::ScriptParser() {
	clean();
}

ScriptParser::ScriptParser(Stream* stream) {
	initWithStream(stream);
}

ScriptParser::~ScriptParser() {
	clean();
}

void ScriptParser::initWithStream(Stream* stream) {
	clean();
	_stream=stream;
	nextChar(); // Get the first character
}

void ScriptParser::setHandler(ScriptParserHandler* handler) {
	_handler=handler;
}

ScriptParserHandler* ScriptParser::getHandler() {
	return _handler;
}

const ScriptToken& ScriptParser::getToken() const {
	return _token;
}

Stream* ScriptParser::getStream() {
	return _stream;
}

void ScriptParser::clean() {
	_token.reset(NoToken);
	_line=1;
	_col=0;
	_stream=NULL;
	_curchar=CHAR_EOF;
	_peekchar=CHAR_EOF;
	_peeked=false;
}

bool ScriptParser::parse() {
	//nextChar();
	skipWhitespace();
	nextToken();
	readToken();
	if (_curchar==CHAR_EOF) {
		_token.reset(EOFToken);
		_handler->handleToken(_token); // Just to make sure the EOF gets handled (data might not end with a newline, causing an EOFToken)
		return false;
	} else if (_token._type==EOFToken) {
		return false;
	}
	return true;
}

UChar32 ScriptParser::nextChar() {
	if (_curchar==CHAR_NEWLINE) {
		_line++;
		_col=0;
	}
	if (_peeked) {
		_curchar=_peekchar;
		_peeked=false;
	} else if (!_stream->eof()) {
		_curchar=_stream->readChar();
	} else {
		_curchar=CHAR_EOF;
	}
	if (_curchar==CHAR_CARRIAGERETURN) { // Skip \r -- IT WAS NEVER THERE
		nextChar();
	} else if (_curchar!=CHAR_EOF) {
		_col++;
	}
	return _curchar;
}

UChar32 ScriptParser::peekChar() {
	if (!_peeked) {
		if (!_stream->eof())
			_peekchar=_stream->readChar();
		else
			_peekchar=CHAR_EOF;
		_peeked=true;
	}
	return _peekchar;
}

void ScriptParser::skipWhitespace() {
	while (_curchar!=CHAR_EOF && _whitespaceset.contains(_curchar))
		nextChar();
}

void ScriptParser::skipToEOL() {
	while (_curchar!=CHAR_EOF && _curchar!=CHAR_NEWLINE /*&& !_eolset.contains(_curchar)*/)
		nextChar();
}

bool ScriptParser::skipToChar(UChar32 c) {
	while (_curchar!=CHAR_EOF && _curchar!=c)
		nextChar();
	return _curchar==c;
}

ScriptToken& ScriptParser::nextToken() {
	_token.reset(NoToken);
	_token.setBeginningPosition(_line, _col);
	switch (_curchar) {
		case CHAR_QUOTE:
			_token._type=QuotedStringToken;
			break;
		case CHAR_ASTERISK:
			if (peekChar()==CHAR_SLASH)
				throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::nextToken", &_token, this, "Encountered unexpected end of block comment");
			_token._type=StringToken;
			break;
		case CHAR_SLASH:
			if (peekChar()==CHAR_SLASH)
				_token._type=CommentToken;
			else if (_peekchar==CHAR_ASTERISK)
				_token._type=CommentBlockToken;
			else
				_token._type=StringToken;
			break;
		case CHAR_EOF:
			_token._type=EOFToken;
			break;
		case CHAR_NEWLINE:
			_token._type=EOLToken;
			break;
		case CHAR_DECIMALPOINT:
			_token._type=DoubleToken;
			_token.addChar(_curchar); // Add the decimal
			break;
		case CHAR_EQUALSIGN:
			_token._type=EqualsToken;
			break;
		case CHAR_OPENBRACE:
			_token._type=OpenBraceToken;
			break;
		case CHAR_CLOSEBRACE:
			_token._type=CloseBraceToken;
			break;
		default:
			_token._type=(_numberset.contains(_curchar)) ? NumberToken : StringToken;
			break;
	}
	return _token;
}

void ScriptParser::readToken() {
	//printf("(ScriptParser::readToken) token-type:%s line:%d, col:%d\n", _token.typeAsString(), _token._beg_line, _token._beg_col);
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
			throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::readToken", NULL, this, "Unhandled token: %s", _token.typeAsString());
			break;
	}
	// Special resolve when Number and Double tokens only contain signs or periods
	switch (_token._type) {
		case NumberToken:
			if (_token.compare(_signset))
				_token._type=StringToken;
			break;
		case DoubleToken:
			if (_token.compare(_signset) || _token.compare(CHAR_DECIMALPOINT))
				_token._type=StringToken;
			break;
		default:
			break;
	}
	_handler->handleToken(_token);
}

void ScriptParser::readNumberToken() {
	while (_curchar!=CHAR_EOF) {
		if (_curchar==CHAR_QUOTE) {
			throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::readNumberToken", &_token, this, "Unexpected quote");
		} else if (_curchar==CHAR_SLASH) {
			if (peekChar()==CHAR_SLASH || _peekchar==CHAR_ASTERISK) {
				break;
			} else {
				_token._type=StringToken;
				readStringToken();
				return;
			}
		} else if (_curchar==CHAR_NEWLINE || _whitespaceset.contains(_curchar) || _curchar==CHAR_CLOSEBRACE || _curchar==CHAR_EQUALSIGN) {
			break;
		} else {
			if (_numberset.contains(_curchar)) {
				_token.addChar(_curchar);
			} else if (_curchar==CHAR_DECIMALPOINT) {
				_token.addChar(_curchar);
				nextChar();
				_token._type=DoubleToken;
				readDoubleToken();
				return;
			} else {
				_token._type=StringToken;
				readStringToken();
				return;
			}
		}
		nextChar();
	}
}

void ScriptParser::readDoubleToken() {
	while (_curchar!=CHAR_EOF) {
		if (_curchar==CHAR_QUOTE) {
			throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::readDoubleToken", &_token, this, "Unexpected quote");
		} else if (_curchar==CHAR_SLASH) {
			if (peekChar()==CHAR_SLASH || _peekchar==CHAR_ASTERISK) {
				break;
			} else {
				_token._type=StringToken;
				readStringToken();
				return;
			}
		} else if (_curchar==CHAR_NEWLINE || _whitespaceset.contains(_curchar) || _curchar==CHAR_CLOSEBRACE || _curchar==CHAR_EQUALSIGN) {
			break;
		} else {
			if (_numberset.contains(_curchar)) {
				_token.addChar(_curchar);
			} else { // (_curchar==CHAR_DECIMALPOINT)
				// The token should've already contained a decimal point, so it must be a string.
				_token._type=StringToken;
				readStringToken();
				return;
			}
		}
		nextChar();
	}
}

void ScriptParser::readStringToken() {
	while (_curchar!=CHAR_EOF) {
		if (_curchar==CHAR_QUOTE) {
			throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::readStringToken", &_token, this, "Unexpected quote");
		} else if (_curchar==CHAR_NEWLINE || _whitespaceset.contains(_curchar)
					|| (_curchar==CHAR_SLASH && (peekChar()==CHAR_SLASH || _peekchar==CHAR_ASTERISK))
					|| _curchar==CHAR_CLOSEBRACE || _curchar==CHAR_EQUALSIGN) {
			break;
		} else {
			_token.addChar(_curchar);
		}
		nextChar();
	}
}

void ScriptParser::readQuotedStringToken() {
	bool eolreached=false;
	nextChar(); // Skip the first character (it will be the initial quote)
	while (_curchar!=CHAR_QUOTE) {
		if (_curchar==CHAR_EOF) {
			throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::readQuotedStringToken", &_token, this, "Encountered EOF whilst reading quoted string");
		} else {
			if (!eolreached)
				_token.addChar(_curchar);
			if (_curchar==CHAR_NEWLINE) {
				//throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::readQuotedStringToken", &_token, this, "Unclosed quote (met EOL character)");
				eolreached=true;
			} else if (eolreached && !_whitespaceset.contains(_curchar)) {
				eolreached=false;
				_token.addChar(_curchar);
			}
		}
		nextChar();
	}
}

void ScriptParser::readCommentBlockToken() {
	nextChar(); // Skip the first character (it will be an asterisk)
	if (_curchar!=CHAR_EOF) {
		while (skipToChar(CHAR_ASTERISK)) {
			if (nextChar()==CHAR_SLASH) {
				nextChar(); // Get the next character, otherwise the nextToken() call will try to handle the slash
				return;
			}
		}
	}
	throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::readCommentBlock", &_token, this, "Unexpected EOF");
}

// class ScriptParserHandler implementation

ScriptParserHandler::~ScriptParserHandler() {
}

void ScriptParserHandler::init() {
	_parser.setHandler(this);
}

void ScriptParserHandler::throwex(ScriptParserException e) {
	throw e;
}

void ScriptParserHandler::clean() {
	_currentnode=NULL;
	_rootnode=NULL;
}

void ScriptParserHandler::process() {
	_rootnode=new Node(NULL);
	_currentnode=_rootnode;
	while (_parser.parse()) {
	}
	finish();
	if (_currentnode!=_rootnode) {
		throwex(ScriptParserException(PARSERERROR_HIERARCHY, "ScriptParserHandler::process", NULL, NULL, "The current node does not match the root node"));
	}
}

Node* ScriptParserHandler::processFromStream(Stream* stream) {
	_parser.initWithStream(stream);
	process();
	Node* node=_rootnode; // Store before cleaning
	clean();
	_parser.clean();
	return node;
}

// class ScriptParserException implementation

ScriptParserException::ScriptParserException(ScriptParserError error, const char* reporter, const ScriptToken* token, const ScriptParser* parser, const char* fmt, ...) {
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
	if (_parser && !_token)
		_token=&_parser->getToken();
	if (_token && _parser)
		sprintf(_message, "(%s) [%s] from line: %d, col: %d to line: %d, col: %d: %s", _reporter, errorToString(_error), _token->_beg_line, _token->_beg_col, _parser->_line, _parser->_col, temp);
	if (_token)
		sprintf(_message, "(%s) [%s] at line: %d, col: %d: %s", _reporter, errorToString(_error), _token->_beg_line, _token->_beg_col, temp);
	else if (_parser)
		sprintf(_message, "(%s) [%s] at line: %d, col: %d: %s", _reporter, errorToString(_error), _parser->_line, _parser->_col, temp);
	else
		sprintf(_message, "(%s) [%s]: %s", _reporter, errorToString(_error), temp);
	_message[511]='\0';
}

const char* ScriptParserException::what() const throw() {
	return _message;
}

const char* ScriptParserException::errorToString(ScriptParserError error) {
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

// class StandardScriptParserHandler implementation

StandardScriptParserHandler::StandardScriptParserHandler() {
	init();
}

void StandardScriptParserHandler::throwex(ScriptParserException e) {
	freeData();
	throw e;
}

void StandardScriptParserHandler::clean() {
	ScriptParserHandler::clean();
	_varname.remove();
	_equals=false;
	_currentvalue=NULL;
	_currentiden=NULL;
}

void StandardScriptParserHandler::freeData() {
	if (_currentvalue) {
		delete _currentvalue;
	}
	if (_currentiden) {
		delete _currentiden;
	}
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

void StandardScriptParserHandler::handleToken(ScriptToken& token) {
	switch (token.getType()) {
		case StringToken: {
			if (_varname.length()>0 && _equals) {
				int bv=Variable::stringToBool(token.asString());
				if (bv!=-1) {
					addVariableAndReset(_currentnode, new BoolVariable((bool)bv, _varname), false, false);
					return;
				} else {
					addVariableAndReset(_currentnode, new StringVariable(token.asString(), _varname), false, false);
				}
			} else if ((_varname.length()>0 || _currentiden) && !_equals) {
				makeIdentifier(&token);
				int bv=Variable::stringToBool(token.asString());
				if (bv!=-1) {
					addVariableAndReset(_currentiden, new BoolVariable((bool)bv), false, false);
					return;
				}
				addVariableAndReset(_currentiden, new StringVariable(token.asString()), false, false);
			} else {// if (!_varname.length()>0 && !_equals) {
				_varname=token.asString();
			}
			}
			break;
		case QuotedStringToken:
			if (_varname.length()>0 && _equals) {
				addVariableAndReset(_currentnode, new StringVariable(token.asString(), _varname), false, false);
			} else if ((_varname.length()>0 || _currentiden) && !_equals) {
				makeIdentifier(&token);
				addVariableAndReset(_currentiden, new StringVariable(token.asString()), false, false);
			} else {// if (!_varname.length()>0 && !_equals) {
				_varname=token.asString();
			}
			break;
		case NumberToken:
			if (_varname.length()>0 && _equals) {
				_currentvalue=new IntVariable(token.asInt(), _varname);
				addVariableAndReset(_currentnode, _currentvalue, false, false);
			} else if ((_varname.length()>0 || _currentiden) && !_equals) {
				makeIdentifier(&token);
				addVariableAndReset(_currentiden, new IntVariable(token.asInt()), false, false);
			} else {
				//throwex(ScriptParserException(PARSERERROR_PARSER, "StandardScriptParserHandler::handleToken", &token, &_parser, "A number cannot be an identifier"));
				_varname.setTo(token.asString());
			}
			break;
		case DoubleToken:
			if (_varname.length()>0 && _equals) {
				_currentvalue=new FloatVariable(token.asDouble(), _varname);
				addVariableAndReset(_currentnode, _currentvalue, false, false);
			} else if ((_varname.length()>0 || _currentiden) && !_equals) {
				makeIdentifier(&token);
				addVariableAndReset(_currentiden, new FloatVariable(token.asDouble()), false, false);
			} else {
				//throwex(ScriptParserException(PARSERERROR_PARSER, "StandardScriptParserHandler::handleToken", &token, &_parser, "A number cannot be an identifier"));
				_varname.setTo(token.asString());
			}
			break;
		case EqualsToken:
			if (_currentiden) {
				throwex(ScriptParserException(PARSERERROR_PARSER, "StandardScriptParserHandler::handleToken", &token, &_parser, "Unexpected equality sign after identifier declaration"));
			} else if (_varname.length()==0) {
				throwex(ScriptParserException(PARSERERROR_PARSER, "StandardScriptParserHandler::handleToken", &token, &_parser, "Expected string, got equality sign"));
			} else if (_equals) {
				throwex(ScriptParserException(PARSERERROR_PARSER, "StandardScriptParserHandler::handleToken", &token, &_parser, "Expected value, got equality sign"));
			} else {
				_equals=true;
			}
			break;
		case OpenBraceToken: {
			if (_currentiden) {
				throwex(ScriptParserException(PARSERERROR_PARSER, "StandardScriptParserHandler::handleToken", &token, &_parser, "Node cannot contain values (possible openbrace typo)"));
			}
			Node* tempnode=new Node(_varname, _currentnode);
			addVariableAndReset(_currentnode, tempnode, false, false);
			_currentnode=tempnode;
			}
			break;
		case CloseBraceToken:
			if (!_currentnode->getParent()) {
				throwex(ScriptParserException(PARSERERROR_PARSER, "StandardScriptParserHandler::handleToken", &token, &_parser, "Mismatched node brace"));
			} else if (_equals) {
				throwex(ScriptParserException(PARSERERROR_PARSER, "StandardScriptParserHandler::handleToken", &token, &_parser, "Expected value, got close-brace"));
			} else {
				if (_currentiden) {
					reset(true, true);
				}
				_currentnode=(Node*)_currentnode->getParent();
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
			//DebugLog("(StandardScriptParserHandler::handleToken) Unhandled token of type "+token.typeAsString())
			break;
	}
}

void StandardScriptParserHandler::finish() {
	if (_parser.getToken().getType()==EOLToken && _equals) {
		throwex(ScriptParserException(PARSERERROR_PARSER, "StandardScriptParserHandler::finish", &_parser.getToken(), &_parser, "Expected value, got EOL/EOF"));
	} else if (_varname.length()>0) { // no-value identifier
		makeIdentifier(NULL, true, true, true);
	} else {
		reset(true, true);
	}
}

void StandardScriptParserHandler::reset(bool iden, bool value) {
	_varname.remove();
	_equals=false;
	if (value) {
		_currentvalue=NULL;
	}
	if (iden) {
		_currentiden=NULL;
	}
}

void StandardScriptParserHandler::addVariableAndReset(CollectionVariable* collection, Variable* variable, bool iden, bool value) {
	collection->addVariable(variable);
	reset(iden, value);
}

void StandardScriptParserHandler::makeIdentifier(const ScriptToken* token, bool resetiden, bool resetvalue, bool force) {
	//if (_currentvalue) {
	//	throw ScriptParserException(PARSERERROR_PARSER, "StandardScriptParserHandler::makeIdentifier", token, NULL, "Value already defined on line");
	if (!_currentiden || force) {
		/*int bv=Variable::stringToBool(_varname);
		if (bv!=-1) {
			throwex(ScriptParserException(PARSERERROR_PARSER, "StandardScriptParserHandler::makeIdentifier", token, &_parser, "Identifier name cannot be boolean value"));
		}*/
		_currentiden=new Identifier(_varname);
		addVariableAndReset(_currentnode, _currentiden, resetiden, resetvalue);
	//} else {
		//throw ScriptParserException(PARSERERROR_PARSER, "StandardScriptParserHandler::makeIdentifier", token, NULL, "Unknown error")
	}
}

// class ScriptFormatter implementation

StandardScriptParserHandler ScriptFormatter::_handler=StandardScriptParserHandler();

bool ScriptFormatter::formatIdentifier(const Identifier& iden, UnicodeString& result, unsigned int nameformat, unsigned int varformat) {
	if (iden.getName().length()>0) {
		UnicodeString temp;
		iden.getNameFormatted(temp, nameformat);
		result.setTo(temp);
		ValueVariable* val;
		for (VarList::const_iterator iter=iden.begin(); iter!=iden.end(); ++iter) {
			val=dynamic_cast<ValueVariable*>(*iter);
			if (val) {
				/*temp=UnicodeString(val->getTypeAsString());
				result+=" <"+temp+">";
				val->getValueFormatted(temp, varformat);
				result+=temp;*/
				val->getValueFormatted(temp, varformat);
				result+=' '+temp;
			}
		}
		return true;
	} else {
		result.remove(); // clear the result string
		debug_print("Identifier name is 0-length");
	}
	return false;
}

bool ScriptFormatter::formatValue(const ValueVariable& value, UnicodeString& result, unsigned int nameformat, unsigned int varformat) {
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

Node* ScriptFormatter::loadFromFile(const char* path, const char* encoding) {
	Stream* stream=FileStream::readFile(path, encoding);
	if (stream) {
		Node* root=_handler.processFromStream(stream);
		stream->close();
		delete stream;
		return root;
	}
	return NULL;
}

Node* ScriptFormatter::loadFromFile(const std::string& path, const char* encoding) {
	return loadFromFile(path.c_str(), encoding);
}

Node* ScriptFormatter::loadFromFile(const UnicodeString& path, const char* encoding) {
	std::string temp;
	path.toUTF8String(temp);
	return loadFromFile(temp.c_str(), encoding);
}

Node* ScriptFormatter::loadFromStream(Stream* stream) {
	if (stream) {
		return _handler.processFromStream(stream);
	}
	return NULL;
}

bool ScriptFormatter::writeToFile(const Node* root, const char* path, const char* encoding, unsigned int nameformat, unsigned int varformat) {
	Stream* stream=FileStream::writeFile(path, encoding);
	if (stream) {
		writeToStream(root, stream, 0, nameformat, varformat);
		stream->close();
		return true;
	}
	return false;
}

bool ScriptFormatter::writeToFile(const Node* root, const std::string& path, const char* encoding, unsigned int nameformat, unsigned int varformat) {
	return writeToFile(root, path.c_str(), encoding, nameformat, varformat);
}

bool ScriptFormatter::writeToFile(const Node* root, const UnicodeString& path, const char* encoding, unsigned int nameformat, unsigned int varformat) {
	std::string temp;
	path.toUTF8String(temp);
	return writeToFile(root, temp.c_str(), encoding, nameformat, varformat);
}

bool ScriptFormatter::writeToStream(const Node* root, Stream* stream, unsigned int tcount, unsigned int nameformat, unsigned int varformat) {
	if (root && stream) {
		UnicodeString temp;
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

