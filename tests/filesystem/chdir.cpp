
#include <duct/filesystem.hpp>

#include <stdio.h>
#include <string>
#include <iostream>
#include <unicode/unistr.h>
#include <unicode/ustream.h>

int main() {
	if (!duct::FileSystem::changeDir("../")) {
		printf("could not change the working directory\n");
	}
	icu::UnicodeString wd;
	if (!duct::FileSystem::getWorkingDir(wd, true)) {
		printf("could not get the working directory\n");
	}
	std::cout<<"current: "<<wd<<std::endl;
	std::string wd_std;
	if (!duct::FileSystem::getWorkingDir(wd_std, false)) {
		printf("could not get the working directory\n");
	}
	std::cout<<"current, std::string without trailing slash: "<<wd_std<<std::endl;
}

