
#include "duct/variables.hpp"
#include "duct/template.hpp"
#include <unicode/ustream.h>
#include <stdio.h>
#include <iostream>

using namespace duct;
using namespace std;

void printValues(const Identifier& iden) {
	printf("Variable count: %lu\n", iden.getChildCount());
	UnicodeString str;
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
	Template* tpl;
	Template* tplv;
	{
		tpl=new Template(NULL, new VTypeLayout(2, VARTYPE_INTEGER, VARTYPE_BOOL), false, VARTYPE_NONE);
		Identifier iden;
		iden.add(new IntVariable(100));
		iden.add(new BoolVariable(true));
		printf("tpl->validateIdentifier(iden): %d\n", tpl->validateIdentifier(&iden));
		
		tplv=new Template(Identity::withCStrings(1, "bool"), new VTypeLayout(1, VARTYPE_BOOL), false, VARTYPE_NONE);
		BoolVariable value(true, "bool");
		printf("tplv->validateValue(value): %d\n", tplv->validateValue(&value));
	}
	printf("tplv->getIdentity()->data[0]: %p\n", (void*)(tplv->getIdentity()->data[0]));
	delete tpl;
	delete tplv;
	return 0;
}

