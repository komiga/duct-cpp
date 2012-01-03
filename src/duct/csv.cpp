/**
@file csv.cpp
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
#include <duct/csv.hpp>
#include <duct/filestream.hpp>
#include <duct/charutils.hpp>

#include <stdlib.h>

namespace duct {

char const* csvm_tokenName__(Token const& token) {
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
	case SeparatorToken:
		return "SeparatorToken";
	case EOFToken:
		return "EOFToken";
	case EOLToken:
		return "EOLToken";
	}
	return "UNKNOWNToken";
}

// class CSVRow implementation

CSVRow::CSVRow(int index)
	: m_index(index)
{/* Do nothing */}

CSVRow::CSVRow()
	: m_index(0)
{/* Do nothing */}

CSVRow::~CSVRow() {
	clear();
}

void CSVRow::setIndex(int index) {
	m_index=index;
}

int CSVRow::getIndex() const {
	return m_index;
}

CSVRecordMap::iterator CSVRow::begin() {
	return m_values.begin();
}

CSVRecordMap::const_iterator CSVRow::begin() const {
	return m_values.begin();
}

CSVRecordMap::iterator CSVRow::end() {
	return m_values.end();
}

CSVRecordMap::const_iterator CSVRow::end() const {
	return m_values.end();
}

CSVRecordMap::reverse_iterator CSVRow::rbegin() {
	return m_values.rbegin();
}

CSVRecordMap::const_reverse_iterator CSVRow::rbegin() const {
	return m_values.rbegin();
}

CSVRecordMap::reverse_iterator CSVRow::rend() {
	return m_values.rend();
}

CSVRecordMap::const_reverse_iterator CSVRow::rend() const {
	return m_values.rend();
}

CSVRecordMap::iterator CSVRow::find(int index) {
	return m_values.find(index);
}

CSVRecordMap::const_iterator CSVRow::find(int index) const {
	return m_values.find(index);
}

void CSVRow::erase(CSVRecordMap::iterator iter) {
	if (iter!=end()) {
		if (iter->second) {
			delete iter->second;
		}
		m_values.erase(iter);
	}
}

void CSVRow::erase(CSVRecordMap::reverse_iterator iter) {
	if (iter!=rend()) {
		if (iter->second) {
			delete iter->second;
		}
		m_values.erase(--(iter.base()));
	}
}

void CSVRow::set(int index, ValueVariable* value) {
	CSVRecordMap::iterator iter=find(index);
	if (iter!=end()) {
		if (iter->second) {
			delete iter->second;
		}
		iter->second=value;
	} else {
		m_values[index]=value;
	}
}

ValueVariable* CSVRow::get(int index) {
	CSVRecordMap::iterator iter=find(index);
	if (iter!=end()) {
		return iter->second;
	}
	return NULL;
}

ValueVariable const* CSVRow::get(int index) const {
	CSVRecordMap::const_iterator iter=find(index);
	if (iter!=end()) {
		return iter->second;
	}
	return NULL;
}

IntVariable* CSVRow::getInt(int index) {
	ValueVariable* v=get(index);
	if (v && (VARTYPE_INTEGER&v->getType())) {
		return (IntVariable*)v;
	}
	return NULL;
}

IntVariable const* CSVRow::getInt(int index) const {
	ValueVariable const* v=get(index);
	if (v && (VARTYPE_INTEGER&v->getType())) {
		return (IntVariable*)v;
	}
	return NULL;
}

bool CSVRow::getIntValue(int& result, int index) const {
	IntVariable const* v=getInt(index);
	if (v) {
		result=v->get();
		return true;
	}
	return false;
}

StringVariable* CSVRow::getString(int index) {
	ValueVariable* v=get(index);
	if (v && (VARTYPE_STRING&v->getType())) {
		return (StringVariable*)v;
	}
	return NULL;
}

StringVariable const* CSVRow::getString(int index) const {
	ValueVariable const* v=get(index);
	if (v && (VARTYPE_STRING&v->getType())) {
		return (StringVariable*)v;
	}
	return NULL;
}

icu::UnicodeString const* CSVRow::getStringValue(int index) const {
	StringVariable const* v=getString(index);
	if (v) {
		return &v->get();
	}
	return NULL;
}

