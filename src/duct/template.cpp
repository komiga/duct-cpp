/**
@file template.cpp
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

duct++ Template implementation.
*/

#include <duct/debug.hpp>
#include <duct/template.hpp>
#include <list>

namespace duct {

// class VTypeLayout implementation

VTypeLayout::VTypeLayout() {
	data = NULL;
}

VTypeLayout::VTypeLayout(unsigned int num, ...) {
	count = num;
	data = new unsigned int[count];
	va_list ap;
	va_start(ap, num);
	for (unsigned int i = 0; i < count; ++i) {
		data[i] = va_arg(ap, unsigned int);
	}
	va_end(ap);
}

VTypeLayout::~VTypeLayout() {
	if (data)
		delete[] data;
}

/*unsigned int VTypeLayout::operator[](unsigned int i) {
	return data[i];
}

const unsigned int& VTypeLayout::operator[](unsigned int i) const {
	return data[i];
}*/

// class Identity implementation

Identity::Identity() {
	data = NULL;
	count = 0;
}

Identity::~Identity() {
	if (data) {
		for (unsigned int i = 0; i < count; ++i) {
			delete data[i];
		}
		delete[] data;
	}
}

Identity* Identity::withUStrings(unsigned int num, ...) {
	Identity* iden = new Identity();
	iden->count = num;
	iden->data = new UnicodeString*[num];
	va_list ap;
	va_start(ap, num);
	for (unsigned int i = 0; i < num; ++i) {
		iden->data[i] = new UnicodeString(*va_arg(ap, const UnicodeString*));
	}
	va_end(ap);
	return iden;
}

Identity* Identity::withCStrings(unsigned int num, ...) {
	Identity* iden = new Identity();
	iden->count = num;
	iden->data = new UnicodeString*[num];
	va_list ap;
	va_start(ap, num);
	for (unsigned int i = 0; i < num; ++i) {
		iden->data[i] = new UnicodeString(va_arg(ap, const char*));
	}
	va_end(ap);
	return iden;
}

// class Template implementation

Template::Template() {
	_iden = NULL;
	_layout = NULL;
	_casesens = false;
	_infinitism = VARTYPE_NONE;
}

Template::Template(Identity* iden, VTypeLayout* layout, bool casesens, unsigned int infinitism) {
	_iden = iden;
	_layout = layout;
	_casesens = casesens;
	_infinitism = infinitism;
}

Template::~Template() {
	if (_iden)
		delete _iden;
	if (_layout)
		delete _layout;
}

void Template::setIdentity(Identity* iden) {
	if (_iden)
		delete _iden;
	_iden = iden;
}

const Identity* Template::getIdentity() const {
	return _iden;
}

void Template::setLayout(VTypeLayout* layout) {
	if (_layout)
		delete _layout;
	_layout = layout;
}

const VTypeLayout* Template::getLayout() const {
	return _layout;
}

void Template::setInfinitism(unsigned int infinitism) {
	_infinitism = infinitism;
}

const unsigned int& Template::getInfinitism() const {
	return _infinitism;
}

void Template::setCaseSensitive(bool casesens) {
	_casesens = casesens;
}

bool Template::getCaseSensitive() const {
	return _casesens;
}

int _checkVariable(unsigned int type, const Variable* variable) {
	if (variable && type != VARTYPE_NONE) {
		return type & variable->getType();
	}
	return false;
}

int _checkIden(const Identity* iden, const UnicodeString& a, bool casesens, unsigned int i = 0) {
	//printf("duct::_checkIden iden:%p\n", (void*)iden);
	if (iden) {
		//printf("duct::_checkIden count:%d i:%d\n", iden->count, i);
		const UnicodeString* b;
		for (; i < iden->count; ++i) {
			b = iden->data[i];
			//printf("duct::_checkIden b:%p\n", (void*)b);
			debug_assert(b, "NULL identity element");
			if (casesens ? (a.compare(*b) == 0) : (a.caseCompare(*b, U_FOLD_CASE_DEFAULT) == 0)) {
				return true;
			}
		}
		return false;
	} else {
		return true; // NULL identity matches any name
	}
}

bool _compareNames(const UnicodeString& a, const UnicodeString& b, bool casesens) {
	if (a.length() == 0 && b.length() == 0) {
		return true;
	} else {
		return casesens ? (a.compare(b) == 0) : (a.caseCompare(b, U_FOLD_CASE_DEFAULT) == 0);
	}
}

bool __matchname(const unsigned int& mc, const Identity* iden, const UnicodeString& name, const bool& casesens, const unsigned int& infinitism) {
	if (iden->count == 0) {
		return true;
	} else if (mc < iden->count) {
		return _compareNames(*(iden->data[mc]), name, casesens);
	} else if (mc >= iden->count && (infinitism != VARTYPE_NONE)) {
		return true;
	} else {
		return false;
	}
}

bool __matchvariable(const unsigned int& mc, const VTypeLayout* layout, const unsigned int& infinitism, const Variable* variable) {
	if (layout && (mc < layout->count)) {
		return layout->data[mc] & variable->getType();
	} else if (infinitism != VARTYPE_NONE) {
		return infinitism & variable->getType();
	}
	return false;
}

class _IteratorVariablePair {
public:
	_IteratorVariablePair(VarList::iterator i, Variable* v) : iter(i), variable(v) {
	};
	
