
#include "duct/template.hpp"
#include "duct/scriptformatter.hpp"
#include <unicode/ustream.h>
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace duct;

int main() {
	Template tpl_test01(Identity::withCStrings(2, "test01", "testalt01"), new VTypeLayout(3, VARTYPE_INTEGER, VARTYPE_STRING, VARTYPE_FLOAT));
	Template tpl_test02(Identity::withCStrings(2, "Test02", "TestAlt02"), new VTypeLayout(1, VARTYPE_INTEGER|VARTYPE_STRING|VARTYPE_FLOAT), true);
	Template tpl_test03(Identity::withCStrings(1, "test03"), new VTypeLayout(1, VARTYPE_STRING), false, VARTYPE_ANY);
	Template tpl_test04(Identity::withCStrings(1, "test04"), new VTypeLayout(2, VARTYPE_STRING, VARTYPE_INTEGER), false, VARTYPE_INTEGER);
	Template tpl_test05(Identity::withCStrings(1, "test05"), new VTypeLayout(1, VARTYPE_BOOL), false, VARTYPE_INTEGER|VARTYPE_STRING|VARTYPE_FLOAT);
	Template tpl_test06(Identity::withCStrings(1, "test06"), new VTypeLayout(1, VARTYPE_FLOAT), false, VARTYPE_INTEGER|VARTYPE_STRING|VARTYPE_FLOAT);
	Template tpl_test07(NULL, new VTypeLayout(2, VARTYPE_INTEGER, VARTYPE_STRING));
	Template tpl_bools(Identity::withCStrings(1, "bools"), new VTypeLayout(4, VARTYPE_BOOL, VARTYPE_BOOL, VARTYPE_BOOL, VARTYPE_BOOL));
	
	try {
		Node* root=ScriptFormatter::loadFromFile("templates.script");
		if (root) {
			UnicodeString temp;
			const Identifier* identifier;
			bool pass=false;
			for (VarList::const_iterator iter=root->begin(); iter!=root->end(); ++iter) {
				identifier=dynamic_cast<const Identifier*>(*iter);
				if (identifier) {
					temp.setTo(identifier->getName());
					temp.toLower();
					if (temp=="test01" || temp=="testalt01") { // Select the identifiers to test by-name. This isn't necessary in practice, but we're just going to do this to illustrate validation
						pass=tpl_test01.validateIdentifier(identifier);
					} else if (temp=="test02" || temp=="testalt02") {
						pass=tpl_test02.validateIdentifier(identifier);
					} else if (temp=="test03") {
						pass=tpl_test03.validateIdentifier(identifier);
					} else if (temp=="test04") {
						pass=tpl_test04.validateIdentifier(identifier);
					} else if (temp=="test05") {
						pass=tpl_test05.validateIdentifier(identifier);
					} else if (temp=="test06") {
						pass=tpl_test06.validateIdentifier(identifier);
					} else if (temp=="bools") {
						pass=tpl_bools.validateIdentifier(identifier);
					} else {
						pass=tpl_test07.validateIdentifier(identifier);
					}
					ScriptFormatter::formatIdentifier(*identifier, temp);
					cout<<"Identifier: "<<((pass) ? "passed {" : "failed {")<<temp<<"}"<<endl;
				}
			}
			delete root;
		} else {
			printf("Root node is NULL\n");
		}
	} catch (ScriptParserException& e) {
		printf("Caught exception: %s\n", e.what());
	}
	return 0;
}

