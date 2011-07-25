
#include <stdio.h>
#include <iostream>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <duct/filesystem.hpp>

int main() {
	if (!duct::FileSystem::changeDir("../")) {
		printf("could not change the working directory\n");
	}
	UnicodeString wd;
	if (!duct::FileSystem::getWorkingDir(wd)) {
		printf("could not get the working directory\n");
	}
	std::cout<<"current: "<<wd<<std::endl;
}