	VarList::iterator iter;
	Variable* variable;
};

typedef std::list<_IteratorVariablePair*> _PairList;

void __add(CollectionVariable* collection, _PairList& pairs, const UnicodeString& name, unsigned int& addcount) {
	if (pairs.size() > 0) {
		const _IteratorVariablePair* pair;
		bool first = true;
		Identifier* iden = new Identifier(name, collection);
		for (_PairList::const_iterator iter = pairs.begin(); iter != pairs.end(); ++iter) {
			pair = (_IteratorVariablePair*)(*iter);
			if (first) {
				*(pair->iter) = iden; // replace the iterator's value with the new identifier
				first = false;
			} else {
				collection->getChildren().erase(pair->iter); // erase without deleting the variable
			}
			iden->addVariable(pair->variable); // re-own to the new identifier
		}
		addcount += 1;
	}
}

void __reset(unsigned int& mc, bool& repeatmatch, bool rep, _PairList& pairs) {
	mc = 0;
	repeatmatch = rep;
	pairs.clear();
}

bool Template::validateIdentifier(const Identifier* identifier) const {
	if (identifier) {
		//printf("Template::validateIdentifier _layout:%d _iden:%d _casesens:%d _infinitism:%d\n", _layout != NULL, _iden != NULL, _casesens, _infinitism);
		if (!(identifier->getChildCount() > _layout->count && (_infinitism == VARTYPE_NONE)) && !(identifier->getChildCount() < _layout->count) && _checkIden(_iden, identifier->getName(), _casesens)) {
			// Compare defined variables in the identifier
			VarList::const_iterator iter = identifier->begin();
			for (unsigned int i = 0; i < _layout->count; ++i) {
				if (!(_layout->data[i] & (*iter)->getType())) {
					return false;
				}
				++iter;
			}
			// Check flexible and infinitism
			if (identifier->getChildCount() > _layout->count && _infinitism != VARTYPE_NONE) {
				for (unsigned int i = _layout->count; i < identifier->getChildCount(); ++i) {
					if (!_checkVariable(_infinitism, (*iter))) {
						return false;
					}
				}
				++iter;
			}
			return true; // Identifier passed all tests
		}
	} else {
		//debug_print("identifier is NULL");
	}
	return false;
}

bool Template::validateValue(const ValueVariable* value) const {
	if (value) {
		if (_checkIden(_iden, value->getName(), _casesens)) {
			if (_layout && _layout->count > 0) {
				return _layout->data[0] & value->getType();
			}
			// No canon types, check infinitism
			if (_infinitism != VARTYPE_NONE) {
				return _infinitism & value->getType();
			}
		}
	}
	return false;
}

unsigned int Template::compactCollection(CollectionVariable* collection, const UnicodeString& name, bool sequential) const {
	unsigned int addcount;
	if (collection && collection->getChildCount() > 0) {
		_PairList pairs;
		bool matched; //, namematched, varmatched;
		unsigned int mc = 0;
		unsigned int mmax = _iden ? _iden->count : 0;
		mmax = (_layout ? _layout->count : 0) > mmax ? _layout->count : mmax;
		//printf("(Template::CompactCollection) mmax:%d\n", mmax);
		ValueVariable* value;
		bool repeatmatch = false;
		VarList::iterator iter = collection->begin();
		while (iter != collection->end()) {
			//printf("(Template::CompactCollection) [%d] repeatmatch:%d\n", mc, repeatmatch);
			if (repeatmatch) {
				repeatmatch = false;
			} else {
				value = dynamic_cast<ValueVariable*>(*iter);
				++iter;
			}
			if (value) {
				//namematched = __matchname(mc, _iden, value->getName(), _casesens, _infinitism);
				//varmatched = __matchvariable(mc, _layout, _infinitism, value);
				//matched = namematched && varmatched;
				matched = __matchname(mc, _iden, value->getName(), _casesens, _infinitism) && __matchvariable(mc, _layout, _infinitism, value);
				//printf("(Template::CompactCollection) [%d] type:%s, name:%s, namematched:%d, varmatched:%d, (mc < mmax):%d, (_layout && mc < _layout->count):%d\n",
				//	mc, value->getTypeAsString(), "" /*value->getName()*/, namematched, varmatched, (mc < mmax), (_layout && mc < _layout->count));
				if (matched) {
					//printf("(Template::CompactCollection) match at mc:%d\n", mc);
					pairs.push_back(new _IteratorVariablePair(iter, value));
					mc += 1;
				} else if (mc > 0 && mc != mmax) {
					//printf("(Template::CompactCollection) unmatched before total; mc:%d\n", mc);
					if (mmax == 0 || mc > mmax) {
						//printf("(Template::CompactCollection) creating from left over\n");
						__add(collection, pairs, name, addcount);
					}
					__reset(mc, repeatmatch, true, pairs);
				}
				if (mmax > 0 && ((mc == mmax && (_infinitism == VARTYPE_NONE)) || (mc > 0 && !(iter != collection->end())))) {
					//printf("(Template::CompactCollection) unmatched total mc:%d, creating identifier\n", mc);
					__add(collection, pairs, name, addcount);
					__reset(mc, repeatmatch, ~matched, pairs);
				}
			} else if (mc > 0 && sequential) {
				//printf("(Template::CompactCollection) non-value inbetween match series mc:%d\n", mc);
				if (mmax == 0 || mc > mmax) {
					//printf("(Template::CompactCollection) creating from left over\n");
					__add(collection, pairs, name, addcount);
				}
				__reset(mc, repeatmatch, false, pairs);
			}
		}
	}
	return addcount;
}

unsigned int Template::renameIdentifiers(CollectionVariable* collection, const UnicodeString& name) const {
	unsigned int count;
	if (collection) {
		Identifier* identifier;
		for (VarList::iterator iter = collection->begin(); iter != collection->end(); ++iter) {
			identifier = dynamic_cast<Identifier*>(*iter);
			if (identifier && validateIdentifier(identifier)) {
				identifier->setName(name);
				count += 1;
			}
		}
	}
	return count;
}

unsigned int Template::renameValues(CollectionVariable* collection, const UnicodeString& name) const {
	unsigned int count;
	if (collection) {
		ValueVariable* value;
		for (VarList::iterator iter = collection->begin(); iter != collection->end(); ++iter) {
			value = dynamic_cast<ValueVariable*>(*iter);
			if (value && validateValue(value)) {
				value->setName(name);
				count += 1;
			}
		}
	}
	return count;
}

Identifier* Template::getMatchingIdentifier(const CollectionVariable* collection) const {
	if (collection) {
		Identifier* iden;
		for (VarList::const_iterator iter = collection->begin(); iter != collection->end(); ++iter) {
			iden = dynamic_cast<Identifier*>(*iter);
			if (iden && validateIdentifier(iden)) {
				return iden;
			}
		}
	}
	return NULL;
}

ValueVariable* Template::getMatchingValue(const CollectionVariable* collection) const {
	if (collection) {
		ValueVariable* value;
		for (VarList::const_iterator iter = collection->begin(); iter != collection->end(); ++iter) {
			value = dynamic_cast<ValueVariable*>(*iter);
			if (value && validateValue(value)) {
				return value;
			}
		}
	}
	return NULL;
}

} // namespace duct