bool CSVRow::getStringValue(icu::UnicodeString& result, int index) const {
	StringVariable const* v=getString(index);
	if (v) {
		result.setTo(v->get());
		return true;
	}
	return false;
}

FloatVariable* CSVRow::getFloat(int index) {
	ValueVariable* v=get(index);
	if (v && (VARTYPE_FLOAT&v->getType())) {
		return (FloatVariable*)v;
	}
	return NULL;
}

FloatVariable const* CSVRow::getFloat(int index) const {
	ValueVariable const* v=get(index);
	if (v && (VARTYPE_FLOAT&v->getType())) {
		return (FloatVariable*)v;
	}
	return NULL;
}

bool CSVRow::getFloatValue(float& result, int index) const {
	FloatVariable const* v=getFloat(index);
	if (v) {
		result=v->get();
		return true;
	}
	return false;
}

BoolVariable* CSVRow::getBool(int index) {
	ValueVariable* v=get(index);
	if (v && (VARTYPE_BOOL&v->getType())) {
		return (BoolVariable*)v;
	}
	return NULL;
}

BoolVariable const* CSVRow::getBool(int index) const {
	ValueVariable const* v=get(index);
	if (v && (VARTYPE_BOOL&v->getType())) {
		return (BoolVariable*)v;
	}
	return NULL;
}

bool CSVRow::getBoolValue(bool& result, int index) const {
	BoolVariable const* v=getBool(index);
	if (v) {
		result=v->get();
		return true;
	}
	return false;
}

bool CSVRow::getAsString(icu::UnicodeString& result, int index) const {
	ValueVariable const* v=get(index);
	if (v) {
		v->valueAsString(result, false);
		return true;
	}
	return false;
}

bool CSVRow::remove(int index) {
	CSVRecordMap::iterator iter=find(index);
	if (iter!=end()) {
		if (iter->second) {
			delete iter->second;
		}
		m_values.erase(iter);
		return true;
	}
	return false;
}

void CSVRow::clear() {
	CSVRecordMap::iterator iter=begin();
	for (; iter!=end(); ++iter) {
		if (iter->second) {
			delete iter->second;
		}
	}
	m_values.clear();
}

bool CSVRow::has(int index) const {
	CSVRecordMap::const_iterator iter=find(index);
	if (iter!=end()) {
		return true;
	}
	return false;
}

size_t CSVRow::getCount(bool nulls) const {
	if (nulls) {
		return m_values.size();
	} else {
		size_t size=0;
		CSVRecordMap::const_iterator iter;
		CSVRecordMap::const_iterator iend=end();
		for (iter=begin(); iter!=iend; ++iter) {
			if (iter->second) {
				size++;
			}
		}
		return size;
	}
}

size_t CSVRow::inRange(int start, int end, bool nulls) const {
	size_t size=0;
	int index;
	CSVRecordMap::const_iterator iter;
	CSVRecordMap::const_iterator iend=this->end();
	for (index=start; index<=end; ++index) {
		iter=find(index);
		if (iter!=iend && (iter->second || nulls)) {
			size++;
		}
	}
	return size;
}

// class CSVMap

CSVMap::CSVMap()
{/* Do nothing */}

CSVMap::~CSVMap() {
	clear();
}

size_t CSVMap::getRowCount() const {
	return m_rows.size();
}

size_t CSVMap::getHeaderCount(int index, bool nulls) const {
	CSVRowMap::const_iterator iter=find(index);
	if (iter!=end()) {
		return iter->second->getCount(nulls);
	}
	return 0;
}

size_t CSVMap::getValueCount() const {
	size_t size=0;
	CSVRowMap::const_iterator iter=begin();
	for (; iter!=end(); ++iter) {
		size+=iter->second->getCount();
	}
	return size;
}

CSVRowMap::iterator CSVMap::begin() {
	return m_rows.begin();
}

CSVRowMap::const_iterator CSVMap::begin() const {
	return m_rows.begin();
}

CSVRowMap::iterator CSVMap::end() {
	return m_rows.end();
}

CSVRowMap::const_iterator CSVMap::end() const {
	return m_rows.end();
}

CSVRowMap::reverse_iterator CSVMap::rbegin() {
	return m_rows.rbegin();
}

CSVRowMap::const_reverse_iterator CSVMap::rbegin() const {
	return m_rows.rbegin();
}

