/**
@file variables.cpp
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
#include <duct/charutils.hpp>
#include <duct/variables.hpp>

#include <algorithm>
#include <unicode/numfmt.h>

namespace duct {

const UChar __uchar_1[]={'1', '\0'};
const UChar __uchar_0[]={'0', '\0'};
const UChar __uchar_true[]={'t', 'r', 'u', 'e', '\0'};
const UChar __uchar_false[]={'f', 'a', 'l', 's', 'e', '\0'};

const UChar __uchar_quotedempty[]={'\"', '\"', '\0'};

// class Variable implementation

Variable::~Variable() { /* Do nothing */ }

void Variable::setName(const icu::UnicodeString& name) {
	_name.setTo(name);
}

const icu::UnicodeString& Variable::getName() const {
	return _name;
}

void Variable::getNameFormatted(icu::UnicodeString& result, unsigned int format) const {
	icu::UnicodeString temp;
	if (format&FMT_VALUE_QUOTE_ALWAYS) {
		temp.setTo('\"'+_name+'\"');
	} else if (_name.isEmpty() && format&FMT_STRING_QUOTE_EMPTY) {
		temp.setTo("\"\"");
	} else if (format&FMT_STRING_QUOTE_WHITESPACE && (_name.indexOf('\t')>-1 || _name.indexOf(' ')>-1 || _name.indexOf('\n')>-1)) {
		temp.setTo('\"'+_name+'\"');
	} else if ((format&FMT_STRING_QUOTE_CONTROL)!=0 && (_name.indexOf(CHAR_OPENBRACE)>-1 || _name.indexOf(CHAR_CLOSEBRACE)>-1 || _name.indexOf(CHAR_EQUALSIGN)>-1)) {
		temp.setTo('\"'+_name+'\"');
	} else {
		temp.setTo(_name);
	}
	CharUtils::escapeString(result, temp, format);
}

void Variable::setParent(CollectionVariable* parent) {
	_parent=parent;
}

CollectionVariable* Variable::getParent() const {
	return _parent;
}

signed int Variable::variableToBool(Variable* source) {
	if (source->getType()==VARTYPE_BOOL) {
		return ((BoolVariable*)source)->get();
	} else if (source->getType()==VARTYPE_STRING) {
		const icu::UnicodeString& str=((StringVariable*)source)->get();
		if (str.caseCompare(__uchar_true, 4, U_FOLD_CASE_DEFAULT)==0 || str.compare(__uchar_1, 1)==0) {
			return 1;
		} else if (str.caseCompare(__uchar_false, 5, U_FOLD_CASE_DEFAULT)==0 || str.compare(__uchar_0, 1)==0) {
			return 0;
		}
	} else if (source->getType()==VARTYPE_INTEGER) {
		int value=((IntVariable*)source)->get();
		if (value==1) {
			return 1;
		} else if (value==0) {
			return 0;
		}
	}
	return -1;
}

signed int Variable::stringToBool(const icu::UnicodeString& source) {
	if (source.caseCompare(__uchar_true, 4, U_FOLD_CASE_DEFAULT)==0 || source.compare(__uchar_1, 1)==0) {
		return 1;
	} else if (source.caseCompare(__uchar_false, 5, U_FOLD_CASE_DEFAULT)==0 || source.compare(__uchar_0, 1)==0) {
		return 0;
	}
	return -1;
}

