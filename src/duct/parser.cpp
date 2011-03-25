/**
@file parser.cpp
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
*/

#include <duct/debug.hpp>
#include <duct/parser.hpp>
#include <duct/charutils.hpp>

namespace duct {

// class Token implementation

Token::Token(int type) : _type(type) {
}

void Token::setType(int type) {
	_type=type;
}

int Token::getType() const {
	return _type;
}

void Token::setLine(int line) {
	_line=line;
}

int Token::getLine() const {
	return _line;
}

void Token::setColumn(int column) {
	_column=column;
}

int Token::getColumn() const {
	return _column;
}

void Token::setPosition(int line, int column) {
	_line=line;
	_column=column;
}

void Token::addChar(UChar32 c) {
	_buffer.addChar(c);
}

void Token::reset(int type) {
	_type=type;
	_buffer.reset();
}

bool Token::compare(UChar32 c) const {
	return _buffer.compare(c);
}

bool Token::compare(const CharacterSet& charset) const {
	return _buffer.compare(charset);
}

bool Token::toString(UnicodeString& str) {
	return _buffer.toString(str);
}

const UnicodeString& Token::toString() {
	return _buffer.toString();
}

int32_t Token::toInt() {
	return _buffer.toInt();
}

bool Token::toInt(int32_t& value) {
	return _buffer.toInt(value);
}

int64_t Token::toLong() {
	return _buffer.toLong();
}

bool Token::toLong(int64_t& value) {
	return _buffer.toLong(value);
}

float Token::toFloat() {
	return _buffer.toFloat();
}

bool Token::toFloat(float& value) {
	return _buffer.toFloat(value);
}

double Token::toDouble() {
	return _buffer.toDouble();
}

bool Token::toDouble(double& value) {
	return _buffer.toDouble(value);
}

// class Parser implementation

Parser::Parser() : _line(1), _column(0), _curchar(-1), _peekchar(-1), _peeked(false), _stream(NULL) {
}

Parser::~Parser() {/* Do nothing */}

int Parser::getLine() const {
	return _line;
}

int Parser::getColumn() const {
	return _column;
}

Token& Parser::getToken() {
	return _token;
}

const Token& Parser::getToken() const {
	return _token;
}

Stream* Parser::getStream() {
	return _stream;
}

bool Parser::initWithStream(Stream* stream) {
	reset();
	if (!stream) {
		return false;
	}
	_stream=stream;
	nextChar(); // get the first character
	return true;
}

void Parser::reset() {
	_token.reset(NULL_TOKEN);
	_line=1;
	_column=0;
	_stream=NULL;
	_curchar=CHAR_EOF;
	_peekchar=CHAR_EOF;
	_peeked=false;
}

UChar32 Parser::nextChar() {
	if (_curchar==CHAR_NEWLINE) {
		_line++;
		_column=0;
	}
	if (_peeked) {
		_curchar=_peekchar;
		_peeked=false;
	} else if (!_stream->eof()) {
		_curchar=_stream->readChar();
	} else {
		_curchar=CHAR_EOF;
	}
	if (_curchar==CHAR_CARRIAGERETURN) {
		nextChar();
	} else if (_curchar!=CHAR_EOF) {
		_column++;
	}
	return _curchar;
}

UChar32 Parser::peekChar() {
	if (!_peeked) {
		if (!_stream->eof())
			_peekchar=_stream->readChar();
		else
			_peekchar=CHAR_EOF;
		_peeked=true;
	}
	return _peekchar;
}

bool Parser::skipToChar(UChar32 c) {
	while (_curchar!=CHAR_EOF && _curchar!=c)
		nextChar();
	return _curchar==c;
}

bool Parser::skipToEOL() {
	while (_curchar!=CHAR_EOF && _curchar!=CHAR_NEWLINE)
		nextChar();
	return _curchar==CHAR_NEWLINE;
}

// class ParserHandler implementation

ParserHandler::~ParserHandler() {/* Do nothing */}

} // namespace duct

