
#include <stdio.h>
#include <iostream>
#include <unicode/ustream.h>
#include <duct/arghandling.hpp>
#include "handler.hpp"

using namespace duct;

ArgumentHandler __handler;

// class HelpImpl implementation

HelpImpl::HelpImpl() {
	_aliases.set("help"); // single alias
}

int HelpImpl::checkErrors() {
	return 0;
}

int HelpImpl::execute() {
	if (_args->getChildCount()>0) {
		ArgImpl* impl=NULL;
		VarList::const_iterator iter;
		for (iter=_args->begin(); iter!=_args->end(); ++iter) {
			if ((*iter)->getType()&VARTYPE_STRING) {
				StringVariable* sv=(StringVariable*)*iter;
				if ((impl=__handler.getImpl(sv->get()))) {
					std::cout<<"usage: "<<impl->getUsage()<<std::endl;
				} else {
					std::cout<<"unknown cmd/arg: "<<sv->get()<<std::endl;
				}
			}
		}
	} else {
		printf("arguments:\n");
		ArgImplList::iterator iter;
		for (iter=__handler.begin(); iter!=__handler.end(); ++iter) {
			std::cout<<"usage: "<<(*iter)->getUsage()<<std::endl;
		}
	}
	return 0;
}

const UnicodeString& HelpImpl::getUsage() const {
	static UnicodeString help_text("help <command>");
	return help_text;
}

// class TestImpl implementation

TestImpl::TestImpl() {
	_aliases.setFromVCStrings(2, "test", "--test"); // Can run as both a command and an option, for testing purposes
}

int TestImpl::checkErrors() {
	if (_args->getChildCount()==0) { // Requires a value
		std::cout<<"error: missing value"<<std::endl<<"usage: "<<getUsage()<<std::endl;
		return -1;
	}
	return 0;
}

int TestImpl::execute() {
	UnicodeString str;
	_args->getAsString(str, 0);
	std::cout<<"test: "<<str<<std::endl;
	return 0;
}

const UnicodeString& TestImpl::getUsage() const {
	static UnicodeString __usage("test|--test <blah>");
	return __usage;
}

// other

void argsToString(const Identifier* root, UnicodeString& out) {
	out.append("\"").append(root->getName()).append("\": [");
	int count=0;
	VarList::const_iterator iter;
	for (iter=root->begin(); iter!=root->end(); ++iter) {
		const Variable* variable=*iter;
		if (variable->getType()==VARTYPE_IDENTIFIER) {
			argsToString((const Identifier*)variable, out);
			out.append(", ");
		} else if (variable->getType()&VARTYPE_VALUE) {
			const ValueVariable* vv=(ValueVariable*)variable;
			out.append(UnicodeString(vv->getTypeName())).append(": \"");
			vv->valueAsString(out, true);
			out.append("\", ");
		}
		count++;
	}
	if (count>0) {
		out.remove(out.length()-2);
	}
	out.append(']');
}

void runArgs(const Identifier* root) {
	static UChar __uchar_dashes[]={'-', '-'};
	ArgImpl* impl=NULL;
	VarList::const_iterator iter;
	for (iter=root->begin(); iter!=root->end(); ++iter) {
		if ((*iter)->getType()&VARTYPE_IDENTIFIER) {
			const UnicodeString& name=(*iter)->getName();
			if ((impl=__handler.getImpl(name))) {
				impl->setCallType(
					name.startsWith(__uchar_dashes, 1) ? CALLTYPE_SWITCH
					: (name.startsWith(__uchar_dashes, 2) ? CALLTYPE_OPTION
					: CALLTYPE_COMMAND)
				);
				impl->setArgs((Identifier*)*iter);
				if (!impl->checkErrors()) {
					impl->execute();
				}
			}
		}
	}
}

int main(int argc, const char** argv) {
	__handler.addImpl(new HelpImpl());
	__handler.addImpl(new TestImpl());
	Identifier* root=parseArgs(argc, argv, true, 1);
	if (root!=NULL) {
		UnicodeString out;
		argsToString(root, out);
		std::cout<<out<<"\n";
		runArgs(root);
		delete root;
	} else {
		printf("No arguments given\n");
	}
	return 0;
}