ValueVariable* Variable::stringToValue(const icu::UnicodeString& source, const icu::UnicodeString& varname, unsigned int type) {
	if (source.isEmpty()) {
		return new StringVariable(source, varname, NULL);
	}
	if (type==VARTYPE_NONE) {
		for (int i=0; i<source.length(); ++i) {
			UChar c=source[i];
			if ((c>='0' && c<='9') || c=='+' || c=='-') {
				if (type==VARTYPE_NONE) { // Leave float and string alone
					type=VARTYPE_INTEGER; // Integer so far..
				}
			} else if (c=='.') {
				if (type==VARTYPE_INTEGER || type==VARTYPE_NONE) {
					type=VARTYPE_FLOAT;
				} else if (type==VARTYPE_FLOAT) {
					type=VARTYPE_STRING; // Float cannot have more than one decimal point, so the source must be a string
					break;
				}
			} else { // If the character is not numerical there is nothing else to deduce and the value is a string
				type=VARTYPE_STRING;
				break;
			}
		}
	}
	ValueVariable* var;
	switch (type) {
		case VARTYPE_INTEGER:
			var=new IntVariable(0, varname);
			break;
		case VARTYPE_FLOAT:
			var=new FloatVariable(0.0, varname, NULL);
			break;
		case VARTYPE_BOOL:
			var=new BoolVariable(false, varname, NULL);
			break;
		default: // NOTE: VARTYPE_STRING results the same as an unrecognized variable type
			int b=stringToBool(source);
			if (b>-1) {
				return new BoolVariable(b==1, varname, NULL);
			} else {
				return new StringVariable(source, varname, NULL);
			}
	}
	var->setFromString(source);
	return var;
}

ValueVariable* Variable::stringToValue(const icu::UnicodeString& source, unsigned int type) {
	icu::UnicodeString varname;
	return stringToValue(source, varname, type);
}

// class ValueVariable implementation

ValueVariable::~ValueVariable() { /* Do nothing */ }

// class IntVariable implementation

IntVariable::IntVariable(int value, CollectionVariable* parent) {
	set(value);
	setParent(parent);
}

IntVariable::IntVariable(int value, const icu::UnicodeString& name, CollectionVariable* parent) {
	set(value);
	setName(name);
	setParent(parent);
}

void IntVariable::set(int value) {
	_value=value;
}

int IntVariable::get() const {
	return _value;
}

void IntVariable::setFromString(const icu::UnicodeString& source) {
	UErrorCode status=U_ZERO_ERROR;
	icu::NumberFormat* nf=icu::NumberFormat::createInstance(status);
	icu::Formattable formattable;
	nf->parse(source, formattable, status);
	if (U_FAILURE(status)) {
		debug_printp_source(this, u_errorName(status));
		_value=0;
	} else {
		_value=formattable.getLong();
	}
	delete nf;
}

void IntVariable::getValueFormatted(icu::UnicodeString& result, unsigned int format) const {
	if (format&FMT_VALUE_QUOTE_ALWAYS) {
		result.setTo('\"');
		valueAsString(result, true);
		result+='\"';
	} else {
		valueAsString(result, false);
	}
}

void IntVariable::valueAsString(icu::UnicodeString& result, bool append) const {
	UErrorCode status=U_ZERO_ERROR;
	icu::NumberFormat* nf=icu::NumberFormat::createInstance(status);
	nf->setGroupingUsed(false);
	nf->setParseIntegerOnly(true);
	if (!append)
		result.remove();
	nf->format(_value, result);
	delete nf;
}

unsigned int IntVariable::getType() const {
	return VARTYPE_INTEGER;
}

const char* IntVariable::getTypeName() const {
	return "int";
}

Variable* IntVariable::clone() const {
	IntVariable* x=new IntVariable(_value, _name, NULL);
	return x;
}

// class StringVariable implementation

StringVariable::StringVariable(CollectionVariable* parent) {
	setParent(parent);
}

StringVariable::StringVariable(const icu::UnicodeString& value, CollectionVariable* parent) {
	set(value);
	setParent(parent);
}

StringVariable::StringVariable(const icu::UnicodeString& value, const icu::UnicodeString& name, CollectionVariable* parent) {
	set(value);
	setName(name);
	setParent(parent);
}

bool StringVariable::isNumeric(bool allowdecimal) const {
	UChar c;
	bool result=false, decimal=false;
	for (int i=0; i<_value.length(); ++i) {
		c=_value[i];
		if (c=='.') {
			if (allowdecimal) {
				if (decimal) { // already got decimal
					result=false;
					break;
				} else { // first decimal
					result=true;
					decimal=true;
				}
			} else {
				result=false;
				break;
			}
		} else if (c>='0' && c<='9') {
			result=true;
		} else if ((c=='-' || c=='+') && i>0) {
			if (i==0) { // sign at beginning
				result=true;
			} else { // already got sign
				result=false;
				break;
			}
		} else {
			result=false;
			break;
		}
	}
	return result;
}