CSVRowMap::reverse_iterator CSVMap::rend() {
	return m_rows.rend();
}

CSVRowMap::const_reverse_iterator CSVMap::rend() const {
	return m_rows.rend();
}

CSVRowMap::iterator CSVMap::find(int index) {
	return m_rows.find(index);
}

CSVRowMap::const_iterator CSVMap::find(int index) const {
	return m_rows.find(index);
}

void CSVMap::erase(CSVRowMap::iterator iter) {
	if (iter!=end()) {
		delete iter->second;
		m_rows.erase(iter);
	}
}

void CSVMap::erase(CSVRowMap::reverse_iterator iter) {
	if (iter!=rend()) {
		delete iter->second;
		m_rows.erase(--(iter.base()));
	}
}

bool CSVMap::set(CSVRow* row) {
	return set(row->getIndex(), row);
}

bool CSVMap::set(int index, CSVRow* row) {
	if (row) {
		CSVRowMap::iterator iter=find(index);
		if (iter!=end()) {
			delete iter->second;
			iter->second=row;
		} else {
			m_rows[index]=row;
		}
		row->setIndex(index);
		return true;
	}
	return false;
}

CSVRow* CSVMap::get(int index) {
	CSVRowMap::iterator iter=find(index);
	if (iter!=end()) {
		return iter->second;
	}
	return NULL;
}

CSVRow const* CSVMap::get(int index) const {
	CSVRowMap::const_iterator iter=find(index);
	if (iter!=end()) {
		return iter->second;
	}
	return NULL;
}

bool CSVMap::moveRow(int src, int dest, bool swap) {
	if (src==dest) {
		return true;
	}
	CSVRowMap::iterator si=find(src);
	if (si!=end()) {
		CSVRow* srow=si->second;
		if (swap) {
			CSVRowMap::iterator di=find(dest);
			if (di!=end()) {
				si->second=di->second;
				si->second->setIndex(src);
				di->second=srow;
				srow->setIndex(dest);
				return true;
			}
		}
		m_rows.erase(si);
		return set(dest, srow);
	}
	return false;
}

bool CSVMap::remove(int index) {
	CSVRowMap::iterator iter=find(index);
	if (iter!=end()) {
		delete iter->second;
		m_rows.erase(iter);
		return true;
	}
	return false;
}

bool CSVMap::has(int row) const {
	CSVRowMap::const_iterator iter=find(row);
	if (iter!=end()) {
		return true;
	}
	return false;
}

void CSVMap::clear() {
	CSVRowMap::iterator iter;
	for (iter=begin(); iter!=end(); ++iter) {
		delete iter->second;
	}
	m_rows.clear();
}

bool CSVMap::setValue(int row, int column, ValueVariable* value, bool autocreate) {
	CSVRow* r=get(row);
	if (!r && autocreate) {
		r=new CSVRow();
		set(row, r);
	}
	if (r) {
		r->set(column, value);
		return true;
	}
	return false;
}

ValueVariable* CSVMap::getValue(int row, int column) {
	CSVRow* r=get(row);
	if (r) {
		return r->get(column);
	}
	return NULL;
}

ValueVariable const* CSVMap::getValue(int row, int column) const {
	CSVRow const* r=get(row);
	if (r) {
		return r->get(column);
	}
	return NULL;
}

IntVariable* CSVMap::getInt(int row, int column) {
	CSVRow* r=get(row);
	if (r) {
		return r->getInt(column);
	}
	return NULL;
}

IntVariable const* CSVMap::getInt(int row, int column) const {
	CSVRow const* r=get(row);
	if (r) {
		r->getInt(column);
	}
	return NULL;
}

bool CSVMap::getIntValue(int& result, int row, int column) const {
	CSVRow const* r=get(row);
	if (r) {
		return r->getIntValue(result, column);
	}
	return false;
}

StringVariable* CSVMap::getString(int row, int column) {
	CSVRow* r=get(row);
	if (r) {
		return r->getString(column);
	}
	return NULL;
}

StringVariable const* CSVMap::getString(int row, int column) const {
	CSVRow const* r=get(row);
	if (r) {
		return r->getString(column);
	}
	return NULL;
}

