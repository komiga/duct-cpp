
#include <duct/variables.hpp>

#include <stdio.h>
#include <iostream>
#include <unicode/ustdio.h>
#include <unicode/ustream.h>

using namespace duct;
using namespace std;

void printValues(Identifier const& iden) {
	printf("Variable count: %lu\n", iden.getChildCount());
	icu::UnicodeString str;
	for (VarList::const_iterator iter=iden.begin(); iter!=iden.end(); ++iter) {
		ValueVariable* var=dynamic_cast<ValueVariable*>(*iter);
		if (var) {
			var->getValueFormatted(str, FMT_ALL_DEFAULT);
			cout<<str<<", ";
			var->getNameFormatted(str, FMT_NAME_DEFAULT);
			cout<<str<<endl;
		}
	}
}

int main() {
	Identifier iden;
	iden.add(new IntVariable(100, "integer"));
	iden.add(new FloatVariable(100, "float"));
	iden.add(new StringVariable("borkbork", "string"));
	iden.add(new BoolVariable(true, "bool"));
	
	printValues(iden);
	Identifier* clone=(Identifier*)iden.clone();
	printValues(*clone);
	delete clone;
	return 0;
}