void StringVariable::set(const icu::UnicodeString& value) {
	_value=value;
}

void StringVariable::setFromString(const icu::UnicodeString& source) {
	_name.setTo(source);
}

void StringVariable::getValueFormatted(icu::UnicodeString& result, unsigned int format) const {
	icu::UnicodeString temp;
	if (format&FMT_VALUE_QUOTE_ALWAYS || format&FMT_STRING_QUOTE_ALWAYS) {
		temp.setTo('\"'); temp.append(_value); temp+='\"';
	} else if (format&FMT_STRING_QUOTE_EMPTY && _value.isEmpty()) {
		temp.setTo(__uchar_quotedempty, 2);
	} else if (format&FMT_STRING_QUOTE_WHITESPACE && (_value.indexOf('\t')>-1 || _value.indexOf(' ')>-1 || _value.indexOf('\n')>-1)) {
		temp.setTo('\"'); temp.append(_value); temp+='\"';
	} else if (format&FMT_STRING_SAFE_BOOL && Variable::variableToBool((Variable*)this)!=-1) {
		temp.setTo('\"'); temp.append(_value); temp+='\"';
	} else if (format&FMT_STRING_SAFE_NUMBER && isNumeric(true)) {
		temp.setTo('\"'); temp.append(_value); temp+='\"';
	} else if ((format&FMT_STRING_QUOTE_CONTROL)!=0 && (_value.indexOf(CHAR_OPENBRACE)>-1
		|| _value.indexOf(CHAR_CLOSEBRACE)>-1 || _value.indexOf(CHAR_EQUALSIGN)>-1)) {
		temp.setTo('\"'); temp.append(_value); temp+='\"';
	} else {
		temp.setTo(_value);
	}
	CharUtils::escapeString(result, temp, format);
}

void StringVariable::valueAsString(icu::UnicodeString& result, bool append) const {
	if (append) {
		result+=_value;
	} else {
		result.setTo(_value);
	}
}

const icu::UnicodeString& StringVariable::get() const {
	return _value;
}

unsigned int StringVariable::getType() const {
	return VARTYPE_STRING;
}

const char* StringVariable::getTypeName() const {
	return "string";
}

Variable* StringVariable::clone() const {
	StringVariable* x=new StringVariable(_value, _name, NULL);
	return x;
}

// class FloatVariable implementation

FloatVariable::FloatVariable(float value, CollectionVariable* parent) {
	set(value);
	setParent(parent);
}

FloatVariable::FloatVariable(float value, const icu::UnicodeString& name, CollectionVariable* parent) {
	set(value);
	setName(name);
	setParent(parent);
}

void FloatVariable::set(float value) {
	_value=value;
}

float FloatVariable::get() const {
	return _value;
}

void FloatVariable::setFromString(const icu::UnicodeString& source) {
	UErrorCode status=U_ZERO_ERROR;
	icu::NumberFormat* nf=icu::NumberFormat::createInstance(status);
	icu::Formattable formattable;
	nf->parse(source, formattable, status);
	if (U_FAILURE(status)) {
		debug_printp_source(this, u_errorName(status));
		_value=0.0;
	} else {
		_value=(float)formattable.getDouble(status);
		//printf("FloatVariable::setFromString formattable _value:%f\n", _value);
	}
	delete nf;
}

void FloatVariable::getValueFormatted(icu::UnicodeString& result, unsigned int format) const {
	if (format&FMT_VALUE_QUOTE_ALWAYS) {
		result.setTo('\"');
		valueAsString(result, true);
		result+='\"';
	} else {
		valueAsString(result, false);
	}
}