icu::UnicodeString const* CSVMap::getStringValue(int row, int column) const {
	CSVRow const* r=get(row);
	if (r) {
		return r->getStringValue(column);
	}
	return NULL;
}

bool CSVMap::getStringValue(icu::UnicodeString& result, int row, int column) const {
	CSVRow const* r=get(row);
	if (r) {
		return r->getStringValue(result, column);
	}
	return false;
}

FloatVariable* CSVMap::getFloat(int row, int column) {
	CSVRow* r=get(row);
	if (r) {
		return r->getFloat(column);
	}
	return NULL;
}

FloatVariable const* CSVMap::getFloat(int row, int column) const {
	CSVRow const* r=get(row);
	if (r) {
		return r->getFloat(column);
	}
	return NULL;
}

bool CSVMap::getFloatValue(float& result, int row, int column) const {
	CSVRow const* r=get(row);
	if (r) {
		return r->getFloatValue(result, column);
	}
	return false;
}

BoolVariable* CSVMap::getBool(int row, int column) {
	CSVRow* r=get(row);
	if (r) {
		return r->getBool(column);
	}
	return NULL;
}

BoolVariable const* CSVMap::getBool(int row, int column) const {
	CSVRow const* r=get(row);
	if (r) {
		return r->getBool(column);
	}
	return NULL;
}

bool CSVMap::getBoolValue(bool& result, int row, int column) const {
	CSVRow const* r=get(row);
	if (r) {
		return r->getBoolValue(result, column);
	}
	return false;
}

bool CSVMap::getAsString(icu::UnicodeString& result, int row, int column) const {
	CSVRow const* r=get(row);
	if (r) {
		return r->getAsString(result, column);
	}
	return false;
}

bool CSVMap::removeValue(int row, int column) {
	CSVRow* r=get(row);
	if (r) {
		return r->remove(column);
	}
	return false;
}

bool CSVMap::hasValue(int row, int column) const {
	CSVRowMap::const_iterator iter=find(row);
	if (iter!=end()) {
		return iter->second->has(column);
	}
	return false;
}

void CSVMap::clearValues() {
	CSVRowMap::iterator iter;
	for (iter=begin(); iter!=end(); ++iter) {
		iter->second->clear();
	}
}

// class CSVParser implementation

CharacterSet CSVParser::s_numberset=CharacterSet("0-9\\-+");
CharacterSet CSVParser::s_numeralset=CharacterSet("0-9");
CharacterSet CSVParser::s_signset=CharacterSet("\\-+");
CharacterSet CSVParser::s_whitespaceset=CharacterSet();

CSVParser::CSVParser()
	: m_handler(NULL)
{
	reset();
}

CSVParser::CSVParser(Stream* stream)
	: m_handler(NULL)
{
	initWithStream(stream);
}

CSVParser::~CSVParser() {/* Do nothing */}

void CSVParser::setSeparator(UChar32 c) {
	m_sepchar=c;
	s_whitespaceset.clear();
	if (m_sepchar!='\t') {
		s_whitespaceset.addRange('\t');
	}
	if (m_sepchar!=' ') {
		s_whitespaceset.addRange(' ');
	}
}

UChar32 CSVParser::getSeparator() const {
	return m_sepchar;
}

void CSVParser::setHandler(ParserHandler* handler) {
	m_handler=(CSVParserHandler*)handler;
}

ParserHandler* CSVParser::getHandler() {
	return m_handler;
}

Token& CSVParser::nextToken() {
	m_token.reset(NULL_TOKEN);
	m_token.setPosition(m_line, m_column);
	switch (m_curchar) {
	case CHAR_QUOTE:
		m_token.setType(QuotedStringToken);
		break;
	case CHAR_EOF:
		m_token.setType(EOFToken);
		break;
	case CHAR_NEWLINE:
		m_token.setType(EOLToken);
		break;
	case CHAR_DECIMALPOINT:
		m_token.setType(DoubleToken);
		m_token.addChar(m_curchar); // add the decimal
		break;
	default:
		if (m_curchar==m_sepchar) {
			m_token.setType(SeparatorToken);
		} else if (s_numberset.contains(m_curchar)) {
			m_token.setType(NumberToken);
			m_token.addChar(m_curchar); // add the number/sign
		} else {
			m_token.setType(StringToken);
		}
		break;
	}
	return m_token;
}

