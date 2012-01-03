
#include "handler.hpp"
#include <duct/arghandling.hpp>

#include <stdio.h>
#include <iostream>
#include <unicode/ustream.h>

using namespace duct;

ArgumentHandler g_handler;

// class HelpImpl implementation

HelpImpl::HelpImpl() {
	m_aliases.set("help"); // single alias
}

int HelpImpl::checkErrors() {
	return 0;
}

int HelpImpl::execute() {
	if (m_args->getChildCount()>0) {
		ArgImpl* impl=NULL;
		VarList::const_iterator iter;
		for (iter=m_args->begin(); iter!=m_args->end(); ++iter) {
			if ((*iter)->getType()&VARTYPE_STRING) {
				StringVariable* sv=(StringVariable*)*iter;
				if ((impl=g_handler.getImpl(sv->get()))) {
					std::cout<<"usage: "<<impl->getUsage()<<std::endl;
				} else {
					std::cout<<"unknown cmd/arg: "<<sv->get()<<std::endl;
				}
			}
		}
	} else {
		printf("arguments:\n");
		ArgImplList::iterator iter;
		for (iter=g_handler.begin(); iter!=g_handler.end(); ++iter) {
			std::cout<<"usage: "<<(*iter)->getUsage()<<std::endl;
		}
	}
	return 0;
}

icu::UnicodeString const& HelpImpl::getUsage() const {
	static icu::UnicodeString help_text("help <command>");
	return help_text;
}

// class TestImpl implementation

TestImpl::TestImpl() {
	m_aliases.setVCStrings(2, "test", "--test"); // Can run as both a command and an option, for testing purposes
}

int TestImpl::checkErrors() {
	if (m_args->getChildCount()==0) { // Requires a value
		std::cout<<"error: missing value"<<std::endl<<"usage: "<<getUsage()<<std::endl;
		return -1;
	}
	return 0;
}

int TestImpl::execute() {
	icu::UnicodeString str;
	m_args->getAsString(str, 0);
	std::cout<<"test: "<<str<<std::endl;
	return 0;
}

icu::UnicodeString const& TestImpl::getUsage() const {
	static icu::UnicodeString __usage("test|--test <blah>");
	return __usage;
}

// other

void argsToString(Identifier const* root, icu::UnicodeString& out) {
	out.append("\"").append(root->getName()).append("\": [");
	int count=0;
	VarList::const_iterator iter;
	for (iter=root->begin(); iter!=root->end(); ++iter) {
		Variable const* variable=*iter;
		if (variable->getType()==VARTYPE_IDENTIFIER) {
			argsToString((Identifier const*)variable, out);
			out.append(", ");
		} else if (variable->getType()&VARTYPE_VALUE) {
			ValueVariable const* vv=(ValueVariable*)variable;
			out.append(icu::UnicodeString(vv->getTypeName())).append(": \"");
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

void runArgs(Identifier const* root) {
	static UChar __uchar_dashes[]={'-', '-'};
	ArgImpl* impl=NULL;
	VarList::const_iterator iter;
	for (iter=root->begin(); iter!=root->end(); ++iter) {
		if ((*iter)->getType()&VARTYPE_IDENTIFIER) {
			icu::UnicodeString const& name=(*iter)->getName();
			if ((impl=g_handler.getImpl(name))) {
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

int main(int argc, char const** argv) {
	g_handler.addImpl(new HelpImpl());
	g_handler.addImpl(new TestImpl());
	Identifier* root=parseArgs(argc, argv, true, 1);
	if (root!=NULL) {
		icu::UnicodeString out;
		argsToString(root, out);
		std::cout<<out<<"\n";
		runArgs(root);
		delete root;
	} else {
		printf("No arguments given\n");
	}
	return 0;
}
