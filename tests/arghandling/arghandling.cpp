
#include <duct/arghandling.hpp>
#include <stdio.h>
#include <iostream>
#include <unicode/ustream.h>

using namespace duct;

void argsToString(const Identifier* root, UnicodeString& out) {
	out.append("\"").append(root->getName()).append("\": [");
	int count=0;
	for (VarList::const_iterator iter=root->begin(); iter!=root->end(); ++iter) {
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

int main(int argc, const char** argv) {
	Identifier* root=parseArgs(argc, argv, true, 1);
	if (root!=NULL) {
		UnicodeString out;
		argsToString(root, out);
		std::cout<<out<<"\n";
	} else {
		printf("No args");
	}
	return 0;
}