void FloatVariable::valueAsString(icu::UnicodeString& result, bool append) const {
	UErrorCode status=U_ZERO_ERROR;
	icu::NumberFormat* nf=icu::NumberFormat::createInstance(status);
	nf->setGroupingUsed(false);
	nf->setParseIntegerOnly(false);
	nf->setMinimumFractionDigits(1);
	if (!append)
		result.remove();
	nf->format(_value, result);
	delete nf;
}

unsigned int FloatVariable::getType() const {
	return VARTYPE_FLOAT;
}

const char* FloatVariable::getTypeName() const {
	return "float";
}

Variable* FloatVariable::clone() const {
	FloatVariable* x=new FloatVariable(_value, _name, NULL);
	return x;
}

// class BoolVariable implementation

BoolVariable::BoolVariable(bool value, CollectionVariable* parent) {
	set(value);
	setParent(parent);
}

BoolVariable::BoolVariable(bool value, const icu::UnicodeString& name, CollectionVariable* parent) {
	set(value);
	setName(name);
	setParent(parent);
}

void BoolVariable::set(bool value) {
	_value=value;
}

bool BoolVariable::get() const {
	return _value;
}

void BoolVariable::setFromString(const icu::UnicodeString& source) {
	if (source.caseCompare(__uchar_true, 4, U_FOLD_CASE_DEFAULT)==0 || source.compare(__uchar_1, 1)==0) {
		_value=true;
	//} else if (source.caseCompare(__uchar_false, 5, U_FOLD_CASE_DEFAULT)==0 || source.compare(__uchar_0, 1)==0) {
	//	_value=false;
	} else {
		_value=false;
	}
}

void BoolVariable::getValueFormatted(icu::UnicodeString& result, unsigned int format) const {
	if (format&FMT_BOOL_QUOTE || format&FMT_VALUE_QUOTE_ALWAYS) {
		result.setTo('\"');
		(_value) ? result.append(__uchar_true, 4) : result.append(__uchar_false, 5);
		result.append('\"');
	} else {
		(_value) ? result.setTo(__uchar_true, 4) : result.setTo(__uchar_false, 5);
	}
}

void BoolVariable::valueAsString(icu::UnicodeString& result, bool append) const {
	if (!append) {
		result.remove(); // Clear the string
	}
	(_value) ? result.append(__uchar_true, 4) : result.append(__uchar_false, 5);
}

unsigned int BoolVariable::getType() const {
	return VARTYPE_BOOL;
}

const char* BoolVariable::getTypeName() const {
	return "bool";
}

Variable* BoolVariable::clone() const {
	BoolVariable* x=new BoolVariable(_value, _name, NULL);
	return x;
}

// class CollectionVariable implementation

CollectionVariable::~CollectionVariable() {
	clear();
}

VarList& CollectionVariable::getChildren() {
	return _children;
}

const VarList& CollectionVariable::getChildren() const {
	return _children;
}

size_t CollectionVariable::getChildCount() const {
	return _children.size();
}

VarList::iterator CollectionVariable::begin() {
	return _children.begin();
}

VarList::const_iterator CollectionVariable::begin() const {
	return _children.begin();
}

VarList::iterator CollectionVariable::end() {
	return _children.end();
}

VarList::const_iterator CollectionVariable::end() const {
	return _children.end();
}

VarList::iterator CollectionVariable::find(const Variable* variable) {
	return find(variable, begin());
}

VarList::const_iterator CollectionVariable::find(const Variable* variable) const {
	return find(variable, begin());
}

VarList::iterator CollectionVariable::find(const Variable* variable, VarList::iterator iter) {
	VarList::iterator ei=end();
	while (iter!=ei) {
		if ((*iter)==variable) {
			return iter;
		}
		++iter;
	}
	return ei;
}

VarList::const_iterator CollectionVariable::find(const Variable* variable, VarList::const_iterator iter) const {
	VarList::const_iterator ei=end();
	while (iter!=ei) {
		if ((*iter)==variable) {
			return iter;
		}
		++iter;
	}
	return ei;
}

