/**
@file parser.cpp
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
#include <duct/parser.hpp>
#include <duct/charutils.hpp>

namespace duct {

// class Token implementation

Token::Token(int type)
	: m_type(type), m_line(-1), m_column(-1), m_buffer()
{/* Do nothing */}

Token::~Token() {/* Do nothing */}

void Token::setType(int type) {
	m_type=type;
}

int Token::getType() const {
	return m_type;
}

void Token::setLine(int line) {
	m_line=line;
}

int Token::getLine() const {
	return m_line;
}

void Token::setColumn(int column) {
	m_column=column;
}

int Token::getColumn() const {
	return m_column;
}

void Token::setPosition(int line, int column) {
	m_line=line;
	m_column=column;
}

void Token::addChar(UChar32 c) {
	m_buffer.addChar(c);
}

void Token::reset(int type) {
	m_type=type;
	m_buffer.reset();
}

bool Token::compare(UChar32 c) const {
	return m_buffer.compare(c);
}

bool Token::compare(CharacterSet const& charset) const {
	return m_buffer.compare(charset);
}

bool Token::toString(icu::UnicodeString& str) {
	return m_buffer.toString(str);
}

icu::UnicodeString const& Token::toString() {
	return m_buffer.toString();
}

int32_t Token::toInt() {
	return m_buffer.toInt();
}

bool Token::toInt(int32_t& value) {
	return m_buffer.toInt(value);
}

int64_t Token::toLong() {
	return m_buffer.toLong();
}

bool Token::toLong(int64_t& value) {
	return m_buffer.toLong(value);
}

float Token::toFloat() {
	return m_buffer.toFloat();
}

bool Token::toFloat(float& value) {
	return m_buffer.toFloat(value);
}

double Token::toDouble() {
	return m_buffer.toDouble();
}

bool Token::toDouble(double& value) {
	return m_buffer.toDouble(value);
}

// class Parser implementation

Parser::Parser()
	: m_line(1), m_column(1), m_curchar(-1), m_peekchar(-1), m_peeked(false), m_stream(NULL)
{/* Do nothing */}

Parser::~Parser() {/* Do nothing */}

int Parser::getLine() const {
	return m_line;
}

int Parser::getColumn() const {
	return m_column;
}

Token& Parser::getToken() {
	return m_token;
}

Token const& Parser::getToken() const {
	return m_token;
}

Stream* Parser::getStream() {
	return m_stream;
}

bool Parser::initWithStream(Stream* stream) {
	reset();
	if (!stream) {
		return false;
	}
	m_stream=stream;
	nextChar(); // get the first character
	return true;
}

void Parser::reset() {
	m_token.reset(NULL_TOKEN);
	m_line=1;
	m_column=1;
	m_stream=NULL;
	m_curchar=CHAR_EOF;
	m_peekchar=CHAR_EOF;
	m_peeked=false;
}

UChar32 Parser::nextChar() {
	if (m_curchar==CHAR_NEWLINE) {
		m_line++;
		m_column=1;
	}
	if (m_peeked) {
		m_curchar=m_peekchar;
		m_peeked=false;
	} else if (!m_stream->eof()) {
		m_curchar=m_stream->readChar();
	} else {
		m_curchar=CHAR_EOF;
	}
	if (m_curchar==CHAR_CARRIAGERETURN) {
		nextChar();
	} else if (m_curchar!=CHAR_EOF) {
		m_column++;
	}
	return m_curchar;
}

UChar32 Parser::peekChar() {
	if (!m_peeked) {
		if (!m_stream->eof())
			m_peekchar=m_stream->readChar();
		else
			m_peekchar=CHAR_EOF;
		m_peeked=true;
	}
	return m_peekchar;
}

bool Parser::skipToChar(UChar32 c) {
	while (m_curchar!=CHAR_EOF && m_curchar!=c)
		nextChar();
	return m_curchar==c;
}

bool Parser::skipToEOL() {
	while (m_curchar!=CHAR_EOF && m_curchar!=CHAR_NEWLINE)
		nextChar();
	return m_curchar==CHAR_NEWLINE;
}

// class ParserHandler implementation

ParserHandler::ParserHandler() {/* Do nothing */}

ParserHandler::~ParserHandler() {/* Do nothing */}

} // namespace duct

