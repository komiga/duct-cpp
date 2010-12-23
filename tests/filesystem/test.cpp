
#include "duct/filesystem.hpp"
#include <unicode/ustream.h>
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace duct;

void testFile(const UnicodeString& path) {
	cout << "(file) \"" << path << '\"';
	if (FileSystem::fileExists(path)) {
		cout << " exists" << endl;
	} else {
		cout << " does not exist" << endl;
	}
}

void testDir(const UnicodeString& path) {
	cout << "(dir) \"" << path << '\"';
	if (FileSystem::dirExists(path)) {
		cout << " exists" << endl;
	} else {
		cout << " does not exist" << endl;
	}
}

int main() {
	UnicodeString dirname("うううう/");
	UnicodeString filename = dirname + "ああああ.foobar";
	testDir(dirname);
	testFile(filename);
	printf("createDir:%d\n", FileSystem::createDir(dirname));
	printf("createFile:%d\n", FileSystem::createFile(filename));
	testDir(dirname);
	testFile(filename);
	
	DirStream ds(dirname);
	if (ds.isOpen()) {
		UnicodeString name;
		while (ds.nextEntry(name)) {
			printf("entry type:%d entry name:\"", ds.entryType());
			cout << name << '\"' << endl;
		}
		ds.close();
	} else {
		cout << "Could not open \"" << dirname << "\"" << endl;
	}
	
	printf("deleteFile:%d\n", FileSystem::deleteFile(filename));
	printf("deleteDir:%d\n", FileSystem::deleteDir(dirname));
	return 0;
}