VarList::iterator CollectionVariable::findAt(int index, unsigned int type) {
	if (index<0 || (size_t)index>=_children.size()) {
		return end();
	}
	int i=0;
	VarList::iterator iter=begin();
	for (; iter!=end() && i<index; ++iter) {
		++i;
	}
	if (i==index && (type&(*iter)->getType())) {
		return iter;
	}
	return end();
}

VarList::const_iterator CollectionVariable::findAt(int index, unsigned int type) const {
	if (index<0 || (size_t)index>=_children.size()) {
		return end();
	}
	int i=0;
	VarList::const_iterator iter=begin();
	for (; iter!=end() && i<index; ++iter) {
		++i;
	}
	if (i==index && (type&(*iter)->getType())) {
		return iter;
	}
	return end();
}

VarList::iterator CollectionVariable::findWithType(unsigned int type) {
	return findWithType(type, begin());
}

VarList::const_iterator CollectionVariable::findWithType(unsigned int type) const {
	return findWithType(type, begin());
}

VarList::iterator CollectionVariable::findWithType(unsigned int type, VarList::iterator iter) {
	VarList::iterator ei=end();
	while (iter!=ei) {
		if (type&(*iter)->getType()) {
			return iter;
		}
		++iter;
	}
	return ei;
}

VarList::const_iterator CollectionVariable::findWithType(unsigned int type, VarList::const_iterator iter) const {
	VarList::const_iterator ei=end();
	while (iter!=ei) {
		if (type&(*iter)->getType()) {
			return iter;
		}
		++iter;
	}
	return ei;
}

VarList::iterator CollectionVariable::findWithName(const icu::UnicodeString& name, bool casesens, unsigned int type) {
	return findWithName(name, begin(), casesens, type);
}

VarList::const_iterator CollectionVariable::findWithName(const icu::UnicodeString& name, bool casesens, unsigned int type) const {
	return findWithName(name, begin(), casesens, type);
}

VarList::iterator CollectionVariable::findWithName(const icu::UnicodeString& name, VarList::iterator iter, bool casesens, unsigned int type) {
	VarList::iterator ei=end();
	const Variable* variable;
	while (iter!=ei) {
		variable=*iter;
		if (type&variable->getType()) {
			if (casesens ? (name.compare(variable->getName())==0) : (name.caseCompare(variable->getName(), U_FOLD_CASE_DEFAULT)==0)) {
				return iter;
			}
		}
		++iter;
	}
	return ei;
}

VarList::const_iterator CollectionVariable::findWithName(const icu::UnicodeString& name, VarList::const_iterator iter, bool casesens, unsigned int type) const {
	VarList::const_iterator ei=end();
	const Variable* variable;
	while (iter!=ei) {
		variable=*iter;
		if (type&variable->getType()) {
			if (casesens ? (name.compare(variable->getName())==0) : (name.caseCompare(variable->getName(), U_FOLD_CASE_DEFAULT)==0)) {
				return iter;
			}
		}
		++iter;
	}
	return ei;
}

void CollectionVariable::erase(VarList::iterator position) {
	delete (*position);
	_children.erase(position);
}

void CollectionVariable::clear() {
	//debug_calledp(this);
	for (VarList::iterator iter=begin(); iter!=end(); ++iter) {
		//printf("Deleting %p\n", (void*)(*iter));
		delete (*iter);
	}
	_children.clear();
}

bool CollectionVariable::add(Variable* variable) {
	if (variable) {
		variable->setParent(this);
		_children.push_back(variable);
		return true;
	}
	return false;
}

bool CollectionVariable::insertBefore(int index, Variable* variable) {
	if (variable) {
		VarList::iterator iter=findAt(index, VARTYPE_ANY);
		if (iter!=end()) {
			_children.insert(iter, variable);
			return true;
		}
	}
	return false;
}

bool CollectionVariable::insertBefore(Variable* variable, Variable* target) {
	if (variable) {
		VarList::iterator iter=find(target);
		if (iter!=end()) {
			_children.insert(iter, variable);
			return true;
		}
	}
	return false;
}

