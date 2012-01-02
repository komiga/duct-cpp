/**
@file arghandling.cpp
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

#include <duct/arghandling.hpp>

namespace duct {

Identifier* parseArgs(int argc, const char** argv, bool fullargs, int optarglimit) {
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

ArgumentHandler::ArgumentHandler() {
}

ArgumentHandler::~ArgumentHandler() {
	clear();
}

ArgImplList::iterator ArgumentHandler::begin() {
	return _list.begin();
}

ArgImplList::const_iterator ArgumentHandler::begin() const {
	return _list.begin();
}

ArgImplList::iterator ArgumentHandler::end() {
	return _list.end();
}

ArgImplList::const_iterator ArgumentHandler::end() const {
	return _list.end();
}

ArgImplList::iterator ArgumentHandler::find(const icu::UnicodeString& alias) {
	ArgImplList::iterator iter;
	for (iter=_list.begin(); iter!=_list.end(); ++iter) {
		if ((*iter)->hasAlias(alias)) {
			break;
		}
	}
	return iter;
}

ArgImplList::const_iterator ArgumentHandler::find(const icu::UnicodeString& alias) const {
	ArgImplList::const_iterator iter;
	for (iter=_list.begin(); iter!=_list.end(); ++iter) {
		if ((*iter)->hasAlias(alias)) {
			break;
		}
	}
	return iter;
}

bool ArgumentHandler::addImpl(ArgImpl* impl) {
	if (impl && impl->getAliases().size()>0) {
		_list.push_back(impl);
		return true;
	}
	return false;
}

ArgImpl* ArgumentHandler::getImpl(const icu::UnicodeString& alias) {
	ArgImplList::iterator iter=find(alias);
	if (iter!=end()) {
		return (*iter);
	}
	return NULL;
}

void ArgumentHandler::clear() {
	ArgImplList::iterator iter;
	for (iter=_list.begin(); iter!=_list.end(); ++iter) {
		delete (*iter);
	}
	_list.clear();
}

// class ArgImpl implementation

ArgImpl::ArgImpl() : _calltype(0), _args(NULL) {
}

ArgImpl::~ArgImpl() {
	_args=NULL;
}

void ArgImpl::setCallType(unsigned int calltype) {
	_calltype=calltype;
}

unsigned int ArgImpl::getCallType() const {
	return _calltype;
}

StringArray& ArgImpl::getAliases() {
	return _aliases;
}

void ArgImpl::setArgs(Identifier* args) {
	_args=args;
}

Identifier* ArgImpl::getArgs() {
	return _args;
}

bool ArgImpl::hasAlias(const icu::UnicodeString& alias) const {
	for (unsigned int i=0; i<_aliases.size(); ++i) {
		if (alias.compare(*_aliases[i])==0) {
			return true;
		}
	}
	return false;
}

} // namespace duct

