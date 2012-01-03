/**
@file template.cpp
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
#include <duct/template.hpp>

#include <list>

namespace duct {

// class Template implementation

Template::Template()
	: m_iden(NULL), m_layout(NULL), m_casesens(false), m_infinitism(VARTYPE_NONE) {
}

Template::Template(StringArray* iden, VTypeLayout* layout, bool casesens, unsigned int infinitism)
	: m_iden(iden), m_layout(layout), m_casesens(casesens), m_infinitism(infinitism) {
}

Template::~Template() {
	if (m_iden) {
		delete m_iden;
		m_iden=NULL;
	}
	if (m_layout) {
		delete m_layout;
		m_layout=NULL;
	}
}

void Template::setIdentity(StringArray* iden) {
	if (m_iden)
		delete m_iden;
	m_iden=iden;
}

StringArray const* Template::getIdentity() const {
	return m_iden;
}

void Template::setLayout(VTypeLayout* layout) {
	if (m_layout)
		delete m_layout;
	m_layout=layout;
}

VTypeLayout const* Template::getLayout() const {
	return m_layout;
}

void Template::setInfinitism(unsigned int infinitism) {
	m_infinitism=infinitism;
}

unsigned int Template::getInfinitism() const {
	return m_infinitism;
}

void Template::setCaseSensitive(bool casesens) {
	m_casesens=casesens;
}

bool Template::getCaseSensitive() const {
	return m_casesens;
}

bool _checkVariable(unsigned int type, Variable const* variable) {
	if (variable && type!=VARTYPE_NONE) {
		return type&variable->getType();
	}
	return false;
}

bool _checkIden(StringArray const* iden, icu::UnicodeString const& a, bool casesens, unsigned int i=0) {
	//printf("duct::_checkIden iden:%p\n", (void*)iden);
	if (iden) {
		//printf("duct::_checkIden count:%u i:%d\n", iden->getCount(), i);
		icu::UnicodeString const* b;
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

bool compareNames_(icu::UnicodeString const& a, icu::UnicodeString const& b, bool casesens) {
	if (a.length()==0 && b.length()==0) {
		return true;
	} else {
		return casesens ? (a.compare(b)==0) : (a.caseCompare(b, U_FOLD_CASE_DEFAULT)==0);
	}
}

bool matchname__(unsigned int const mc, StringArray const* iden, icu::UnicodeString const& name, bool const casesens, unsigned int const infinitism) {
	unsigned int count=iden->size();
	if (count==0) {
		return true;
	} else if (mc<count) {
		return compareNames_(*(*iden)[mc], name, casesens);
	} else if (mc>=count && (infinitism!=VARTYPE_NONE)) {
		return true;
	} else {
		return false;
	}
}

bool matchvariable__(unsigned int const mc, VTypeLayout const* layout, unsigned int const infinitism, Variable const* variable) {
	if (layout && (mc<layout->size())) {
		return ((*layout)[mc]&variable->getType())!=0;
	} else if (infinitism!=VARTYPE_NONE) {
		return (infinitism&variable->getType())!=0;
	}
	return false;
}

class IteratorVariablePair_ {
public:
	IteratorVariablePair_(VarList::iterator i, Variable* v) : iter(i), variable(v) {
	};
	
	VarList::iterator iter;
	Variable* variable;
};

typedef std::list<IteratorVariablePair_*> _PairList;

void add__(CollectionVariable* collection, _PairList& pairs, icu::UnicodeString const& name, unsigned int& addcount) {
	if (pairs.size()>0) {
		IteratorVariablePair_ const* pair;
		bool first=true;
		Identifier* iden=new Identifier(name, collection);
		for (_PairList::const_iterator iter=pairs.begin(); iter!=pairs.end(); ++iter) {
			pair=(IteratorVariablePair_*)(*iter);
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

bool Template::validateIdentifier(Identifier const* identifier) const {
	if (identifier) {
		//printf("Template::validateIdentifier m_layout:%d m_iden:%d m_casesens:%d m_infinitism:%d\n", m_layout!=NULL, m_iden!=NULL, m_casesens, m_infinitism);
		size_t layout_size=(NULL!=m_layout) ? m_layout->size() : 0;
		if (!(identifier->getChildCount()>layout_size && (m_infinitism==VARTYPE_NONE)) && !(identifier->getChildCount()<layout_size) && _checkIden(m_iden, identifier->getName(), m_casesens)) {
			// Compare defined variables in the identifier
			VarList::const_iterator iter=identifier->begin();
			for (unsigned int i=0; i<layout_size; ++i) {
				if (!((*m_layout)[i]&(*iter)->getType())) {
					return false;
				}
				++iter;
			}
			// Check flexible and infinitism
			if (identifier->getChildCount()>layout_size && m_infinitism!=VARTYPE_NONE) {
				for (unsigned int i=layout_size; i<identifier->getChildCount(); ++i) {
					if (!_checkVariable(m_infinitism, (*iter))) {
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

bool Template::validateValue(ValueVariable const* value) const {
	if (value) {
		if (_checkIden(m_iden, value->getName(), m_casesens)) {
			if (m_layout && m_layout->size()>0) {
				return ((*m_layout)[0]&value->getType())!=0;
			}
			// No canon types, check infinitism
			if (m_infinitism!=VARTYPE_NONE) {
				return (m_infinitism&value->getType())!=0;
			}
		}
	}
	return false;
}

bool Template::validateIdentity(Variable const* variable) const {
	if (variable) {
		return _checkIden(m_iden, variable->getName(), m_casesens);
	}
	return false;
}

unsigned int Template::compactCollection(CollectionVariable* collection, icu::UnicodeString const& name, bool sequential) const {
	unsigned int addcount=0;
	if (collection && collection->getChildCount()>0) {
		_PairList pairs;
		bool matched; //, namematched, varmatched;
		unsigned int mc=0;
		unsigned int mmax=m_iden ? m_iden->size() : 0;
		mmax=(m_layout ? m_layout->size() : 0)>mmax ? m_layout->size() : mmax;
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
				//namematched=matchname__(mc, m_iden, value->getName(), m_casesens, m_infinitism);
				//varmatched=matchvariable__(mc, m_layout, m_infinitism, value);
				//matched=namematched && varmatched;
				matched=matchname__(mc, m_iden, value->getName(), m_casesens, m_infinitism) && matchvariable__(mc, m_layout, m_infinitism, value);
				//printf("(Template::CompactCollection) [%d] type:%s, name:%s, namematched:%d, varmatched:%d, (mc<mmax):%d, (m_layout && mc<m_layout->size()):%d\n",
				//	mc, value->getTypeAsString(), "" /*value->getName()*/, namematched, varmatched, (mc<mmax), (m_layout && mc<m_layout->size()));
				if (matched) {
					//printf("(Template::CompactCollection) match at mc:%d\n", mc);
					pairs.push_back(new IteratorVariablePair_(iter, value));
					mc+=1;
				} else if (mc>0 && mc!=mmax) {
					//printf("(Template::CompactCollection) unmatched before total; mc:%d\n", mc);
					if (mmax==0 || mc>mmax) {
						//printf("(Template::CompactCollection) creating from left over\n");
						add__(collection, pairs, name, addcount);
					}
					__reset(mc, repeatmatch, true, pairs);
				}
				if (mmax>0 && ((mc==mmax && (m_infinitism==VARTYPE_NONE)) || (mc>0 && !(iter!=collection->end())))) {
					//printf("(Template::CompactCollection) unmatched total mc:%d, creating identifier\n", mc);
					add__(collection, pairs, name, addcount);
					__reset(mc, repeatmatch, !matched, pairs);
				}
			} else if (mc>0 && sequential) {
				//printf("(Template::CompactCollection) non-value inbetween match series mc:%d\n", mc);
				if (mmax==0 || mc>mmax) {
					//printf("(Template::CompactCollection) creating from left over\n");
					add__(collection, pairs, name, addcount);
				}
				__reset(mc, repeatmatch, false, pairs);
			}
		}
	}
	return addcount;
}

unsigned int Template::renameIdentifiers(CollectionVariable* collection, icu::UnicodeString const& name) const {
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

unsigned int Template::renameValues(CollectionVariable* collection, icu::UnicodeString const& name) const {
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

Identifier* Template::getMatchingIdentifier(CollectionVariable const* collection, bool reverse) const {
	if (collection) {
		if (reverse) {
			return (Identifier*)getMatchingVariable(collection->getChildren().rbegin(), collection->getChildren().rend(), VARTYPE_IDENTIFIER);
		} else {
			return (Identifier*)getMatchingVariable(collection->begin(), collection->end(), VARTYPE_IDENTIFIER);
		}
	}
	return NULL;
}

ValueVariable* Template::getMatchingValue(CollectionVariable const* collection, bool reverse) const {
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