bool CollectionVariable::insertAfter(int index, Variable* variable) {
	if (variable) {
		VarList::iterator iter=findAt(index, VARTYPE_ANY);
		if (iter!=end()) {
			iter++; // get the next position, since insert() inserts before, not after
			if (iter!=end()) {
				_children.insert(iter, variable);
			} else {
				_children.push_back(variable);
			}
			return true;
		}
	}
	return false;
}

bool CollectionVariable::insertAfter(Variable* variable, Variable* target) {
	if (variable) {
		VarList::iterator iter=find(target);
		if (iter!=end()) {
			iter++; // get the next position, since insert() inserts before, not after
			if (iter!=end()) {
				_children.insert(iter, variable);
			} else {
				_children.push_back(variable);
			}
			return true;
		}
	}
	return false;
}

bool CollectionVariable::remove(int index, unsigned int type) {
	VarList::iterator iter=findAt(index, type);
	if (iter!=end()) {
		erase(iter);
		return true;
	}
	return false;
}

bool CollectionVariable::remove(const Variable* variable) {
	if (variable) {
		VarList::iterator iter=find(variable);
		if (iter!=end()) {
			erase(iter);
			return true;
		}
	}
	return false;
}

bool CollectionVariable::remove(unsigned int type) {
	VarList::iterator iter=findWithType(type);
	if (iter!=end()) {
		erase(iter);
		return true;
	}
	return false;
}

bool CollectionVariable::remove(const icu::UnicodeString& name, bool casesens, unsigned int type) {
	VarList::iterator iter=findWithName(name, casesens, type);
	if (iter!=end()) {
		erase(iter);
		return true;
	}
	return false;
}

Variable* CollectionVariable::get(const icu::UnicodeString& name, bool casesens, unsigned int type) {
	VarList::iterator iter=findWithName(name, casesens, type);
	if (iter!=end()) {
		return (*iter);
	}
	return NULL;
}

const Variable* CollectionVariable::get(const icu::UnicodeString& name, bool casesens, unsigned int type) const {
	VarList::const_iterator iter=findWithName(name, casesens, type);
	if (iter!=end()) {
		return (*iter);
	}
	return NULL;
}

Variable* CollectionVariable::get(int index, unsigned int type) {
	VarList::iterator iter=findAt(index, type);
	if (iter!=end()) {
		return (*iter);
	}
	return NULL;
}

const Variable* CollectionVariable::get(int index, unsigned int type) const {
	VarList::const_iterator iter=findAt(index, type);
	if (iter!=end()) {
		return (*iter);
	}
	return NULL;
}

IntVariable* CollectionVariable::getInt(int index) {
	Variable* var=get(index, VARTYPE_INTEGER);
	if (var) {
		return (IntVariable*)var;
	}
	return NULL;
}

const IntVariable* CollectionVariable::getInt(int index) const {
	const Variable* var=get(index, VARTYPE_INTEGER);
	if (var) {
		return (IntVariable*)var;
	}
	return NULL;
}

IntVariable* CollectionVariable::getInt(const icu::UnicodeString& name, bool casesens) {
	Variable* var=get(name, casesens, VARTYPE_INTEGER);
	if (var) {
		return (IntVariable*)var;
	}
	return NULL;
}

const IntVariable* CollectionVariable::getInt(const icu::UnicodeString& name, bool casesens) const {
	const Variable* var=get(name, casesens, VARTYPE_INTEGER);
	if (var) {
		return (IntVariable*)var;
	}
	return NULL;
}

bool CollectionVariable::getIntValue(int& result, int index) const {
	const IntVariable* var=getInt(index);
	if (var) {
		result=var->get();
		return true;
	}
	return false;
}

bool CollectionVariable::getIntValue(int& result, const icu::UnicodeString& name, bool casesens) const {
	const IntVariable* var=getInt(name, casesens);
	if (var) {
		result=var->get();
	}
	return false;
}

StringVariable* CollectionVariable::getString(int index) {
	Variable* var=get(index, VARTYPE_STRING);
	if (var) {
		return (StringVariable*)var;
	}
	return NULL;
}

