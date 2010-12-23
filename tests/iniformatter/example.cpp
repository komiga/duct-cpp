
#include "duct/debug.hpp"
#include "duct/iniformatter.hpp"

using namespace duct;

int main() {
	int exitcode = 1;
	Node* root = IniFormatter::loadFromFile("in.ini");
	if (root) {
		if (IniFormatter::writeToFile(root, "out.ini")) {
			printf("Wrote out.ini\n");
			exitcode = 0;
		} else {
			printf("Failed to write out.ini\n");
		}
		delete root;
	} else {
		printf("Failed to read in.ini\n");
	}
	return exitcode;
}

