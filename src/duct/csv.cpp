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

const char* __csv_tokenName(const Token& token) {
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
	
CSVRow::CSVRow(int index) : _index(index) {
}

CSVRow::CSVRow() : _index(0) {
}

CSVRow::~CSVRow() {
	clear();
}

void CSVRow::setIndex(int index) {
	_index=index;
}

int CSVRow::getIndex() const {
	return _index;
}

CSVRecordMap::iterator CSVRow::begin() {
	return _values.begin();
}

CSVRecordMap::const_iterator CSVRow::begin() const {
	return _values.begin();
}

CSVRecordMap::iterator CSVRow::end() {
	return _values.end();
}

CSVRecordMap::const_iterator CSVRow::end() const {
	return _values.end();
}

CSVRecordMap::reverse_iterator CSVRow::rbegin() {
	return _values.rbegin();
}

CSVRecordMap::const_reverse_iterator CSVRow::rbegin() const {
	return _values.rbegin();
}

CSVRecordMap::reverse_iterator CSVRow::rend() {
	return _values.rend();
}

CSVRecordMap::const_reverse_iterator CSVRow::rend() const {
	return _values.rend();
}

CSVRecordMap::iterator CSVRow::find(int index) {
	return _values.find(index);
}

CSVRecordMap::const_iterator CSVRow::find(int index) const {
	return _values.find(index);
}

void CSVRow::erase(CSVRecordMap::iterator iter) {
	if (iter!=end()) {
		if (iter->second) {
			delete iter->second;
		}
		_values.erase(iter);
	}
}

void CSVRow::erase(CSVRecordMap::reverse_iterator iter) {
	if (iter!=rend()) {
		if (iter->second) {
			delete iter->second;
		}
		_values.erase(--(iter.base()));
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
		_values[index]=value;
	}
}

ValueVariable* CSVRow::get(int index) {
	CSVRecordMap::iterator iter=find(index);
	if (iter!=end()) {
		return iter->second;
	}
	return NULL;
}

const ValueVariable* CSVRow::get(int index) const {
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

const IntVariable* CSVRow::getInt(int index) const {
	const ValueVariable* v=get(index);
	if (v && (VARTYPE_INTEGER&v->getType())) {
		return (IntVariable*)v;
	}
	return NULL;
}

bool CSVRow::getIntValue(int& result, int index) const {
	const IntVariable* v=getInt(index);
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

const StringVariable* CSVRow::getString(int index) const {
	const ValueVariable* v=get(index);
	if (v && (VARTYPE_STRING&v->getType())) {
		return (StringVariable*)v;
	}
	return NULL;
}

const icu::UnicodeString* CSVRow::getStringValue(int index) const {
	const StringVariable* v=getString(index);
	if (v) {
		return &v->get();
	}
	return NULL;
}

bool CSVRow::getStringValue(icu::UnicodeString& result, int index) const {
	const StringVariable* v=getString(index);
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

const FloatVariable* CSVRow::getFloat(int index) const {
	const ValueVariable* v=get(index);
	if (v && (VARTYPE_FLOAT&v->getType())) {
		return (FloatVariable*)v;
	}
	return NULL;
}

bool CSVRow::getFloatValue(float& result, int index) const {
	const FloatVariable* v=getFloat(index);
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

const BoolVariable* CSVRow::getBool(int index) const {
	const ValueVariable* v=get(index);
	if (v && (VARTYPE_BOOL&v->getType())) {
		return (BoolVariable*)v;
	}
	return NULL;
}

bool CSVRow::getBoolValue(bool& result, int index) const {
	const BoolVariable* v=getBool(index);
	if (v) {
		result=v->get();
		return true;
	}
	return false;
}

bool CSVRow::getAsString(icu::UnicodeString& result, int index) const {
	const ValueVariable* v=get(index);
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
		_values.erase(iter);
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
	_values.clear();
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
		return _values.size();
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

CSVMap::CSVMap() {
}

CSVMap::~CSVMap() {
	clear();
}

size_t CSVMap::getRowCount() const {
	return _rows.size();
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
	return _rows.begin();
}

CSVRowMap::const_iterator CSVMap::begin() const {
	return _rows.begin();
}

CSVRowMap::iterator CSVMap::end() {
	return _rows.end();
}

CSVRowMap::const_iterator CSVMap::end() const {
	return _rows.end();
}

CSVRowMap::reverse_iterator CSVMap::rbegin() {
	return _rows.rbegin();
}

CSVRowMap::const_reverse_iterator CSVMap::rbegin() const {
	return _rows.rbegin();
}

CSVRowMap::reverse_iterator CSVMap::rend() {
	return _rows.rend();
}

CSVRowMap::const_reverse_iterator CSVMap::rend() const {
	return _rows.rend();
}

CSVRowMap::iterator CSVMap::find(int index) {
	return _rows.find(index);
}

CSVRowMap::const_iterator CSVMap::find(int index) const {
	return _rows.find(index);
}

void CSVMap::erase(CSVRowMap::iterator iter) {
	if (iter!=end()) {
		delete iter->second;
		_rows.erase(iter);
	}
}

void CSVMap::erase(CSVRowMap::reverse_iterator iter) {
	if (iter!=rend()) {
		delete iter->second;
		_rows.erase(--(iter.base()));
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
			_rows[index]=row;
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

const CSVRow* CSVMap::get(int index) const {
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
		_rows.erase(si);
		return set(dest, srow);
	}
	return false;
}

bool CSVMap::remove(int index) {
	CSVRowMap::iterator iter=find(index);
	if (iter!=end()) {
		delete iter->second;
		_rows.erase(iter);
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
	_rows.clear();
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

const ValueVariable* CSVMap::getValue(int row, int column) const {
	const CSVRow* r=get(row);
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

const IntVariable* CSVMap::getInt(int row, int column) const {
	const CSVRow* r=get(row);
	if (r) {
		r->getInt(column);
	}
	return NULL;
}

bool CSVMap::getIntValue(int& result, int row, int column) const {
	const CSVRow* r=get(row);
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

const StringVariable* CSVMap::getString(int row, int column) const {
	const CSVRow* r=get(row);
	if (r) {
		return r->getString(column);
	}
	return NULL;
}

const icu::UnicodeString* CSVMap::getStringValue(int row, int column) const {
	const CSVRow* r=get(row);
	if (r) {
		return r->getStringValue(column);
	}
	return NULL;
}

bool CSVMap::getStringValue(icu::UnicodeString& result, int row, int column) const {
	const CSVRow* r=get(row);
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

const FloatVariable* CSVMap::getFloat(int row, int column) const {
	const CSVRow* r=get(row);
	if (r) {
		return r->getFloat(column);
	}
	return NULL;
}

bool CSVMap::getFloatValue(float& result, int row, int column) const {
	const CSVRow* r=get(row);
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

const BoolVariable* CSVMap::getBool(int row, int column) const {
	const CSVRow* r=get(row);
	if (r) {
		return r->getBool(column);
	}
	return NULL;
}

bool CSVMap::getBoolValue(bool& result, int row, int column) const {
	const CSVRow* r=get(row);
	if (r) {
		return r->getBoolValue(result, column);
	}
	return false;
}

bool CSVMap::getAsString(icu::UnicodeString& result, int row, int column) const {
	const CSVRow* r=get(row);
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

CharacterSet CSVParser::_numberset=CharacterSet("0-9\\-+");
CharacterSet CSVParser::_numeralset=CharacterSet("0-9");
CharacterSet CSVParser::_signset=CharacterSet("\\-+");
CharacterSet CSVParser::_whitespaceset=CharacterSet();

CSVParser::CSVParser() : _handler(NULL) {
	reset();
}

CSVParser::CSVParser(Stream* stream) {
	initWithStream(stream);
}

CSVParser::~CSVParser() {
}

void CSVParser::setSeparator(UChar32 c) {
	_sepchar=c;
	_whitespaceset.clear();
	if (_sepchar!='\t') {
		_whitespaceset.addRange('\t');
	}
	if (_sepchar!=' ') {
		_whitespaceset.addRange(' ');
	}
}

UChar32 CSVParser::getSeparator() const {
	return _sepchar;
}

void CSVParser::setHandler(ParserHandler* handler) {
	_handler=(CSVParserHandler*)handler;
}

ParserHandler* CSVParser::getHandler() {
	return _handler;
}

Token& CSVParser::nextToken() {
	_token.reset(NULL_TOKEN);
	_token.setPosition(_line, _column);
	switch (_curchar) {
		case CHAR_QUOTE:
			_token.setType(QuotedStringToken);
			break;
		case CHAR_EOF:
			_token.setType(EOFToken);
			break;
		case CHAR_NEWLINE:
			_token.setType(EOLToken);
			break;
		case CHAR_DECIMALPOINT:
			_token.setType(DoubleToken);
			_token.addChar(_curchar); // add the decimal
			break;
		default:
			if (_curchar==_sepchar) {
				_token.setType(SeparatorToken);
			} else if (_numberset.contains(_curchar)) {
				_token.setType(NumberToken);
				_token.addChar(_curchar); // add the number/sign
			} else {
				_token.setType(StringToken);
			}
			break;
	}
	return _token;
}

void CSVParser::readToken() {
	switch (_token.getType()) {
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
			throw CSVParserException(PARSERERROR_PARSER, "CSVParser::readToken", NULL, this, "Unhandled token: %s", __csv_tokenName(_token));
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

bool CSVParser::parse() {
	//skipWhitespace();
	nextToken();
	readToken();
	if (_curchar==CHAR_EOF) {
		_token.reset(EOFToken);
		_handler->handleToken(_token);
		return false;
	} else if (_token.getType()==EOFToken) {
		return false;
	}
	return true;
}

void CSVParser::readNumberToken() {
	while (_curchar!=CHAR_EOF) {
		if (_curchar==CHAR_QUOTE) {
			throw CSVParserException(PARSERERROR_PARSER, "CSVParser::readNumberToken", &_token, this, "Unexpected quote");
		} else if (_curchar==CHAR_NEWLINE || _curchar==_sepchar) {
			break;
		} else if (_numeralset.contains(_curchar)) {
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

void CSVParser::readDoubleToken() {
	while (_curchar!=CHAR_EOF) {
		if (_curchar==CHAR_QUOTE) {
			throw CSVParserException(PARSERERROR_PARSER, "CSVParser::readDoubleToken", &_token, this, "Unexpected quote");
		} else if (_curchar==CHAR_NEWLINE || _curchar==_sepchar) {
			break;
		} else if (_numeralset.contains(_curchar)) {
			_token.addChar(_curchar);
		} else { // (_curchar==CHAR_DECIMALPOINT)
			// the token should've already contained a decimal point, so it must be a string.
			_token.setType(StringToken);
			readStringToken();
			return;
		}
		nextChar();
	}
}

void CSVParser::readStringToken() {
	while (_curchar!=CHAR_EOF) {
		if (_curchar==CHAR_QUOTE) {
			if (_token.compare(_whitespaceset)) {
				// valid; whitespace before a quoted string is thrown away
				_token.reset(QuotedStringToken);
				readQuotedStringToken();
				nextChar(); // skip ending quote
				return;
			} else {
				throw CSVParserException(PARSERERROR_PARSER, "CSVParser::readStringToken", &_token, this, "Unexpected quote");
			}
		} else if (_curchar==CHAR_BACKSLASH) {
			UChar32 c=CharUtils::getEscapeChar(nextChar());
			if (c!=CHAR_EOF) {
				_token.addChar(c);
			} else {
				throw CSVParserException(PARSERERROR_PARSER, "CSVParser::readStringToken", &_token, this, "Unknown escape sequence: %c", _curchar);
			}
		} else if (_curchar==_sepchar || _curchar==CHAR_NEWLINE) {
			break;
		} else {
			_token.addChar(_curchar);
		}
		nextChar();
	}
}

void CSVParser::readQuotedStringToken() {
	bool eolreached=false;
	nextChar(); // skip the first character (it will be the initial quote)
	while (_curchar!=CHAR_QUOTE) {
		if (_curchar==CHAR_EOF) {
			throw CSVParserException(PARSERERROR_PARSER, "CSVParser::readQuotedStringToken", &_token, this, "Encountered EOF whilst reading quoted string");
		} else if (_curchar==CHAR_BACKSLASH) {
			UChar32 c=CharUtils::getEscapeChar(nextChar());
			if (c!=CHAR_EOF) {
				_token.addChar(c);
			} else {
				throw CSVParserException(PARSERERROR_PARSER, "CSVParser::readQuotedStringToken", &_token, this, "Unknown escape sequence: %c", _curchar);
			}
		} else {
			if (!eolreached) {
				_token.addChar(_curchar);
			}
			if (_curchar==CHAR_NEWLINE) {
				eolreached=true;
			} else if (eolreached && !_whitespaceset.contains(_curchar)) {
				eolreached=false;
				_token.addChar(_curchar);
			}
		}
		nextChar();
	}
}

// class CSVParserException implementation

CSVParserException::CSVParserException(CSVParserError error, const char* reporter, const Token* token, const CSVParser* parser, const char* fmt, ...) {
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

const char* CSVParserException::what() const throw() {
	return _message;
}

const char* CSVParserException::errorToString(CSVParserError error) {
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

CSVParserHandler::CSVParserHandler(CSVParser& parser) : _parser(parser), _map(NULL), _currentrow(NULL), _strow(0), _row(0), _column(0), _gtoken(false) {
	_parser.setHandler(this);
}

void CSVParserHandler::setParser(Parser& parser) {
	_parser=(CSVParser&)parser;
	_parser.setHandler(this);
}

Parser& CSVParserHandler::getParser() {
	return _parser;
}

void CSVParserHandler::throwex(CSVParserException e) {
	freeData();
	throw e;
}

void CSVParserHandler::clean() {
	_currentrow=NULL;
	_row=_strow;
	_column=0;
	_gtoken=false;
}

bool CSVParserHandler::process() {
	_map=new CSVMap();
	_currentrow=new CSVRow();
	while (_parser.parse()) {
	}
	finish();
	return true;
}

void CSVParserHandler::handleToken(Token& token) {
	switch (token.getType()) {
	case StringToken: {
		if (_gtoken) {
			if (token.compare(CSVParser::_whitespaceset)) {
				return; // ignore whitespace
			} else {
				throwex(CSVParserException(PARSERERROR_PARSER, "CSVParserHandler::handleToken", &token, &_parser,
					"Unexpected non-whitespace StringToken"));
			}
		}
		const icu::UnicodeString& str=token.toString();
		int bv=Variable::stringToBool(str);
		if (bv!=-1) {
			addToRow(new BoolVariable(bv==1 ? true : false));
		} else {
			addToRow(new StringVariable(str));
		}
		}
		break;
	case QuotedStringToken:
		if (_gtoken) {
			throwex(CSVParserException(PARSERERROR_PARSER, "CSVParserHandler::handleToken", &token, &_parser,
				"Unexpected QuotedStringToken"));
		}
		addToRow(new StringVariable(token.toString()));
		break;
	case NumberToken:
		if (_gtoken) {
			throwex(CSVParserException(PARSERERROR_PARSER, "CSVParserHandler::handleToken", &token, &_parser,
				"Unexpected NumberToken"));
		}
		addToRow(new IntVariable(token.toInt()));
		break;
	case DoubleToken:
		if (_gtoken) {
			throwex(CSVParserException(PARSERERROR_PARSER, "CSVParserHandler::handleToken", &token, &_parser,
				"Unexpected DoubleToken"));
		}
		addToRow(new FloatVariable(token.toFloat()));
		break;
	case SeparatorToken:
		if (!_gtoken) {
			addToRow(NULL);
		}
		_gtoken=false;
		_column++;
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
	_parser.initWithStream(stream);
	clean(); // make sure the row index is reset
	process();
	CSVMap* map=_map; // store before cleaning
	clean();
	_parser.reset();
	return map;
}

void CSVParserHandler::setup(UChar32 sepchar, unsigned int headercount) {
	_parser.setSeparator(sepchar);
	_strow=-(int)headercount;
}

void CSVParserHandler::freeData() {
	if (_currentrow) {
		delete _currentrow;
	}
	if (_map) {
		delete _map;
	}
	clean();
}

void CSVParserHandler::addToRow(ValueVariable* val) {
	if (_currentrow==NULL) {
		newRow();
	}
	_currentrow->set(_column, val);
	_gtoken=(val!=NULL);
}

void CSVParserHandler::newRow() {
	if (_currentrow!=NULL) {
		if (!_gtoken) {
			addToRow(NULL);
		}
		_map->set(_row++, _currentrow);
	}
	_currentrow=new CSVRow(_row);
	_column=0;
	_gtoken=false;
}

// class CSVFormatter implementation

CSVParser CSVFormatter::_parser=CSVParser();
CSVParserHandler CSVFormatter::_handler=CSVParserHandler(CSVFormatter::_parser);

void CSVFormatter::formatRow(const CSVRow& row, icu::UnicodeString& result, UChar32 sepchar, unsigned int varformat) {
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

CSVMap* CSVFormatter::loadFromFile(const char* path, UChar32 sepchar, unsigned int headercount, const char* encoding) {
	Stream* stream=FileStream::readFile(path, encoding);
	if (stream) {
		_handler.setup(sepchar, headercount);
		CSVMap* map=_handler.processFromStream(stream);
		stream->close();
		delete stream;
		return map;
	}
	return NULL;
}

CSVMap* CSVFormatter::loadFromFile(const std::string& path, UChar32 sepchar, unsigned int headercount, const char* encoding) {
	return loadFromFile(path.c_str(), sepchar, headercount, encoding);
}

CSVMap* CSVFormatter::loadFromFile(const icu::UnicodeString& path, UChar32 sepchar, unsigned int headercount, const char* encoding) {
	std::string temp;
	path.toUTF8String(temp);
	return loadFromFile(temp.c_str(), sepchar, headercount, encoding);
}

CSVMap* CSVFormatter::loadFromStream(Stream* stream, UChar32 sepchar, unsigned int headercount) {
	if (stream) {
		_handler.setup(sepchar, headercount);
		return _handler.processFromStream(stream);
	}
	return NULL;
}

bool CSVFormatter::writeToFile(const CSVMap* map, const char* path, UChar32 sepchar, const char* encoding, unsigned int varformat) {
	Stream* stream=FileStream::writeFile(path, encoding);
	if (stream) {
		writeToStream(map, stream, sepchar, varformat);
		stream->close();
		return true;
	}
	return false;
}

bool CSVFormatter::writeToFile(const CSVMap* map, const std::string& path, UChar32 sepchar, const char* encoding, unsigned int varformat) {
	return writeToFile(map, path.c_str(), sepchar, encoding, varformat);
}

bool CSVFormatter::writeToFile(const CSVMap* map, const icu::UnicodeString& path, UChar32 sepchar, const char* encoding, unsigned int varformat) {
	std::string temp;
	path.toUTF8String(temp);
	return writeToFile(map, temp.c_str(), sepchar, encoding, varformat);
}

bool CSVFormatter::writeToStream(const CSVMap* map, Stream* stream, UChar32 sepchar, unsigned int varformat) {
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