const StringVariable* CollectionVariable::getString(int index) const {
	const Variable* var=get(index, VARTYPE_STRING);
	if (var) {
		return (StringVariable*)var;
	}
	return NULL;
}

StringVariable* CollectionVariable::getString(const icu::UnicodeString& name, bool casesens) {
	Variable* var=get(name, casesens, VARTYPE_STRING);
	if (var) {
		return (StringVariable*)var;
	}
	return NULL;
}

const StringVariable* CollectionVariable::getString(const icu::UnicodeString& name, bool casesens) const {
	const Variable* var=get(name, casesens, VARTYPE_STRING);
	if (var) {
		return (StringVariable*)var;
	}
	return NULL;
}

const icu::UnicodeString* CollectionVariable::getStringValue(int index) const {
	const StringVariable* var=getString(index);
	if (var) {
		return &(var->get());
	}
	return NULL;
}

bool CollectionVariable::getStringValue(icu::UnicodeString& result, int index) const {
	const icu::UnicodeString* ptr=getStringValue(index);
	if (ptr) {
		result.setTo(*ptr);
		return true;
	}
	return false;
}

const icu::UnicodeString* CollectionVariable::getStringValue(const icu::UnicodeString& name, bool casesens) const {
	const StringVariable* var=getString(name, casesens);
	if (var) {
		return &(var->get());
	}
	return NULL;
}

bool CollectionVariable::getStringValue(icu::UnicodeString& result, const icu::UnicodeString& name, bool casesens) const {
	const icu::UnicodeString* ptr=getStringValue(name, casesens);
	if (ptr) {
		result.setTo(*ptr);
		return true;
	}
	return false;
}

FloatVariable* CollectionVariable::getFloat(int index) {
	Variable* var=get(index, VARTYPE_FLOAT);
	if (var) {
		return (FloatVariable*)var;
	}
	return NULL;
}

const FloatVariable* CollectionVariable::getFloat(int index) const {
	const Variable* var=get(index, VARTYPE_FLOAT);
	if (var) {
		return (FloatVariable*)var;
	}
	return NULL;
}

FloatVariable* CollectionVariable::getFloat(const icu::UnicodeString& name, bool casesens) {
	Variable* var=get(name, casesens, VARTYPE_FLOAT);
	if (var) {
		return (FloatVariable*)var;
	}
	return NULL;
}

const FloatVariable* CollectionVariable::getFloat(const icu::UnicodeString& name, bool casesens) const {
	const Variable* var=get(name, casesens, VARTYPE_FLOAT);
	if (var) {
		return (FloatVariable*)var;
	}
	return NULL;
}

bool CollectionVariable::getFloatValue(float& result, int index) const {
	const FloatVariable* var=getFloat(index);
	if (var) {
		result=var->get();
		return true;
	}
	return false;
}

bool CollectionVariable::getFloatValue(float& result, const icu::UnicodeString& name, bool casesens) const {
	const FloatVariable* var=getFloat(name, casesens);
	if (var) {
		result=var->get();
		return true;
	}
	return false;
}

BoolVariable* CollectionVariable::getBool(int index) {
	Variable* var=get(index, VARTYPE_BOOL);
	if (var) {
		return (BoolVariable*)var;
	}
	return NULL;
}

const BoolVariable* CollectionVariable::getBool(int index) const {
	const Variable* var=get(index, VARTYPE_BOOL);
	if (var) {
		return (BoolVariable*)var;
	}
	return NULL;
}

BoolVariable* CollectionVariable::getBool(const icu::UnicodeString& name, bool casesens) {
	const Variable* var=get(name, casesens, VARTYPE_BOOL);
	if (var) {
		return (BoolVariable*)var;
	}
	return NULL;
}

const BoolVariable* CollectionVariable::getBool(const icu::UnicodeString& name, bool casesens) const {
	const Variable* var=get(name, casesens, VARTYPE_BOOL);
	if (var) {
		return (BoolVariable*)var;
	}
	return NULL;
}

