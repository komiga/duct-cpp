
#include <duct/arghandling.hpp>

#include <stdio.h>
#include <iostream>
#include <unicode/ustream.h>

using namespace duct;

void argsToString(Identifier const* root, icu::UnicodeString& out) {
	out.append("\"").append(root->getName()).append("\": [");
	int count=0;
	VarList::const_iterator iter;
	for (iter=root->begin(); iter!=root->end(); ++iter) {
		const Variable* variable=*iter;
		if (variable->getType()==VARTYPE_IDENTIFIER) {
			argsToString((Identifier const*)variable, out);
			out.append(", ");
		} else if (variable->getType()&VARTYPE_VALUE) {
			const ValueVariable* vv=(ValueVariable*)variable;
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

int main(int argc, char const** argv) {
	Identifier* root=parseArgs(argc, argv, true, 1);
	if (root!=NULL) {
		icu::UnicodeString out;
		argsToString(root, out);
		std::cout<<out<<"\n";
		delete root;
	} else {
		printf("No args\n");
	}
	return 0;
}

