
#include <stdio.h>
#include <unicode/unistr.h>
#include <duct/filesystem.hpp>

int main() {
	if (!duct::FileSystem::changeDir("../")) {
		printf("could not change the working directory\n");
	}
}

