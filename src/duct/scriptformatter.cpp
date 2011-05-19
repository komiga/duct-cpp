/**
@file scriptformatter.cpp
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
#include <duct/scriptformatter.hpp>
#include <duct/charutils.hpp>

namespace duct {

const char* __script_tokenName(const Token& token) {
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

CharacterSet ScriptParser::_whitespaceset=CharacterSet("\t ");
//CharacterSet ScriptParser::_eolset=CharacterSet('\n');
CharacterSet ScriptParser::_numberset=CharacterSet("0-9\\-+");
CharacterSet ScriptParser::_signset=CharacterSet("\\-+");

ScriptParser::ScriptParser() : _handler(NULL) {
	reset();
}

ScriptParser::ScriptParser(Stream* stream) {
	initWithStream(stream);
}

ScriptParser::~ScriptParser() {
	reset();
}

void ScriptParser::setHandler(ParserHandler* handler) {
	_handler=(ScriptParserHandler*)handler;
}

ParserHandler* ScriptParser::getHandler() {
	return _handler;
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
	} else if (_token.getType()==EOFToken) {
		return false;
	}
	return true;
}

void ScriptParser::skipWhitespace() {
	while (_curchar!=CHAR_EOF && _whitespaceset.contains(_curchar))
		nextChar();
}

Token& ScriptParser::nextToken() {
	_token.reset(NULL_TOKEN);
	_token.setPosition(_line, _column);
	switch (_curchar) {
		case CHAR_QUOTE:
			_token.setType(QuotedStringToken);
			break;
		case CHAR_ASTERISK:
			if (peekChar()==CHAR_SLASH)
				throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::nextToken", &_token, this, "Encountered unexpected end of block comment");
			_token.setType(StringToken);
			break;
		case CHAR_SLASH:
			if (peekChar()==CHAR_SLASH)
				_token.setType(CommentToken);
			else if (_peekchar==CHAR_ASTERISK)
				_token.setType(CommentBlockToken);
			else
				_token.setType(StringToken);
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
		case CHAR_OPENBRACE:
			_token.setType(OpenBraceToken);
			break;
		case CHAR_CLOSEBRACE:
			_token.setType(CloseBraceToken);
			break;
		default:
			_token.setType((_numberset.contains(_curchar)) ? NumberToken : StringToken);
			break;
	}
	return _token;
}

void ScriptParser::readToken() {
	//printf("(ScriptParser::readToken) token-type:%s line:%d, col:%d\n", __script_tokenName(_token), _token.getLine(), _token.getColumn());
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
			throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::readToken", NULL, this, "Unhandled token: %s", __script_tokenName(_token));
			break;
	}
	// Special resolve when Number and Double tokens only contain signs or periods
	switch (_token.getType()) {
		case NumberToken:
			if (_token.compare(_signset))
				_token.setType(StringToken);
			break;
		case DoubleToken:
			if (_token.compare(_signset) || _token.compare(CHAR_DECIMALPOINT))
				_token.setType(StringToken);
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
				_token.setType(StringToken);
				readStringToken();
				return;
			}
		} else if (_curchar==CHAR_NEWLINE || _whitespaceset.contains(_curchar) || _curchar==CHAR_CLOSEBRACE || _curchar==CHAR_EQUALSIGN) {
			break;
		} else if (_numberset.contains(_curchar)) {
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
				_token.setType(StringToken);
				readStringToken();
				return;
			}
		} else if (_curchar==CHAR_NEWLINE || _whitespaceset.contains(_curchar) || _curchar==CHAR_CLOSEBRACE || _curchar==CHAR_EQUALSIGN) {
			break;
		} else if (_numberset.contains(_curchar)) {
			_token.addChar(_curchar);
		} else { // (_curchar==CHAR_DECIMALPOINT)
			// The token should've already contained a decimal point, so it must be a string.
			_token.setType(StringToken);
			readStringToken();
			return;
		}
		nextChar();
	}
}

void ScriptParser::readStringToken() {
	while (_curchar!=CHAR_EOF) {
		if (_curchar==CHAR_QUOTE) {
			throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::readStringToken", &_token, this, "Unexpected quote");
		} else if (_curchar==CHAR_BACKSLASH) {
			UChar32 c=CharUtils::getEscapeChar(nextChar());
			if (c!=CHAR_EOF) {
				_token.addChar(c);
			} else {
				throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::readStringToken", &_token, this, "Unexpected escape sequence: %c", _curchar);
			}
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
		} else if (_curchar==CHAR_BACKSLASH) {
			UChar32 c=CharUtils::getEscapeChar(nextChar());
			if (c!=CHAR_EOF) {
				_token.addChar(c);
			} else {
				throw ScriptParserException(PARSERERROR_PARSER, "ScriptParser::readStringToken", &_token, this, "Unexpected escape sequence: %c", _curchar);
			}
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

// class ScriptParserException implementation

ScriptParserException::ScriptParserException(ScriptParserError error, const char* reporter, const Token* token, const ScriptParser* parser, const char* fmt, ...) {
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

// class ScriptParserHandler implementation

ScriptParserHandler::ScriptParserHandler(ScriptParser& parser) : _parser(parser), _equals(false), _currentiden(NULL), _currentvalue(NULL), _rootnode(NULL), _currentnode(NULL) {
	_parser.setHandler(this);
}

ScriptParserHandler::~ScriptParserHandler() {
}

void ScriptParserHandler::setParser(Parser& parser) {
	_parser=(ScriptParser&)parser;
	_parser.setHandler(this);
}

Parser& ScriptParserHandler::getParser() {
	return _parser;
}

void ScriptParserHandler::throwex(ScriptParserException e) {
	freeData();
	throw e;
}

void ScriptParserHandler::clean() {
	_varname.remove();
	_equals=false;
	_currentvalue=NULL;
	_currentiden=NULL;
}

bool ScriptParserHandler::process() {
	_rootnode=new Node(NULL);
	_currentnode=_rootnode;
	while (_parser.parse()) {
	}
	finish();
	if (_currentnode!=_rootnode) {
		//throwex(ScriptParserException(PARSERERROR_HIERARCHY, "ScriptParserHandler::process", NULL, NULL, "The current node does not match the root node"));
		return false;
	}
	return true;
}

void ScriptParserHandler::handleToken(Token& token) {
	switch (token.getType()) {
		case StringToken: {
			if (_varname.length()>0 && _equals) {
				int bv=Variable::stringToBool(token.toString());
				if (bv!=-1) {
					addVariableAndReset(_currentnode, new BoolVariable((bool)bv, _varname), false, false);
					return;
				} else {
					addVariableAndReset(_currentnode, new StringVariable(token.toString(), _varname), false, false);
				}
			} else if ((_varname.length()>0 || _currentiden) && !_equals) {
				makeIdentifier(&token);
				int bv=Variable::stringToBool(token.toString());
				if (bv!=-1) {
					addVariableAndReset(_currentiden, new BoolVariable((bool)bv), false, false);
					return;
				}
				addVariableAndReset(_currentiden, new StringVariable(token.toString()), false, false);
			} else {// if (!_varname.length()>0 && !_equals) {
				_varname=token.toString();
			}
			}
			break;
		case QuotedStringToken:
			if (_varname.length()>0 && _equals) {
				addVariableAndReset(_currentnode, new StringVariable(token.toString(), _varname), false, false);
			} else if ((_varname.length()>0 || _currentiden) && !_equals) {
				makeIdentifier(&token);
				addVariableAndReset(_currentiden, new StringVariable(token.toString()), false, false);
			} else {// if (!_varname.length()>0 && !_equals) {
				_varname=token.toString();
			}
			break;
		case NumberToken:
			if (_varname.length()>0 && _equals) {
				_currentvalue=new IntVariable(token.toInt(), _varname);
				addVariableAndReset(_currentnode, _currentvalue, false, false);
			} else if ((_varname.length()>0 || _currentiden) && !_equals) {
				makeIdentifier(&token);
				addVariableAndReset(_currentiden, new IntVariable(token.toInt()), false, false);
			} else {
				//throwex(ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::handleToken", &token, &_parser, "A number cannot be an identifier"));
				_varname.setTo(token.toString());
			}
			break;
		case DoubleToken:
			if (_varname.length()>0 && _equals) {
				_currentvalue=new FloatVariable(token.toDouble(), _varname);
				addVariableAndReset(_currentnode, _currentvalue, false, false);
			} else if ((_varname.length()>0 || _currentiden) && !_equals) {
				makeIdentifier(&token);
				addVariableAndReset(_currentiden, new FloatVariable(token.toDouble()), false, false);
			} else {
				//throwex(ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::handleToken", &token, &_parser, "A number cannot be an identifier"));
				_varname.setTo(token.toString());
			}
			break;
		case EqualsToken:
			if (_currentiden) {
				throwex(ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::handleToken", &token, &_parser, "Unexpected equality sign after identifier declaration"));
			} else if (_varname.length()==0) {
				throwex(ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::handleToken", &token, &_parser, "Expected string, got equality sign"));
			} else if (_equals) {
				throwex(ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::handleToken", &token, &_parser, "Expected value, got equality sign"));
			} else {
				_equals=true;
			}
			break;
		case OpenBraceToken: {
			if (_currentiden) {
				throwex(ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::handleToken", &token, &_parser, "Node cannot contain values (possible openbrace typo)"));
			}
			Node* tempnode=new Node(_varname, _currentnode);
			addVariableAndReset(_currentnode, tempnode, false, false);
			_currentnode=tempnode;
			}
			break;
		case CloseBraceToken:
			if (!_currentnode->getParent()) {
				throwex(ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::handleToken", &token, &_parser, "Mismatched node brace"));
			} else if (_equals) {
				throwex(ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::handleToken", &token, &_parser, "Expected value, got close-brace"));
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
			//printf("(ScriptParserHandler::handleToken) Unhandled token of type "+__script_tokenName(token))
			break;
	}
}

void ScriptParserHandler::finish() {
	if (_parser.getToken().getType()==EOLToken && _equals) {
		throwex(ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::finish", &_parser.getToken(), &_parser, "Expected value, got EOL/EOF"));
	} else if (_varname.length()>0) { // no-value identifier
		makeIdentifier(NULL, true, true, true);
	} else {
		reset(true, true);
	}
}

Node* ScriptParserHandler::processFromStream(Stream* stream) {
	_parser.initWithStream(stream);
	process();
	Node* node=_rootnode; // Store before cleaning
	clean();
	_parser.reset();
	return node;
}

void ScriptParserHandler::freeData() {
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

void ScriptParserHandler::reset(bool iden, bool value) {
	_varname.remove();
	_equals=false;
	if (value) {
		_currentvalue=NULL;
	}
	if (iden) {
		_currentiden=NULL;
	}
}

void ScriptParserHandler::addVariableAndReset(CollectionVariable* collection, Variable* variable, bool iden, bool value) {
	collection->add(variable);
	reset(iden, value);
}

void ScriptParserHandler::makeIdentifier(const Token* token, bool resetiden, bool resetvalue, bool force) {
	//if (_currentvalue) {
	//	throw ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::makeIdentifier", token, NULL, "Value already defined on line");
	if (!_currentiden || force) {
		/*int bv=Variable::stringToBool(_varname);
		if (bv!=-1) {
			throwex(ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::makeIdentifier", token, _parser, "Identifier name cannot be boolean value"));
		}*/
		_currentiden=new Identifier(_varname);
		addVariableAndReset(_currentnode, _currentiden, resetiden, resetvalue);
	//} else {
		//throw ScriptParserException(PARSERERROR_PARSER, "ScriptParserHandler::makeIdentifier", token, NULL, "Unknown error")
	}
}

// class ScriptFormatter implementation

ScriptParser ScriptFormatter::_parser=ScriptParser();
ScriptParserHandler ScriptFormatter::_handler=ScriptParserHandler(ScriptFormatter::_parser);

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

bool ScriptFormatter::formatValue(const ValueVariable& value, UnicodeString& result, unsigned int nameformat, unsigned int varformat) {
	if (value.getName().length()>0) {
		value.getNameFormatted(result, nameformat);
		UnicodeString temp;
		value.getValueFormatted(temp, varformat);
		result.append('=').append(temp);
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