bool CollectionVariable::getBoolValue(bool& result, int index) const {
	const BoolVariable* var=getBool(index);
	if (var) {
		result=var->get();
		return true;
	}
	return false;
}

bool CollectionVariable::getBoolValue(bool& result, const icu::UnicodeString& name, bool casesens) {
	const BoolVariable* var=getBool(name, casesens);
	if (var) {
		result=var->get();
		return true;
	}
	return false;	
}

bool CollectionVariable::getAsString(icu::UnicodeString& result, int index, unsigned int type) const {
	const ValueVariable* var=(ValueVariable*)get(index, type);
	if (var) {
		var->valueAsString(result, false);
		return true;
	}
	return false;
}

bool CollectionVariable::getAsString(icu::UnicodeString& result, const icu::UnicodeString& name, bool casesens, unsigned int type) const {
	const ValueVariable* var=(ValueVariable*)get(name, casesens, type);
	if (var) {
		var->valueAsString(result, false);
		return true;
	}
	return false;
}

Identifier* CollectionVariable::getIdentifier(int index) {
	Variable* var=get(index, VARTYPE_IDENTIFIER);
	if (var) {
		return (Identifier*)var;
	}
	return NULL;
}

const Identifier* CollectionVariable::getIdentifier(int index) const {
	const Variable* var=get(index, VARTYPE_IDENTIFIER);
	if (var) {
		return (Identifier*)var;
	}
	return NULL;
}

Identifier* CollectionVariable::getIdentifier(const icu::UnicodeString& name, bool casesens) {
	Variable* var=get(name, casesens, VARTYPE_IDENTIFIER);
	if (var) {
		return (Identifier*)var;
	}
	return NULL;
}

const Identifier* CollectionVariable::getIdentifier(const icu::UnicodeString& name, bool casesens) const {
	const Variable* var=get(name, casesens, VARTYPE_IDENTIFIER);
	if (var) {
		return (Identifier*)var;
	}
	return NULL;
}

Node* CollectionVariable::getNode(int index) {
	Variable* var=get(index, VARTYPE_NODE);
	if (var) {
		return (Node*)var;
	}
	return NULL;
}

const Node* CollectionVariable::getNode(int index) const {
	const Variable* var=get(index, VARTYPE_NODE);
	if (var) {
		return (Node*)var;
	}
	return NULL;
}

Node* CollectionVariable::getNode(const icu::UnicodeString& name, bool casesens) {
	Variable* var=get(name, casesens, VARTYPE_NODE);
	if (var) {
		return (Node*)var;
	}
	return NULL;
}

const Node* CollectionVariable::getNode(const icu::UnicodeString& name, bool casesens) const {
	const Variable* var=get(name, casesens, VARTYPE_NODE);
	if (var) {
		return (Node*)var;
	}
	return NULL;
}

void CollectionVariable::cloneChildren(CollectionVariable& dest) const {
	VarList::const_iterator iter=end();
	for (iter=begin(); iter!=end(); ++iter) {
		dest.add((*iter)->clone());
	}
}

// class Identifier implementation

Identifier::Identifier(CollectionVariable* parent) {
	setParent(parent);
}

Identifier::Identifier(const icu::UnicodeString& name, CollectionVariable* parent) {
	setName(name);
	setParent(parent);
}

unsigned int Identifier::getType() const {
	return VARTYPE_IDENTIFIER;
}

const char* Identifier::getTypeName() const {
	return "identifier";
}

Variable* Identifier::clone() const {
	Identifier* x=new Identifier(_name, NULL);
	cloneChildren(*x);
	return x;
}

// class Node implementation

Node::Node(CollectionVariable* parent) {
	setParent(parent);
}

Node::Node(const icu::UnicodeString& name, CollectionVariable* parent) {
	setName(name);
	setParent(parent);
}

unsigned int Node::getType() const {
	return VARTYPE_NODE;
}

const char* Node::getTypeName() const {
	return "node";
}

Variable* Node::clone() const {
	Node* x=new Node(_name, NULL);
	cloneChildren(*x);
	return x;
}

} // namespace duct