void CSVParser::readToken() {
	switch (m_token.getType()) {
	case QuotedStringToken:
		readQuotedStringToken();
		nextChar();
		break;
	case StringToken:
		readStringToken();
		break;
	case NumberToken:
		nextChar();
		readNumberToken();
		break;
	case DoubleToken:
		nextChar();
		readDoubleToken();
		break;
	case SeparatorToken:
		nextChar();
		break;
	case EOLToken:
		nextChar();
		break;
	case EOFToken:
		// Do nothing
		break;
	default:
		throw CSVParserException(PARSERERROR_PARSER, "CSVParser::readToken", NULL, this, "Unhandled token: %s", csvm_tokenName__(m_token));
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

bool CSVParser::parse() {
	//skipWhitespace();
	nextToken();
	readToken();
	if (m_curchar==CHAR_EOF) {
		m_token.reset(EOFToken);
		m_handler->handleToken(m_token);
		return false;
	} else if (m_token.getType()==EOFToken) {
		return false;
	}
	return true;
}

void CSVParser::readNumberToken() {
	while (m_curchar!=CHAR_EOF) {
		if (m_curchar==CHAR_QUOTE) {
			throw CSVParserException(PARSERERROR_PARSER, "CSVParser::readNumberToken", &m_token, this, "Unexpected quote");
		} else if (m_curchar==CHAR_NEWLINE || m_curchar==m_sepchar) {
			break;
		} else if (s_numeralset.contains(m_curchar)) {
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

void CSVParser::readDoubleToken() {
	while (m_curchar!=CHAR_EOF) {
		if (m_curchar==CHAR_QUOTE) {
			throw CSVParserException(PARSERERROR_PARSER, "CSVParser::readDoubleToken", &m_token, this, "Unexpected quote");
		} else if (m_curchar==CHAR_NEWLINE || m_curchar==m_sepchar) {
			break;
		} else if (s_numeralset.contains(m_curchar)) {
			m_token.addChar(m_curchar);
		} else { // (m_curchar==CHAR_DECIMALPOINT)
			// the token should've already contained a decimal point, so it must be a string.
			m_token.setType(StringToken);
			readStringToken();
			return;
		}
		nextChar();
	}
}

void CSVParser::readStringToken() {
	while (m_curchar!=CHAR_EOF) {
		if (m_curchar==CHAR_QUOTE) {
			if (m_token.compare(s_whitespaceset)) {
				// valid; whitespace before a quoted string is thrown away
				m_token.reset(QuotedStringToken);
				readQuotedStringToken();
				nextChar(); // skip ending quote
				return;
			} else {
				throw CSVParserException(PARSERERROR_PARSER, "CSVParser::readStringToken", &m_token, this, "Unexpected quote");
			}
		} else if (m_curchar==CHAR_BACKSLASH) {
			UChar32 c=CharUtils::getEscapeChar(nextChar());
			if (c!=CHAR_EOF) {
				m_token.addChar(c);
			} else {
				throw CSVParserException(PARSERERROR_PARSER, "CSVParser::readStringToken", &m_token, this, "Unknown escape sequence: %c", m_curchar);
			}
		} else if (m_curchar==m_sepchar || m_curchar==CHAR_NEWLINE) {
			break;
		} else {
			m_token.addChar(m_curchar);
		}
		nextChar();
	}
}

void CSVParser::readQuotedStringToken() {
	bool eolreached=false;
	nextChar(); // skip the first character (it will be the initial quote)
	while (m_curchar!=CHAR_QUOTE) {
		if (m_curchar==CHAR_EOF) {
			throw CSVParserException(PARSERERROR_PARSER, "CSVParser::readQuotedStringToken", &m_token, this, "Encountered EOF whilst reading quoted string");
		} else if (m_curchar==CHAR_BACKSLASH) {
			UChar32 c=CharUtils::getEscapeChar(nextChar());
			if (c!=CHAR_EOF) {
				m_token.addChar(c);
			} else {
				throw CSVParserException(PARSERERROR_PARSER, "CSVParser::readQuotedStringToken", &m_token, this, "Unknown escape sequence: %c", m_curchar);
			}
		} else {
			if (!eolreached) {
				m_token.addChar(m_curchar);
			}
			if (m_curchar==CHAR_NEWLINE) {
				eolreached=true;
			} else if (eolreached && !s_whitespaceset.contains(m_curchar)) {
				eolreached=false;
				m_token.addChar(m_curchar);
			}
		}
		nextChar();
	}
}

// class CSVParserException implementation

CSVParserException::CSVParserException(CSVParserError error, char const* reporter, Token const* token, CSVParser const* parser, char const* fmt, ...)
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

char const* CSVParserException::what() const throw() {
	return m_message;
}

char const* CSVParserException::errorToString(CSVParserError error) {
	switch (error) {
		case PARSERERROR_PARSER:
			return "ERROR_PARSER";
		case PARSERERROR_MEMALLOC:
			return "ERROR_MEMALLOC";
		default:
			return "ERROR_UNKNOWN";
	}
}

// class CSVParserHandler implementation

CSVParserHandler::CSVParserHandler(CSVParser& parser)
	: m_parser(parser), m_map(NULL), m_currentrow(NULL), m_strow(0), m_row(0), m_column(0), m_gtoken(false)
{
	m_parser.setHandler(this);
}

void CSVParserHandler::setParser(Parser& parser) {
	m_parser=(CSVParser&)parser;
	m_parser.setHandler(this);
}

Parser& CSVParserHandler::getParser() {
	return m_parser;
}

void CSVParserHandler::throwex(CSVParserException e) {
	freeData();
	throw e;
}

void CSVParserHandler::clean() {
	m_currentrow=NULL;
	m_row=m_strow;
	m_column=0;
	m_gtoken=false;
}

bool CSVParserHandler::process() {
	m_map=new CSVMap();
	m_currentrow=new CSVRow();
	while (m_parser.parse()) {}
	finish();
	return true;
}

void CSVParserHandler::handleToken(Token& token) {
	switch (token.getType()) {
	case StringToken: {
		if (m_gtoken) {
			if (token.compare(CSVParser::s_whitespaceset)) {
				return; // ignore whitespace
			} else {
				throwex(CSVParserException(PARSERERROR_PARSER, "CSVParserHandler::handleToken", &token, &m_parser,
					"Unexpected non-whitespace StringToken"));
			}
		}
		icu::UnicodeString const& str=token.toString();
		int bv=Variable::stringToBool(str);
		if (bv!=-1) {
			addToRow(new BoolVariable(bv==1 ? true : false));
		} else {
			addToRow(new StringVariable(str));
		}
		}
		break;
	case QuotedStringToken:
		if (m_gtoken) {
			throwex(CSVParserException(PARSERERROR_PARSER, "CSVParserHandler::handleToken", &token, &m_parser,
				"Unexpected QuotedStringToken"));
		}
		addToRow(new StringVariable(token.toString()));
		break;
	case NumberToken:
		if (m_gtoken) {
			throwex(CSVParserException(PARSERERROR_PARSER, "CSVParserHandler::handleToken", &token, &m_parser,
				"Unexpected NumberToken"));
		}
		addToRow(new IntVariable(token.toInt()));
		break;
	case DoubleToken:
		if (m_gtoken) {
			throwex(CSVParserException(PARSERERROR_PARSER, "CSVParserHandler::handleToken", &token, &m_parser,
				"Unexpected DoubleToken"));
		}
		addToRow(new FloatVariable(token.toFloat()));
		break;
	case SeparatorToken:
		if (!m_gtoken) {
			addToRow(NULL);
		}
		m_gtoken=false;
		m_column++;
		break;
	case EOLToken:
		newRow();
		break;
	case EOFToken:
		break;
	}
}

void CSVParserHandler::finish() {
}

CSVMap* CSVParserHandler::processFromStream(Stream* stream) {
	m_parser.initWithStream(stream);
	clean(); // make sure the row index is reset
	process();
	CSVMap* map=m_map; // store before cleaning
	clean();
	m_parser.reset();
	return map;
}

void CSVParserHandler::setup(UChar32 sepchar, unsigned int headercount) {
	m_parser.setSeparator(sepchar);
	m_strow=-(int)headercount;
}

void CSVParserHandler::freeData() {
	if (m_currentrow) {
		delete m_currentrow;
	}
	if (m_map) {
		delete m_map;
	}
	clean();
}

void CSVParserHandler::addToRow(ValueVariable* val) {
	if (m_currentrow==NULL) {
		newRow();
	}
	m_currentrow->set(m_column, val);
	m_gtoken=(val!=NULL);
}

void CSVParserHandler::newRow() {
	if (m_currentrow!=NULL) {
		if (!m_gtoken) {
			addToRow(NULL);
		}
		m_map->set(m_row++, m_currentrow);
	}
	m_currentrow=new CSVRow(m_row);
	m_column=0;
	m_gtoken=false;
}

// class CSVFormatter implementation

CSVParser CSVFormatter::s_parser=CSVParser();
CSVParserHandler CSVFormatter::s_handler=CSVParserHandler(CSVFormatter::s_parser);

void CSVFormatter::formatRow(CSVRow const& row, icu::UnicodeString& result, UChar32 sepchar, unsigned int varformat) {
	result.remove();
	icu::UnicodeString formatted;
	int lastcolumn=0;
	CSVRecordMap::const_iterator iter;
	for (iter=row.begin(); iter!=row.end(); ++iter) {
		for (; lastcolumn<iter->first; ++lastcolumn) {
			result.append(sepchar);
		}
		if (iter->second) {
			iter->second->getValueFormatted(formatted, varformat);
		//	result.append(iter->second->getTypeName());
		//	result.append(':');
			result.append(formatted);
		//} else {
		//	result.append("[null]");
		}
	}
}

CSVMap* CSVFormatter::loadFromFile(char const* path, UChar32 sepchar, unsigned int headercount, char const* encoding) {
	Stream* stream=FileStream::readFile(path, encoding);
	if (stream) {
		s_handler.setup(sepchar, headercount);
		CSVMap* map=s_handler.processFromStream(stream);
		stream->close();
		delete stream;
		return map;
	}
	return NULL;
}

CSVMap* CSVFormatter::loadFromFile(std::string const& path, UChar32 sepchar, unsigned int headercount, char const* encoding) {
	return loadFromFile(path.c_str(), sepchar, headercount, encoding);
}

CSVMap* CSVFormatter::loadFromFile(icu::UnicodeString const& path, UChar32 sepchar, unsigned int headercount, char const* encoding) {
	std::string temp;
	path.toUTF8String(temp);
	return loadFromFile(temp.c_str(), sepchar, headercount, encoding);
}

CSVMap* CSVFormatter::loadFromStream(Stream* stream, UChar32 sepchar, unsigned int headercount) {
	if (stream) {
		s_handler.setup(sepchar, headercount);
		return s_handler.processFromStream(stream);
	}
	return NULL;
}

bool CSVFormatter::writeToFile(CSVMap const* map, char const* path, UChar32 sepchar, char const* encoding, unsigned int varformat) {
	Stream* stream=FileStream::writeFile(path, encoding);
	if (stream) {
		writeToStream(map, stream, sepchar, varformat);
		stream->close();
		return true;
	}
	return false;
}

bool CSVFormatter::writeToFile(CSVMap const* map, std::string const& path, UChar32 sepchar, char const* encoding, unsigned int varformat) {
	return writeToFile(map, path.c_str(), sepchar, encoding, varformat);
}

bool CSVFormatter::writeToFile(CSVMap const* map, icu::UnicodeString const& path, UChar32 sepchar, char const* encoding, unsigned int varformat) {
	std::string temp;
	path.toUTF8String(temp);
	return writeToFile(map, temp.c_str(), sepchar, encoding, varformat);
}

bool CSVFormatter::writeToStream(CSVMap const* map, Stream* stream, UChar32 sepchar, unsigned int varformat) {
	if (map!=NULL && stream!=NULL) {
		icu::UnicodeString temp;
		bool first=false;
		int lastrow=0;
		CSVRowMap::const_iterator iter;
		for (iter=map->begin(); iter!=map->end(); ++iter) {
			if (!first) {
				lastrow=iter->first;
				first=true;
			}
			/*for (; lastrow!=iter->first; ++lastrow) {
				stream->writeChar('\n');
			}*/
			if (iter->second) { // check if record is NULL
				formatRow(*iter->second, temp, sepchar, varformat);
				stream->writeLine(temp);
			}
		}
		return true;
	}
	return false;
}

} // namespace duct

