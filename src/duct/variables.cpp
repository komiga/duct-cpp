/**
@file variables.cpp
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

duct++ Variables implementation.
*/

#include <duct/debug.hpp>
#include <duct/variables.hpp>
#include <unicode/numfmt.h>
#include <algorithm>

namespace duct {

const UChar __uchar_1[]={'1', '\0'};
const UChar __uchar_0[]={'0', '\0'};
const UChar __uchar_true[]={'t', 'r', 'u', 'e', '\0'};
const UChar __uchar_false[]={'f', 'a', 'l', 's', 'e', '\0'};

// class Variable implementation

Variable::~Variable() { /* Do nothing */ }

void Variable::setName(const UnicodeString& name) {
	_name=name;
}

const UnicodeString& Variable::getName() const {
	return _name;
}

void Variable::getNameFormatted(UnicodeString& result, unsigned int format) const {
	if (format&FMT_VALUE_QUOTE_ALWAYS) {
		result.setTo('\"'+_name+'\"');
	} else if (_name.length()==0 && format&FMT_STRING_QUOTE_EMPTY) {
		result.setTo("\"\"");
	} else if (format&FMT_STRING_QUOTE_WHITESPACE && (_name.indexOf('\t')>-1 || _name.indexOf(' ')>-1 || _name.indexOf('\n')>-1)) {
		result.setTo('\"'+_name+'\"');
	} else {
		result.setTo(_name);
	}
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
		const UnicodeString& str=((StringVariable*)source)->get();
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

signed int Variable::stringToBool(const UnicodeString& source) {
	if (source.caseCompare(__uchar_true, 4, U_FOLD_CASE_DEFAULT)==0 || source.compare(__uchar_1, 1)==0) {
		return 1;
	} else if (source.caseCompare(__uchar_false, 5, U_FOLD_CASE_DEFAULT)==0 || source.compare(__uchar_0, 1)==0) {
		return 0;
	}
	return -1;
}

ValueVariable* Variable::stringToValue(const UnicodeString& source, const UnicodeString& varname, unsigned int type) {
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

ValueVariable* Variable::stringToValue(const UnicodeString& source, unsigned int type) {
	UnicodeString varname;
	return stringToValue(source, varname, type);
}

// class ValueVariable implementation

ValueVariable::~ValueVariable() { /* Do nothing */ }

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

VarList::const_iterator CollectionVariable::begin() const {
	return _children.begin();
}

VarList::const_iterator CollectionVariable::end() const {
	return _children.end();
}

VarList::iterator CollectionVariable::begin() {
	return _children.begin();
}

VarList::iterator CollectionVariable::end() {
	return _children.end();
}

VarList::iterator CollectionVariable::find(Variable* variable) {
	return find(variable, begin());
}

VarList::iterator CollectionVariable::find(Variable* variable, VarList::iterator iter) {
	VarList::iterator ei=end();
	while (iter!=ei) {
		if ((*iter)==variable) {
			return iter;
		}
		++iter;
	}
	return ei;
}

VarList::iterator CollectionVariable::findWithType(unsigned int type) {
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

VarList::iterator CollectionVariable::findWithName(const UnicodeString& name, bool casesens, unsigned int type) {
	return findWithName(name, begin(), casesens, type);
}

VarList::iterator CollectionVariable::findWithName(const UnicodeString& name, VarList::iterator iter, bool casesens, unsigned int type) {
	VarList::iterator ei=end();
	Variable* variable;
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

bool CollectionVariable::addVariable(Variable* variable) {
	if (variable) {
		variable->setParent(this);
		_children.push_back(variable);
		return true;
	}
	return false;
}

bool CollectionVariable::removeVariable(Variable* variable) {
	if (variable) {
		VarList::iterator iter=find(variable);
		if (iter!=end()) {
			erase(iter);
			return true;
		}
	}
	return false;
}

bool CollectionVariable::removeVariableWithType(unsigned int type) {
	VarList::iterator iter=findWithType(type);
	if (iter!=end()) {
		erase(iter);
		return true;
	}
	return false;
}

bool CollectionVariable::removeVariableWithName(const UnicodeString& name, bool casesens, unsigned int type) {
	VarList::iterator iter=findWithName(name, casesens, type);
	if (iter!=end()) {
		erase(iter);
		return true;
	}
	return false;
}

Variable* CollectionVariable::getVariableWithName(const UnicodeString& name, bool casesens, unsigned int type) {
	VarList::iterator iter=findWithName(name, casesens, type);
	if (iter!=end()) {
		return (*iter);
	}
	return NULL;
}

Variable* CollectionVariable::get(int index, unsigned int type) {
	int i=0;
	VarList::iterator iter=begin();
	for (; iter!=end() && i<index; ++iter) {
		++i;
	}
	if (i==index && (type&(*iter)->getType())) {
		return (*iter);
	}
	return NULL;
}

const Variable* CollectionVariable::get(int index, unsigned int type) const {
	int i=0;
	VarList::const_iterator iter=begin();
	for (; iter!=end() && i<index; ++iter) {
		++i;
	}
	if (i==index && (type&(*iter)->getType())) {
		return (*iter);
	}
	return NULL;
}

bool CollectionVariable::getInt(int index, int& result) const {
	const Variable* var=get(index, VARTYPE_INTEGER);
	if (var) {
		result=((IntVariable*)var)->get();
		return true;
	}
	return false;
}

const UnicodeString* CollectionVariable::getString(int index) const {
	const Variable* var=get(index, VARTYPE_STRING);
	if (var) {
		return &(((StringVariable*)var)->get());
	}
	return NULL;
}

bool CollectionVariable::getString(int index, UnicodeString& result) const {
	const UnicodeString* ptr=getString(index);
	if (ptr) {
		result.setTo(*ptr);
		return true;
	}
	return false;
}

bool CollectionVariable::getAsString(int index, UnicodeString& result, unsigned int format) const {
	const ValueVariable* var=(ValueVariable*)get(index, VARTYPE_VALUE);
	if (var) {
		var->getValueFormatted(result, format);
		return true;
	}
	return false;
}

bool CollectionVariable::getFloat(int index, float& result) const {
	const Variable* var=get(index, VARTYPE_FLOAT);
	if (var) {
		result=((FloatVariable*)var)->get();
		return true;
	}
	return false;
}

bool CollectionVariable::getBool(int index, bool& result) const {
	const Variable* var=get(index, VARTYPE_BOOL);
	if (var) {
		result=((BoolVariable*)var)->get();
		return true;
	}
	return false;
}

// class IntVariable implementation

IntVariable::IntVariable(int value, CollectionVariable* parent) {
	set(value);
	setParent(parent);
}

IntVariable::IntVariable(int value, const UnicodeString& name, CollectionVariable* parent) {
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

void IntVariable::setFromString(const UnicodeString& source) {
	UErrorCode status=U_ZERO_ERROR;
	NumberFormat *nf=NumberFormat::createInstance(status);
	Formattable formattable;
	nf->parse(source, formattable, status);
	if (U_FAILURE(status)) {
		debug_printp_source(this, u_errorName(status));
		_value=0;
	} else {
		_value=formattable.getLong();
	}
	delete nf;
}

void IntVariable::getValueFormatted(UnicodeString& result, unsigned int format) const {
	if (format&FMT_VALUE_QUOTE_ALWAYS) {
		result.setTo('\"');
		valueAsString(result, true);
		result+='\"';
	} else {
		valueAsString(result, false);
	}
}

void IntVariable::valueAsString(UnicodeString& result, bool append) const {
	UErrorCode status=U_ZERO_ERROR;
	NumberFormat *nf=NumberFormat::createInstance(status);
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

const char* IntVariable::getTypeAsString() const {
	return "int";
}

// class StringVariable implementation

StringVariable::StringVariable(CollectionVariable* parent) {
	setParent(parent);
}

StringVariable::StringVariable(const UnicodeString& value, CollectionVariable* parent) {
	set(value);
	setParent(parent);
}

StringVariable::StringVariable(const UnicodeString& value, const UnicodeString& name, CollectionVariable* parent) {
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

void StringVariable::set(const UnicodeString& value) {
	_value=value;
}

void StringVariable::setFromString(const UnicodeString& source) {
	_name.setTo(source);
}

void StringVariable::getValueFormatted(UnicodeString& result, unsigned int format) const {
	if (format&FMT_VALUE_QUOTE_ALWAYS) {
		result.setTo('\"'+_value+'\"');
	} else if (format&FMT_STRING_QUOTE_EMPTY && _value.length()==0) {
		result.setTo("\"\"");
	} else if (format&FMT_STRING_QUOTE_WHITESPACE && (_value.indexOf('\t')>-1 || _value.indexOf(' ')>-1 || _value.indexOf('\n')>-1)) {
		result.setTo('\"'+_value+'\"');
	} else if (format&FMT_STRING_SAFE_BOOL && Variable::variableToBool((Variable*)this)!=-1) {
		result.setTo('\"'+_value+'\"');
	} else if (format&FMT_STRING_SAFE_NUMBER && isNumeric(true)) {
		result.setTo('\"'+_value+'\"');
	} else {
		result.setTo(_value);
	}
}

void StringVariable::valueAsString(UnicodeString& result, bool append) const {
	if (append) {
		result+=_value;
	} else {
		result.setTo(_value);
	}
}

const UnicodeString& StringVariable::get() const {
	return _value;
}

unsigned int StringVariable::getType() const {
	return VARTYPE_STRING;
}

const char* StringVariable::getTypeAsString() const {
	return "string";
}

// class FloatVariable implementation

FloatVariable::FloatVariable(float value, CollectionVariable* parent) {
	set(value);
	setParent(parent);
}

FloatVariable::FloatVariable(float value, const UnicodeString& name, CollectionVariable* parent) {
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

void FloatVariable::setFromString(const UnicodeString& source) {
	UErrorCode status=U_ZERO_ERROR;
	NumberFormat *nf=NumberFormat::createInstance(status);
	Formattable formattable;
	nf->parse(source, formattable, status);
	if (U_FAILURE(status)) {
		debug_printp_source(this, u_errorName(status));
		_value=0.0;
	} else {
		_value=formattable.getDouble(status);
		//printf("FloatVariable::setFromString formattable _value:%f\n", _value);
	}
	delete nf;
}

void FloatVariable::getValueFormatted(UnicodeString& result, unsigned int format) const {
	if (format&FMT_FLOAT_TRUNCATE) {
		// TODO
	}
	if (format&FMT_VALUE_QUOTE_ALWAYS) {
		result.setTo('\"');
		valueAsString(result, true);
		result+='\"';
	} else {
		valueAsString(result, false);
	}
}

void FloatVariable::valueAsString(UnicodeString& result, bool append) const {
	UErrorCode status=U_ZERO_ERROR;
	NumberFormat *nf=NumberFormat::createInstance(status);
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

const char* FloatVariable::getTypeAsString() const {
	return "float";
}

// class BoolVariable implementation

BoolVariable::BoolVariable(bool value, CollectionVariable* parent) {
	set(value);
	setParent(parent);
}

BoolVariable::BoolVariable(bool value, const UnicodeString& name, CollectionVariable* parent) {
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

void BoolVariable::setFromString(const UnicodeString& source) {
	/*if (source.caseCompare(__uchar_true, 4, U_FOLD_CASE_DEFAULT)==0) {
		_value=true;
	} else if (source.caseCompare(__uchar_false, 5, U_FOLD_CASE_DEFAULT)==0) {
		_value=false;
	} else {
		UErrorCode status=U_ZERO_ERROR;
		NumberFormat *nf=NumberFormat::createInstance(status);
		Formattable formattable;
		nf->parse(source, formattable, status);
		if (U_FAILURE(status)) {
			debug_printp_source(this, u_errorName(status));
			_value=false;
		} else {
			_value=formattable.getLong()>0;
		}
		delete nf;
	}*/
	if (source.caseCompare(__uchar_true, 4, U_FOLD_CASE_DEFAULT)==0 || source.compare(__uchar_1, 1)==0) {
		_value=true;
	//} else if (source.caseCompare(__uchar_false, 5, U_FOLD_CASE_DEFAULT)==0 || source.compare(__uchar_0, 1)==0) {
	//	_value=false;
	} else {
		_value=false;
	}
}

void BoolVariable::getValueFormatted(UnicodeString& result, unsigned int format) const {
	if (format&FMT_BOOL_STRING) {
		if (format&FMT_VALUE_QUOTE_ALWAYS) {
			if (_value) {
				result.setTo("\"true\"");
			} else {
				result.setTo("\"false\"");
			}
		} else {
			if (_value) {
				result.setTo("true");
			} else {
				result.setTo("false");
			}
		}
	} else if (format&FMT_VALUE_QUOTE_ALWAYS) {
		UChar* buf=result.getBuffer(3); // Get the string's buffer with a minimum capacity of 3 units
		buf[0]='\"';
		buf[1]=(_value) ? '1' : '0';
		buf[2]='\"';
		result.releaseBuffer(3); // Release the open buffer
	} else {
		valueAsString(result, false);
	}
}

void BoolVariable::valueAsString(UnicodeString& result, bool append) const {
	if (!append) {
		result.remove(); // Clear the string
	}
	(_value) ? result+='1' : result+='0';
}

unsigned int BoolVariable::getType() const {
	return VARTYPE_BOOL;
}

const char* BoolVariable::getTypeAsString() const {
	return "bool";
}

// class Identifier implementation

Identifier::Identifier(CollectionVariable* parent) {
	setParent(parent);
}

Identifier::Identifier(const UnicodeString& name, CollectionVariable* parent) {
	setName(name);
	setParent(parent);
}

unsigned int Identifier::getType() const {
	return VARTYPE_IDENTIFIER;
}

const char* Identifier::getTypeAsString() const {
	return "identifier";
}

// class Node implementation

Node::Node(CollectionVariable* parent) {
	setParent(parent);
}

Node::Node(const UnicodeString& name, CollectionVariable* parent) {
	setName(name);
	setParent(parent);
}

unsigned int Node::getType() const {
	return VARTYPE_NODE;
}

const char* Node::getTypeAsString() const {
	return "node";
}

} // namespace duct

