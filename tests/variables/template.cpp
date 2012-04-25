
#include <duct/Variables.hpp>
#include <duct/Template.hpp>

#include <stdio.h>
#include <iostream>
#include <unicode/ustream.h>

using namespace duct;
using namespace std;

void printValues(Identifier const& iden) {
	printf("Variable count: %lu\n", (unsigned long)iden.getChildCount());
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
	Template* tpl;
	Template* tplv;
	{
		tpl=new Template(NULL, new VTypeLayout(2, VARTYPE_INTEGER, VARTYPE_BOOL), false, VARTYPE_NONE);
		Identifier iden;
		iden.add(new IntVariable(100));
		iden.add(new BoolVariable(true));
		printf("tpl->validateIdentifier(iden): %d\n", tpl->validateIdentifier(&iden)); // pass
		
		tplv=new Template(StringArray::withCStrings(1, "bool"), new VTypeLayout(1, VARTYPE_BOOL), false, VARTYPE_NONE);
		BoolVariable value(true, "bool");
		printf("tplv->validateValue(value): %d\n", tplv->validateValue(&value)); // pass
	}
	printf("tplv->getIdentity()[0]: %p\n", (void*)((*tplv->getIdentity())[0]));
	delete tpl;
	delete tplv;
	return 0;
}
