
#include "duct/variables.hpp"
#include <unicode/ustdio.h>
#include <unicode/ustream.h>
#include <stdio.h>
#include <iostream>

using namespace duct;
using namespace std;

void printValues(const Identifier& iden) {
	printf("Variable count: %d\n", iden.getChildCount());
	UnicodeString str;
	for (VarList::const_iterator iter = iden.begin(); iter != iden.end(); ++iter) {
		ValueVariable* var = dynamic_cast<ValueVariable*>(*iter);
		if (var) {
			var->getValueFormatted(str, FMT_ALL_DEFAULT);
			cout << str << ", ";
			var->getNameFormatted(str, FMT_NAME_DEFAULT);
			cout << str << endl;
		}
	}
}

int main() {
	Identifier iden;
	iden.addVariable(new IntVariable(100, "integer"));
	iden.addVariable(new FloatVariable(100, "float"));
	iden.addVariable(new StringVariable("borkbork", "string"));
	iden.addVariable(new BoolVariable(true, "bool"));
	
	printValues(iden);
	printf("removing VARTYPE_BOOL: %d\n", iden.removeVariableWithType(VARTYPE_BOOL));
	printf("removing \"string\": %d\n\n", iden.removeVariableWithName("string"));
	printValues(iden);
	return 0;
}

