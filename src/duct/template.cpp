/**
@file template.cpp
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

#include <list>
#include <duct/debug.hpp>
#include <duct/template.hpp>

namespace duct {

// class Template implementation

Template::Template()
	: _iden(NULL), _layout(NULL), _casesens(false), _infinitism(VARTYPE_NONE) {
}

Template::Template(StringArray* iden, VTypeLayout* layout, bool casesens, unsigned int infinitism)
	: _iden(iden), _layout(layout), _casesens(casesens), _infinitism(infinitism) {
}

Template::~Template() {
	if (_iden) {
		delete _iden;
		_iden=NULL;
	}
	if (_layout) {
		delete _layout;
		_layout=NULL;
	}
}

void Template::setIdentity(StringArray* iden) {
	if (_iden)
		delete _iden;
	_iden=iden;
}

const StringArray* Template::getIdentity() const {
	return _iden;
}

void Template::setLayout(VTypeLayout* layout) {
	if (_layout)
		delete _layout;
	_layout=layout;
}

const VTypeLayout* Template::getLayout() const {
	return _layout;
}

void Template::setInfinitism(unsigned int infinitism) {
	_infinitism=infinitism;
}

const unsigned int& Template::getInfinitism() const {
	return _infinitism;
}

void Template::setCaseSensitive(bool casesens) {
	_casesens=casesens;
}

bool Template::getCaseSensitive() const {
	return _casesens;
}

bool _checkVariable(unsigned int type, const Variable* variable) {
	if (variable && type!=VARTYPE_NONE) {
		return type&variable->getType();
	}
	return false;
}

bool _checkIden(const StringArray* iden, const UnicodeString& a, bool casesens, unsigned int i=0) {
	//printf("duct::_checkIden iden:%p\n", (void*)iden);
	if (iden) {
		//printf("duct::_checkIden count:%u i:%d\n", iden->getCount(), i);
		const UnicodeString* b;
		for (; i<iden->size(); ++i) {
			b=(*iden)[i];
			//printf("duct::_checkIden b:%p\n", (void*)b);
			debug_assert(b, "NULL identity element");
			if (casesens ? (a.compare(*b)==0) : (a.caseCompare(*b, U_FOLD_CASE_DEFAULT)==0)) {
				return true;
			}
		}
		return false;
	} else {
		return true; // NULL identity matches any name
	}
}

bool _compareNames(const UnicodeString& a, const UnicodeString& b, bool casesens) {
	if (a.length()==0 && b.length()==0) {
		return true;
	} else {
		return casesens ? (a.compare(b)==0) : (a.caseCompare(b, U_FOLD_CASE_DEFAULT)==0);
	}
}

bool __matchname(const unsigned int& mc, const StringArray* iden, const UnicodeString& name, const bool& casesens, const unsigned int& infinitism) {
	unsigned int count=iden->size();
	if (count==0) {
		return true;
	} else if (mc<count) {
		return _compareNames(*(*iden)[mc], name, casesens);
	} else if (mc>=count && (infinitism!=VARTYPE_NONE)) {
		return true;
	} else {
		return false;
	}
}

bool __matchvariable(const unsigned int& mc, const VTypeLayout* layout, const unsigned int& infinitism, const Variable* variable) {
	if (layout && (mc<layout->size())) {
		return ((*layout)[mc]&variable->getType())!=0;
	} else if (infinitism!=VARTYPE_NONE) {
		return (infinitism&variable->getType())!=0;
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
	if (pairs.size()>0) {
		const _IteratorVariablePair* pair;
		bool first=true;
		Identifier* iden=new Identifier(name, collection);
		for (_PairList::const_iterator iter=pairs.begin(); iter!=pairs.end(); ++iter) {
			pair=(_IteratorVariablePair*)(*iter);
			if (first) {
				*(pair->iter)=iden; // replace the iterator's value with the new identifier
				first=false;
			} else {
				collection->getChildren().erase(pair->iter); // erase without deleting the variable
			}
			iden->add(pair->variable); // re-own to the new identifier
		}
		addcount+=1;
	}
}

void __reset(unsigned int& mc, bool& repeatmatch, bool rep, _PairList& pairs) {
	mc=0;
	repeatmatch=rep;
	pairs.clear();
}

bool Template::validateIdentifier(const Identifier* identifier) const {
	if (identifier) {
		//printf("Template::validateIdentifier _layout:%d _iden:%d _casesens:%d _infinitism:%d\n", _layout!=NULL, _iden!=NULL, _casesens, _infinitism);
		size_t layout_size=(NULL!=_layout) ? _layout->size() : 0;
		if (!(identifier->getChildCount()>layout_size && (_infinitism==VARTYPE_NONE)) && !(identifier->getChildCount()<layout_size) && _checkIden(_iden, identifier->getName(), _casesens)) {
			// Compare defined variables in the identifier
			VarList::const_iterator iter=identifier->begin();
			for (unsigned int i=0; i<layout_size; ++i) {
				if (!((*_layout)[i]&(*iter)->getType())) {
					return false;
				}
				++iter;
			}
			// Check flexible and infinitism
			if (identifier->getChildCount()>layout_size && _infinitism!=VARTYPE_NONE) {
				for (unsigned int i=layout_size; i<identifier->getChildCount(); ++i) {
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
			if (_layout && _layout->size()>0) {
				return ((*_layout)[0]&value->getType())!=0;
			}
			// No canon types, check infinitism
			if (_infinitism!=VARTYPE_NONE) {
				return (_infinitism&value->getType())!=0;
			}
		}
	}
	return false;
}

bool Template::validateIdentity(const Variable* variable) const {
	if (variable) {
		return _checkIden(_iden, variable->getName(), _casesens);
	}
	return false;
}

unsigned int Template::compactCollection(CollectionVariable* collection, const UnicodeString& name, bool sequential) const {
	unsigned int addcount=0;
	if (collection && collection->getChildCount()>0) {
		_PairList pairs;
		bool matched; //, namematched, varmatched;
		unsigned int mc=0;
		unsigned int mmax=_iden ? _iden->size() : 0;
		mmax=(_layout ? _layout->size() : 0)>mmax ? _layout->size() : mmax;
		//printf("(Template::CompactCollection) mmax:%d\n", mmax);
		ValueVariable* value=NULL;
		bool repeatmatch=false;
		VarList::iterator iter=collection->begin();
		while (iter!=collection->end()) {
			//printf("(Template::CompactCollection) [%d] repeatmatch:%d\n", mc, repeatmatch);
			if (repeatmatch) {
				repeatmatch=false;
			} else {
				value=dynamic_cast<ValueVariable*>(*iter);
				++iter;
			}
			if (value) {
				//namematched=__matchname(mc, _iden, value->getName(), _casesens, _infinitism);
				//varmatched=__matchvariable(mc, _layout, _infinitism, value);
				//matched=namematched && varmatched;
				matched=__matchname(mc, _iden, value->getName(), _casesens, _infinitism) && __matchvariable(mc, _layout, _infinitism, value);
				//printf("(Template::CompactCollection) [%d] type:%s, name:%s, namematched:%d, varmatched:%d, (mc<mmax):%d, (_layout && mc<_layout->size()):%d\n",
				//	mc, value->getTypeAsString(), "" /*value->getName()*/, namematched, varmatched, (mc<mmax), (_layout && mc<_layout->size()));
				if (matched) {
					//printf("(Template::CompactCollection) match at mc:%d\n", mc);
					pairs.push_back(new _IteratorVariablePair(iter, value));
					mc+=1;
				} else if (mc>0 && mc!=mmax) {
					//printf("(Template::CompactCollection) unmatched before total; mc:%d\n", mc);
					if (mmax==0 || mc>mmax) {
						//printf("(Template::CompactCollection) creating from left over\n");
						__add(collection, pairs, name, addcount);
					}
					__reset(mc, repeatmatch, true, pairs);
				}
				if (mmax>0 && ((mc==mmax && (_infinitism==VARTYPE_NONE)) || (mc>0 && !(iter!=collection->end())))) {
					//printf("(Template::CompactCollection) unmatched total mc:%d, creating identifier\n", mc);
					__add(collection, pairs, name, addcount);
					__reset(mc, repeatmatch, !matched, pairs);
				}
			} else if (mc>0 && sequential) {
				//printf("(Template::CompactCollection) non-value inbetween match series mc:%d\n", mc);
				if (mmax==0 || mc>mmax) {
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
	unsigned int count=0;
	if (collection) {
		Identifier* identifier;
		for (VarList::iterator iter=collection->begin(); iter!=collection->end(); ++iter) {
			identifier=dynamic_cast<Identifier*>(*iter);
			if (identifier && validateIdentifier(identifier)) {
				identifier->setName(name);
				count+=1;
			}
		}
	}
	return count;
}

unsigned int Template::renameValues(CollectionVariable* collection, const UnicodeString& name) const {
	unsigned int count=0;
	if (collection) {
		ValueVariable* value;
		for (VarList::iterator iter=collection->begin(); iter!=collection->end(); ++iter) {
			value=dynamic_cast<ValueVariable*>(*iter);
			if (value && validateValue(value)) {
				value->setName(name);
				count+=1;
			}
		}
	}
	return count;
}

Identifier* Template::getMatchingIdentifier(const CollectionVariable* collection, bool reverse) const {
	if (collection) {
		if (reverse) {
			return (Identifier*)getMatchingVariable(collection->getChildren().rbegin(), collection->getChildren().rend(), VARTYPE_IDENTIFIER);
		} else {
			return (Identifier*)getMatchingVariable(collection->begin(), collection->end(), VARTYPE_IDENTIFIER);
		}
	}
	return NULL;
}

ValueVariable* Template::getMatchingValue(const CollectionVariable* collection, bool reverse) const {
	if (collection) {
		if (reverse) {
			return (ValueVariable*)getMatchingVariable(collection->getChildren().rbegin(), collection->getChildren().rend(), VARTYPE_VALUE);
		} else {
			return (ValueVariable*)getMatchingVariable(collection->begin(), collection->end(), VARTYPE_VALUE);
		}
	}
	return NULL;
}

} // namespace duct

