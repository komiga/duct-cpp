
#include "duct/scriptformatter.hpp"
#include <unicode/ustream.h>
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace duct;

void doTabs(unsigned int count) {
	while (0 < count--) {
		cout << '\t';
	}
}

void outputNode(Node* node, unsigned int tcount = 0) {
	Node* n;
	Identifier* i;
	ValueVariable* v;
	UnicodeString temp;
	for (VarList::const_iterator iter = node->begin(); iter != node->end(); ++iter) {
		n = dynamic_cast<Node*>(*iter);
		i = dynamic_cast<Identifier*>(*iter);
		v = dynamic_cast<ValueVariable*>(*iter);
		if (n) {
			doTabs(tcount);
			cout << "Name: \"" << n->getName() << "\"" << endl;
			outputNode(n, tcount + 1);
		} else if (i) {
			ScriptFormatter::formatIdentifier(*i, temp);
			doTabs(tcount);
			cout << temp << endl;
		} else if (v) {
			ScriptFormatter::formatValue(*v, temp);
			doTabs(tcount);
			//cout << "<" << v->getTypeAsString() << ">";
			cout << temp << endl;
		}
	}
}

int main() {
	try {
		Node* root = ScriptFormatter::loadFromFile("in.script");
		if (root) {
			outputNode(root);
			ScriptFormatter::writeToFile(root, "out.script");
		} else {
			printf("Root node is NULL\n");
			return 1;
		}
	} catch (ScriptParserException& e) {
		printf("Caught exception: %s\n", e.what());
		return 1;
	}
	return 0;
}

