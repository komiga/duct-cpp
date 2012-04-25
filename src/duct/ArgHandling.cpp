/**
@file ArgHandling.cpp
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

#include <duct/ArgHandling.hpp>

namespace duct {

Identifier* parseArgs(int argc, char const** argv, bool fullargs, int optarglimit) {
	if (argc<1)
		return NULL;
	Identifier* root=new Identifier();
	int i=0, length=argc-1;
	if (fullargs)
		root->setName(icu::UnicodeString(argv[i++]));
	icu::UnicodeString arg;
	Identifier* sub;
	bool subset=false;
	optarglimit=(optarglimit==-1) ? length : optarglimit;
	for (; i<=length; ++i) {
		arg=icu::UnicodeString(argv[i]);
		sub=new Identifier(arg, NULL);
		if (arg.length()>0 && arg[0]=='-') {
			if (arg.length()>1 && arg[1]=='-') {
				int lim=(length<(i+optarglimit)) ? length : (i+optarglimit);
				i++;
				while (i<=length) {
					arg=icu::UnicodeString(argv[i]);
					if (arg.length()>0 || arg[0]!='-') {
						sub->add(Variable::stringToValue(arg));
						i++;
						if (i>lim) {
							i--;
							break;
						}
					} else {
						i--;
						break;
					}
				}
			}
			root->add(sub);
		} else {
			if (!subset) {
				root->add(sub);
				root=sub;
				subset=true;
			} else {
				root->add(Variable::stringToValue(arg));
				delete sub;
				sub=NULL;
			}
		}
	}
	while (root->getParent()!=NULL) {
		root=(Identifier*)root->getParent();
	}
	return root;
}

// class ArgumentHandler

ArgumentHandler::ArgumentHandler()
	: m_list()
{/* Do nothing*/}

ArgumentHandler::~ArgumentHandler() {
	clear();
}

ArgImplList::iterator ArgumentHandler::begin() {
	return m_list.begin();
}

ArgImplList::const_iterator ArgumentHandler::begin() const {
	return m_list.begin();
}

ArgImplList::iterator ArgumentHandler::end() {
	return m_list.end();
}

ArgImplList::const_iterator ArgumentHandler::end() const {
	return m_list.end();
}

ArgImplList::iterator ArgumentHandler::find(icu::UnicodeString const& alias) {
	ArgImplList::iterator iter;
	for (iter=m_list.begin(); iter!=m_list.end(); ++iter) {
		if ((*iter)->hasAlias(alias)) {
			break;
		}
	}
	return iter;
}

ArgImplList::const_iterator ArgumentHandler::find(icu::UnicodeString const& alias) const {
	ArgImplList::const_iterator iter;
	for (iter=m_list.begin(); iter!=m_list.end(); ++iter) {
		if ((*iter)->hasAlias(alias)) {
			break;
		}
	}
	return iter;
}

bool ArgumentHandler::addImpl(ArgImpl* impl) {
	if (impl && impl->getAliases().size()>0) {
		m_list.push_back(impl);
		return true;
	}
	return false;
}

ArgImpl* ArgumentHandler::getImpl(icu::UnicodeString const& alias) {
	ArgImplList::iterator iter=find(alias);
	if (iter!=end()) {
		return (*iter);
	}
	return NULL;
}

void ArgumentHandler::clear() {
	ArgImplList::iterator iter;
	for (iter=m_list.begin(); iter!=m_list.end(); ++iter) {
		delete (*iter);
	}
	m_list.clear();
}

// class ArgImpl implementation

ArgImpl::ArgImpl()
	: m_calltype(0), m_args(NULL)
{/* Do nothing*/}

ArgImpl::~ArgImpl() {
	m_args=NULL;
}

void ArgImpl::setCallType(unsigned int calltype) {
	m_calltype=calltype;
}

unsigned int ArgImpl::getCallType() const {
	return m_calltype;
}

StringArray& ArgImpl::getAliases() {
	return m_aliases;
}

void ArgImpl::setArgs(Identifier* args) {
	m_args=args;
}

Identifier* ArgImpl::getArgs() {
	return m_args;
}

bool ArgImpl::hasAlias(icu::UnicodeString const& alias) const {
	for (unsigned int i=0; i<m_aliases.size(); ++i) {
		if (alias.compare(*m_aliases[i])==0) {
			return true;
		}
	}
	return false;
}

} // namespace duct
